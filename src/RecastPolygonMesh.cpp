#include "RecastPolygonMesh.hpp"

#include <string>

#include <ArrayMesh.hpp>

#include "RecastContext.hpp"

using namespace godot;

bool RecastPolygonMesh::build_from_input_geometry(
    godot::Ref<InputGeometry> input_geometry,
    godot::Ref<RecastPolygonMeshConfig> config)
{
  auto triangles = input_geometry->get_ccw_triangles();
  PoolVector3Array vertices = triangles[Mesh::ARRAY_VERTEX];
  PoolIntArray indices = triangles[Mesh::ARRAY_INDEX];

  // unpack Vector3s into flat floats
  const int vertices_num = vertices.size();
  float raw_vertices[vertices_num * 3] = {0};
  PoolVector3Array::Read vertices_reader = vertices.read();
  for (int vertex_index = 0; vertex_index < vertices_num; vertex_index++)
  {
    raw_vertices[vertex_index * 3 + 0] = vertices_reader[vertex_index].x;
    raw_vertices[vertex_index * 3 + 1] = vertices_reader[vertex_index].y;
    raw_vertices[vertex_index * 3 + 2] = vertices_reader[vertex_index].z;
  }

  // access raw indices
  PoolIntArray::Read indices_reader = indices.read();
  const int* raw_indices = indices_reader.ptr();

  const int triangles_num = indices.size() / 3;
  return build_from_raw_triangles(raw_vertices, vertices_num, raw_indices, triangles_num, config);
}

bool RecastPolygonMesh::build_from_raw_triangles(
    const float* vertices,
    const int vertices_num,
    const int* triangles,
    const int triangles_num,
    Ref<RecastPolygonMeshConfig> config)
{
  if (config->pipeline_logs)
  {
    Godot::print(
        "RecastPolygonMesh::build_from_raw_triangles(,vertices_num={0},,triangles_num={1})",
        vertices_num,
        triangles_num);
  }

  std::unique_ptr<recast::Heightfield> height_field{std::make_unique<recast::Heightfield>()};
  std::unique_ptr<recast::CompactHeightfield> compact_height_field{
      std::make_unique<recast::CompactHeightfield>()};
  std::unique_ptr<recast::ContourSet> contour_set{std::make_unique<recast::ContourSet>()};
  std::unique_ptr<recast::PolyMesh> poly_mesh{std::make_unique<recast::PolyMesh>()};
  std::unique_ptr<recast::PolyMeshDetail> poly_mesh_detail{
      std::make_unique<recast::PolyMeshDetail>()};
  if (height_field->ptr() == nullptr or compact_height_field->ptr() == nullptr or
      contour_set->ptr() == nullptr or poly_mesh->ptr() == nullptr or
      poly_mesh_detail->ptr() == nullptr)
  {
    ERR_PRINT("Cannot allocate basic recast structures");
    return false;
  }

  RecastContext recast_context;

  int grid_width;
  int grid_height;
  float bmin[3];
  float bmax[3];

  if (config->custom_aabb_enabled)
  {
    bmin[0] = config->bmin.x;
    bmin[1] = config->bmin.y;
    bmin[2] = config->bmin.z;
    bmax[0] = config->bmax.x;
    bmax[1] = config->bmax.y;
    bmax[2] = config->bmax.z;
  }
  else
  {
    rcCalcBounds(vertices, vertices_num, bmin, bmax);
  }

  if (config->pipeline_logs)
  {
    Godot::print(
        "RecastPolygonMesh::build_from_raw_triangles(): bmin: ({0},{1},{2}), bmax: ({3},{4},{5})",
        bmin[0],
        bmin[1],
        bmin[2],
        bmax[0],
        bmax[1],
        bmax[2]);
  }

  rcCalcGridSize(bmin, bmax, config->cell_size, &grid_width, &grid_height);

  if (config->pipeline_logs)
  {
    Godot::print(
        "RecastPolygonMesh::build_from_raw_triangles(): grid w: {0}, h: {1}",
        grid_width,
        grid_height);
  }

  if (not rcCreateHeightfield(
          &recast_context,
          height_field->ref(),
          grid_width,
          grid_height,
          bmin,
          bmax,
          config->cell_size,
          config->cell_height))
  {
    ERR_PRINT("rcCreateHeightfield() failed");
    return false;
  }

  unsigned char areas[triangles_num] = {0};
  memset(areas, 0, triangles_num * sizeof(unsigned char));
  rcMarkWalkableTriangles(
      &recast_context,
      config->walkable_slope_angle,
      vertices,
      vertices_num,
      triangles,
      triangles_num,
      areas);

  if (not rcRasterizeTriangles(
          &recast_context,
          vertices,
          vertices_num,
          triangles,
          areas,
          triangles_num,
          height_field->ref(),
          config->walkable_climb))
  {
    ERR_PRINT("rcRasterizeTriangles() failed");
    return false;
  }

  if (config->filter_low_hanging_walkable_obstacles)
  {
    rcFilterLowHangingWalkableObstacles(
        &recast_context, config->walkable_climb, height_field->ref());
  }

  if (config->filter_ledge_spans)
  {
    rcFilterLedgeSpans(
        &recast_context, config->walkable_height, config->walkable_climb, height_field->ref());
  }

  if (config->filter_walkable_low_height_spans)
  {
    rcFilterWalkableLowHeightSpans(&recast_context, config->walkable_height, height_field->ref());
  }

  if (not rcBuildCompactHeightfield(
          &recast_context,
          config->walkable_height,
          config->walkable_climb,
          height_field->ref(),
          compact_height_field->ref()))
  {
    ERR_PRINT("rcBuildCompactHeightfield() failed");
    return false;
  }

  if (config->pipeline_logs)
  {
    Godot::print(
        "RecastPolygonMesh::build_from_raw_triangles(): compact height field's spanCount: {0}",
        compact_height_field->ref().spanCount);
  }

  if (not rcErodeWalkableArea(
          &recast_context, config->walkable_radius, compact_height_field->ref()))
  {
    ERR_PRINT("rcErodeWalkableArea() failed");
    return false;
  }

  const int border_size = 0;
  switch (config->partitioning_algorithm)
  {
    case RecastPolygonMeshConfig::PARTITIONING_ALGORITHM_WATERSHED:
    {
      if (not rcBuildDistanceField(&recast_context, compact_height_field->ref()))
      {
        ERR_PRINT("rcBuildDistanceField() failed");
        return false;
      }
      if (not rcBuildRegions(
              &recast_context,
              compact_height_field->ref(),
              border_size,
              config->min_region_area,
              config->merge_region_area))
      {
        ERR_PRINT("rcBuildRegions() failed");
        return false;
      }
      break;
    }
    case RecastPolygonMeshConfig::PARTITIONING_ALGORITHM_MONOTONE:
    {
      if (not rcBuildRegionsMonotone(
              &recast_context,
              *compact_height_field->ptr(),
              border_size,
              config->min_region_area,
              config->merge_region_area))
      {
        ERR_PRINT("rcBuildRegionsMonotone() failed");
        return false;
      }
      break;
    }
    case RecastPolygonMeshConfig::PARTITIONING_ALGORITHM_LAYERS:
    {
      if (not rcBuildLayerRegions(
              &recast_context, *compact_height_field->ptr(), border_size, config->min_region_area))
      {
        ERR_PRINT("rcBuildLayerRegions() failed");
        return false;
      }
      break;
    }
    default:
      return false;
  }

  if (config->pipeline_logs)
  {
    Godot::print(
        "RecastPolygonMesh::build_from_raw_triangles(): compact height field's spanCount: {0}",
        compact_height_field->ref().spanCount);
  }

  if (not rcBuildContours(
          &recast_context,
          compact_height_field->ref(),
          config->max_simplification_error,
          config->max_edge_len,
          contour_set->ref()))
  {
    ERR_PRINT("rcBuildContours() failed");
    return false;
  }

  if (config->pipeline_logs)
  {
    Godot::print(
        "RecastPolygonMesh::build_from_raw_triangles(): contour set's nconts: {0}",
        contour_set->ptr()->nconts);
  }

  if (not rcBuildPolyMesh(
          &recast_context, contour_set->ref(), config->max_verts_per_poly, poly_mesh->ref()))
  {
    ERR_PRINT("rcBuildPolyMesh() failed");
    return false;
  }

  if (config->pipeline_logs)
  {
    Godot::print(
        "RecastPolygonMesh::build_from_raw_triangles(): poly mesh'es nverts: {0}, npolys: {1}",
        poly_mesh->ptr()->nverts,
        poly_mesh->ptr()->npolys);
  }

  if (not rcBuildPolyMeshDetail(
          &recast_context,
          poly_mesh->ref(),
          compact_height_field->ref(),
          config->detail_sample_dist,
          config->detail_sample_max_error,
          poly_mesh_detail->ref()))
  {
    ERR_PRINT("rcBuildPolyMeshDetail() failed");
    return false;
  }

  if (config->pipeline_logs)
  {
    Godot::print(
        "RecastPolygonMesh::build_from_raw_triangles(): poly mesh detail's nverts: {0}, nmeshes: "
        "{1}",
        poly_mesh_detail->ptr()->nverts,
        poly_mesh_detail->ptr()->nmeshes);
  }

  height_field_solid = std::move(height_field);
  simple_recast_mesh = std::move(poly_mesh);
  detailed_recast_mesh = std::move(poly_mesh_detail);

  return true;
}

Ref<Mesh> RecastPolygonMesh::get_height_field_mesh() const
{
  if (height_field_solid == nullptr or height_field_solid->ref().width == 0 or
      height_field_solid->ref().height == 0)
  {
    return nullptr;
  }
  rcHeightfield& height_field = height_field_solid->ref();
  PoolVector3Array vertices;
  PoolIntArray indices;

  // TODO: refactor
  // impl taken from RecastDebugDraw.cpp duDebugDrawHeightfieldSolid()
  rcHeightfield& hf = height_field;
  const float* orig = hf.bmin;
  const float cs = hf.cs;
  const float ch = hf.ch;

  const int w = hf.width;
  const int h = hf.height;

  for (int y = 0; y < h; ++y)
  {
    for (int x = 0; x < w; ++x)
    {
      float fx = orig[0] + x * cs;
      float fz = orig[2] + y * cs;
      const rcSpan* s = hf.spans[x + y * w];
      while (s)
      {
        float minx = fx;
        float miny = orig[1] + s->smin * ch;
        float minz = fz;
        float maxx = fx + cs;
        float maxy = orig[1] + s->smax * ch;
        float maxz = fz + cs;

        vertices.append(Vector3(minx, maxy, minz));
        auto hbl = vertices.size() - 1;
        vertices.append(Vector3(minx, maxy, maxz));
        auto htl = vertices.size() - 1;
        vertices.append(Vector3(maxx, maxy, maxz));
        auto htr = vertices.size() - 1;
        vertices.append(Vector3(maxx, maxy, minz));
        auto hbr = vertices.size() - 1;
        vertices.append(Vector3(minx, miny, minz));
        auto lbl = vertices.size() - 1;
        vertices.append(Vector3(minx, miny, maxz));
        auto ltl = vertices.size() - 1;
        vertices.append(Vector3(maxx, miny, maxz));
        auto ltr = vertices.size() - 1;
        vertices.append(Vector3(maxx, miny, minz));
        auto lbr = vertices.size() - 1;

        // top:
        indices.append(hbl);
        indices.append(hbr);
        indices.append(htr);
        indices.append(htr);
        indices.append(htl);
        indices.append(hbl);

        // side: lbl - lbr
        indices.append(lbl);
        indices.append(lbr);
        indices.append(hbr);
        indices.append(hbr);
        indices.append(hbl);
        indices.append(lbl);

        // side: lbr - ltr
        indices.append(lbr);
        indices.append(ltr);
        indices.append(htr);
        indices.append(htr);
        indices.append(hbr);
        indices.append(lbr);

        // side: ltr - ltl
        indices.append(ltr);
        indices.append(ltl);
        indices.append(htl);
        indices.append(htl);
        indices.append(htr);
        indices.append(ltr);

        // side: ltl - lbl
        indices.append(ltl);
        indices.append(lbl);
        indices.append(hbl);
        indices.append(hbl);
        indices.append(htl);
        indices.append(ltl);

        s = s->next;
      }
    }
  }

  Array arrays;
  arrays.resize(Mesh::ARRAY_MAX);
  arrays[Mesh::ARRAY_VERTEX] = vertices;
  arrays[Mesh::ARRAY_INDEX] = indices;
  Ref<ArrayMesh> resulting_mesh = ArrayMesh::_new();
  resulting_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
  return resulting_mesh;
}

Ref<Mesh> RecastPolygonMesh::get_poly_mesh() const
{
  if (simple_recast_mesh == nullptr or simple_recast_mesh->ref().nverts <= 0)
  {
    return nullptr;
  }
  rcPolyMesh& poly_mesh = simple_recast_mesh->ref();
  PoolVector3Array vertices;
  PoolIntArray indices;

  // TODO: refactor
  // impl taken from RecastDebugDraw.cpp duDebugDrawPolyMesh()
  const int nvp = poly_mesh.nvp;
  const float cs = poly_mesh.cs;
  const float ch = poly_mesh.ch;
  const float* orig = poly_mesh.bmin;
  for (int polygon_index = 0; polygon_index < poly_mesh.npolys; ++polygon_index)
  {
    const unsigned short* p = &poly_mesh.polys[polygon_index * poly_mesh.nvp * 2];

    unsigned short vi[3];
    for (int j = 2; j < nvp; ++j)
    {
      if (p[j] == RC_MESH_NULL_IDX)
        break;
      vi[0] = p[0];
      vi[1] = p[j - 1];
      vi[2] = p[j];
      for (int k = 0; k < 3; ++k)
      {
        const unsigned short* v = &poly_mesh.verts[vi[k] * 3];
        const float x = orig[0] + v[0] * cs;
        const float y = orig[1] + (v[1] + 1) * ch;
        const float z = orig[2] + v[2] * cs;
        vertices.append(Vector3(x, y, z));
        indices.append(vertices.size() - 1);
      }
    }
  }
  for (int triangle_index = 0; triangle_index < indices.size() / 3; triangle_index++)
  {
    int tmp = indices[triangle_index * 3 + 1];
    indices.set(triangle_index * 3 + 1, indices[triangle_index * 3 + 2]);
    indices.set(triangle_index * 3 + 2, tmp);
  }

  Array arrays;
  arrays.resize(Mesh::ARRAY_MAX);
  arrays[Mesh::ARRAY_VERTEX] = vertices;
  arrays[Mesh::ARRAY_INDEX] = indices;
  Ref<ArrayMesh> resulting_mesh = ArrayMesh::_new();
  resulting_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
  return resulting_mesh;
}

Ref<Mesh> RecastPolygonMesh::get_poly_mesh_detail() const
{
  if (detailed_recast_mesh == nullptr or detailed_recast_mesh->ref().nverts <= 0)
  {
    return nullptr;
  }
  rcPolyMeshDetail& poly_mesh_detail = detailed_recast_mesh->ref();

  PoolVector3Array vertices;
  vertices.resize(poly_mesh_detail.nverts);
  PoolVector3Array::Write vertices_writer = vertices.write();
  for (int vertex_index = 0; vertex_index < poly_mesh_detail.nverts; vertex_index++)
  {
    const float* raw_vertex = &poly_mesh_detail.verts[vertex_index * 3];
    vertices_writer[vertex_index].x = raw_vertex[0];
    vertices_writer[vertex_index].y = raw_vertex[1];
    vertices_writer[vertex_index].z = raw_vertex[2];
  }

  // TODO: consider creating separate surfaces for each submesh
  PoolIntArray indices;
  indices.resize(poly_mesh_detail.ntris * 3);
  PoolIntArray::Write indices_writer = indices.write();
  unsigned next_index_index = 0;
  for (int submesh_index = 0; submesh_index < poly_mesh_detail.nmeshes; submesh_index++)
  {
    const unsigned int* submesh = &poly_mesh_detail.meshes[submesh_index * 4];
    const unsigned int& bverts = submesh[0];
    const unsigned int& btris = submesh[2];
    const unsigned int& ntris = submesh[3];
    const unsigned char* tris = &poly_mesh_detail.tris[btris * 4];
    for (unsigned int triangle_index = 0; triangle_index < ntris; triangle_index++)
    {
      indices_writer[next_index_index++] = bverts + tris[triangle_index * 4 + 0];
      indices_writer[next_index_index++] = bverts + tris[triangle_index * 4 + 2];
      indices_writer[next_index_index++] = bverts + tris[triangle_index * 4 + 1];
    }
  }

  Array arrays;
  arrays.resize(Mesh::ARRAY_MAX);
  arrays[Mesh::ARRAY_VERTEX] = vertices;
  arrays[Mesh::ARRAY_INDEX] = indices;
  Ref<ArrayMesh> resulting_mesh = ArrayMesh::_new();
  resulting_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
  return resulting_mesh;
}
