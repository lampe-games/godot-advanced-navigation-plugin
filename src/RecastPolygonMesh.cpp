#include "RecastPolygonMesh.hpp"

#include <string>

#include <ArrayMesh.hpp>

#include "RecastContext.hpp"

// #define DEBUG_LOGS

using namespace godot;

RecastPolygonMesh::RecastPolygonMesh()
{
  Godot::print("RecastPolygonMesh::RecastPolygonMesh()");
}

RecastPolygonMesh::~RecastPolygonMesh()
{
  Godot::print("RecastPolygonMesh::~RecastPolygonMesh()");
}

void RecastPolygonMesh::_init()
{
  Godot::print("RecastPolygonMesh::_init()");
}

bool RecastPolygonMesh::build_from_plane_mesh(Ref<PlaneMesh> plane_mesh)
{
  Array mesh_arrays = plane_mesh->get_mesh_arrays();
  // TODO: ensure no copy
  PoolVector3Array triangle_vertices = mesh_arrays[Mesh::ARRAY_VERTEX];
  PoolIntArray triangle_indices = mesh_arrays[Mesh::ARRAY_INDEX];
  return build_from_triangles(triangle_vertices, triangle_indices);
}

bool RecastPolygonMesh::build_from_triangles(PoolVector3Array& vertices, PoolIntArray& indices)
{
  // unpack Vector3s into flat floats
  const int vertices_num = vertices.size();
  float raw_vertices[vertices_num * 3] = {0};
  PoolVector3Array::Read vertices_reader = vertices.read();
  for (unsigned vertex_index = 0; vertex_index < vertices_num; vertex_index++)
  {
    raw_vertices[vertex_index * 3 + 0] = vertices_reader[vertex_index].x;
    raw_vertices[vertex_index * 3 + 1] = vertices_reader[vertex_index].y;
    raw_vertices[vertex_index * 3 + 2] = vertices_reader[vertex_index].z;
  }

  // reorder ABC triangles into ACB ones for godot-recast compatibility
  int raw_indices[indices.size()] = {0};
  const int triangles_num = indices.size() / 3;
  PoolIntArray::Read indices_reader = indices.read();
  for (unsigned triangle_index = 0; triangle_index < triangles_num; triangle_index++)
  {
    raw_indices[triangle_index * 3 + 0] = indices_reader[triangle_index * 3 + 0];
    raw_indices[triangle_index * 3 + 1] = indices_reader[triangle_index * 3 + 2];
    raw_indices[triangle_index * 3 + 2] = indices_reader[triangle_index * 3 + 1];
  }

  return build_from_raw_triangles(raw_vertices, vertices_num, raw_indices, triangles_num);
}

bool RecastPolygonMesh::build_from_raw_triangles(
    const float* vertices,
    const int vertices_num,
    const int* triangles,
    const int triangles_num)
{
#ifdef DEBUG_LOGS
  Godot::print(
      "RecastPolygonMesh::build_from_raw_triangles(,vertices_num={0},,triangles_num={1})",
      vertices_num,
      triangles_num);
#endif

  std::unique_ptr<Recast::Heightfield> height_field{std::make_unique<Recast::Heightfield>()};
  std::unique_ptr<Recast::CompactHeightfield> compact_height_field{
      std::make_unique<Recast::CompactHeightfield>()};
  std::unique_ptr<Recast::ContourSet> contour_set{std::make_unique<Recast::ContourSet>()};
  std::unique_ptr<Recast::PolyMesh> poly_mesh{std::make_unique<Recast::PolyMesh>()};
  std::unique_ptr<Recast::PolyMeshDetail> poly_mesh_detail{
      std::make_unique<Recast::PolyMeshDetail>()};
  if (height_field->ptr() == nullptr or compact_height_field->ptr() == nullptr or
      contour_set->ptr() == nullptr or poly_mesh->ptr() == nullptr or
      poly_mesh_detail->ptr() == nullptr)
  {
    ERR_PRINT("Cannot allocate basic recast structures");
    return false;
  }

  RecastContext recast_context;

  rcConfig recast_config;
  memset(&recast_config, 0, sizeof(recast_config));
  // recast_config.width = 64;
  // recast_config.height = 64;
  // recast_config.tileSize = 64;
  // recast_config.borderSize = 0;
  recast_config.cs = 0.3;
  recast_config.ch = 0.2;
  // recast_config.bmin[0] = -10;
  // recast_config.bmin[1] = -10;
  // recast_config.bmin[2] = -10;
  // recast_config.bmax[0] = 10;
  // recast_config.bmax[1] = 10;
  // recast_config.bmax[2] = 10;
  recast_config.walkableSlopeAngle = 45; // [dg]
  recast_config.walkableHeight = 10; // [vx]
  recast_config.walkableClimb = 4; // [vx]
  recast_config.walkableRadius = 2; // [vx]
  recast_config.maxEdgeLen = 40; // [vx]
  recast_config.maxSimplificationError = 1.3; // [vx]
  recast_config.minRegionArea = 64; // [vx]
  recast_config.mergeRegionArea = 20 * 20; // [vx]
  recast_config.maxVertsPerPoly = 6;
  recast_config.detailSampleDist = 1.8; // [wu]
  recast_config.detailSampleMaxError = 1.0; // [wu]

  rcCalcBounds(vertices, vertices_num, recast_config.bmin, recast_config.bmax);

#ifdef DEBUG_LOGS
  Godot::print(
      "RecastPolygonMesh::build_from_raw_triangles(): bmin: ({0},{1},{2}), bmax: ({3},{4},{5})",
      recast_config.bmin[0],
      recast_config.bmin[1],
      recast_config.bmin[2],
      recast_config.bmax[0],
      recast_config.bmax[1],
      recast_config.bmax[2]);
#endif

  rcCalcGridSize(
      recast_config.bmin,
      recast_config.bmax,
      recast_config.cs,
      &recast_config.width,
      &recast_config.height);

#ifdef DEBUG_LOGS
  Godot::print(
      "RecastPolygonMesh::build_from_raw_triangles(): grid w: {0}, h: {1}",
      recast_config.width,
      recast_config.height);
#endif

  if (not rcCreateHeightfield(
          &recast_context,
          height_field->ref(),
          recast_config.width,
          recast_config.height,
          recast_config.bmin,
          recast_config.bmax,
          recast_config.cs,
          recast_config.ch))
  {
    ERR_PRINT("rcCreateHeightfield() failed");
    return false;
  }

  unsigned char areas[triangles_num] = {0};
  memset(areas, 0, triangles_num * sizeof(unsigned char));
  rcMarkWalkableTriangles(
      &recast_context,
      recast_config.walkableSlopeAngle,
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
          recast_config.walkableClimb))
  {
    ERR_PRINT("rcRasterizeTriangles() failed");
    return false;
  }

  // TODO: make conditional
  rcFilterLowHangingWalkableObstacles(
      &recast_context, recast_config.walkableClimb, height_field->ref());
  rcFilterLedgeSpans(
      &recast_context,
      recast_config.walkableHeight,
      recast_config.walkableClimb,
      height_field->ref());
  rcFilterWalkableLowHeightSpans(
      &recast_context, recast_config.walkableHeight, height_field->ref());

  if (not rcBuildCompactHeightfield(
          &recast_context,
          recast_config.walkableHeight,
          recast_config.walkableClimb,
          height_field->ref(),
          compact_height_field->ref()))
  {
    ERR_PRINT("rcBuildCompactHeightfield() failed");
    return false;
  }

#ifdef DEBUG_LOGS
  Godot::print(
      "RecastPolygonMesh::build_from_raw_triangles(): compact height field's spanCount: {0}",
      compact_height_field->ref().spanCount);
#endif

  if (not rcErodeWalkableArea(
          &recast_context, recast_config.walkableRadius, compact_height_field->ref()))
  {
    ERR_PRINT("rcErodeWalkableArea() failed");
    return false;
  }

  // TODO: implement all partitioning types
  if (not rcBuildDistanceField(&recast_context, compact_height_field->ref()))
  {
    ERR_PRINT("rcBuildDistanceField() failed");
    return false;
  }
  if (not rcBuildRegions(
          &recast_context,
          compact_height_field->ref(),
          0,
          recast_config.minRegionArea,
          recast_config.mergeRegionArea))
  {
    ERR_PRINT("rcBuildRegions() failed");
    return false;
  }
  // if (not !rcBuildRegionsMonotone(
  //         &recast_context,
  //         *compact_height_field->ptr(),
  //         0,
  //         recast_config.minRegionArea,
  //         recast_config.mergeRegionArea))
  // {
  //   ERR_PRINT("rcBuildRegionsMonotone() failed");
  //   return false;
  // }

#ifdef DEBUG_LOGS
  Godot::print(
      "RecastPolygonMesh::build_from_raw_triangles(): compact height field's spanCount: {0}",
      compact_height_field->ref().spanCount);
#endif

  if (not rcBuildContours(
          &recast_context,
          compact_height_field->ref(),
          recast_config.maxSimplificationError,
          recast_config.maxEdgeLen,
          contour_set->ref()))
  {
    ERR_PRINT("rcBuildContours() failed");
    return false;
  }

#ifdef DEBUG_LOGS
  Godot::print(
      "RecastPolygonMesh::build_from_raw_triangles(): contour set's nconts: {0}",
      contour_set->ptr()->nconts);
#endif

  if (not rcBuildPolyMesh(
          &recast_context, contour_set->ref(), recast_config.maxVertsPerPoly, poly_mesh->ref()))
  {
    ERR_PRINT("rcBuildPolyMesh() failed");
    return false;
  }

#ifdef DEBUG_LOGS
  Godot::print(
      "RecastPolygonMesh::build_from_raw_triangles(): poly mesh'es nverts: {0}, npolys: {1}",
      poly_mesh->ptr()->nverts,
      poly_mesh->ptr()->npolys);
#endif

  if (not rcBuildPolyMeshDetail(
          &recast_context,
          poly_mesh->ref(),
          compact_height_field->ref(),
          recast_config.detailSampleDist,
          recast_config.detailSampleMaxError,
          poly_mesh_detail->ref()))
  {
    ERR_PRINT("rcBuildPolyMeshDetail() failed");
    return false;
  }

#ifdef DEBUG_LOGS
  Godot::print(
      "RecastPolygonMesh::build_from_raw_triangles(): poly mesh detail's nverts: {0}, nmeshes: {1}",
      poly_mesh_detail->ptr()->nverts,
      poly_mesh_detail->ptr()->nmeshes);
#endif

  simple_recast_mesh = std::move(poly_mesh);
  detailed_recast_mesh = std::move(poly_mesh_detail);

  return true;
}

Ref<Mesh> RecastPolygonMesh::get_poly_mesh() const
{
  if (simple_recast_mesh)
  {
    rcPolyMeshDetail* poly_mesh_detail = detailed_recast_mesh->ptr();
    PoolVector3Array vertices;
    Godot::print(
        "RecastPolygonMesh::get_mesh(): nverts: {0}, nmeshes: {1}",
        poly_mesh_detail->nverts,
        poly_mesh_detail->nmeshes);
    if (poly_mesh_detail->nverts == 0)
    {
      return nullptr;
    }
    for (int vertexIndex = 0; vertexIndex < poly_mesh_detail->nverts; vertexIndex++)
    {
      const float* v = &poly_mesh_detail->verts[vertexIndex * 3];
      vertices.append(Vector3(v[0], v[1], v[2]));
    }
    PoolIntArray indices;
    // TODO: resize & use ::write API
    for (int submeshIndex = 0; submeshIndex < poly_mesh_detail->nmeshes; submeshIndex++)
    {
      const unsigned int* m = &poly_mesh_detail->meshes[submeshIndex * 4];
      const unsigned int bverts = m[0];
      const unsigned int btris = m[2];
      const unsigned int ntris = m[3];
      const unsigned char* tris = &poly_mesh_detail->tris[btris * 4];
      for (unsigned int triangleIndex = 0; triangleIndex < ntris; triangleIndex++)
      {
        // Vector<int> nav_indices;
        // nav_indices.resize(3);
        // // Polygon order in recast is opposite than godot's
        // nav_indices.write[0] = ((int)(bverts + tris[triangleIndex * 4 + 0]));
        // nav_indices.write[1] = ((int)(bverts + tris[triangleIndex * 4 + 2]));
        // nav_indices.write[2] = ((int)(bverts + tris[triangleIndex * 4 + 1]));
        // p_nav_mesh->add_polygon(nav_indices);
        indices.append(bverts + tris[triangleIndex * 4 + 0]);
        indices.append(bverts + tris[triangleIndex * 4 + 2]);
        indices.append(bverts + tris[triangleIndex * 4 + 1]);
      }
    }
    Array arrays;
    arrays.resize(Mesh::ARRAY_MAX);
    arrays[Mesh::ARRAY_VERTEX] = vertices;
    arrays[Mesh::ARRAY_INDEX] = indices;
    Ref<ArrayMesh> resulting_mesh = ArrayMesh::_new();
    Godot::print(
        "RecastPolygonMesh::get_mesh(): #vs: {0}, #is: {1}", vertices.size(), indices.size());
    resulting_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
    return resulting_mesh;
  }
  return nullptr;
}

Ref<Mesh> RecastPolygonMesh::get_poly_mesh_detail() const
{
  if (simple_recast_mesh)
  {
    rcPolyMeshDetail* poly_mesh_detail = detailed_recast_mesh->ptr();
    PoolVector3Array vertices;
    Godot::print(
        "RecastPolygonMesh::get_mesh(): nverts: {0}, nmeshes: {1}",
        poly_mesh_detail->nverts,
        poly_mesh_detail->nmeshes);
    if (poly_mesh_detail->nverts == 0)
    {
      return nullptr;
    }
    for (int vertexIndex = 0; vertexIndex < poly_mesh_detail->nverts; vertexIndex++)
    {
      const float* v = &poly_mesh_detail->verts[vertexIndex * 3];
      vertices.append(Vector3(v[0], v[1], v[2]));
    }
    PoolIntArray indices;
    // TODO: resize & use ::write API
    for (int submeshIndex = 0; submeshIndex < poly_mesh_detail->nmeshes; submeshIndex++)
    {
      const unsigned int* m = &poly_mesh_detail->meshes[submeshIndex * 4];
      const unsigned int bverts = m[0];
      const unsigned int btris = m[2];
      const unsigned int ntris = m[3];
      const unsigned char* tris = &poly_mesh_detail->tris[btris * 4];
      for (unsigned int triangleIndex = 0; triangleIndex < ntris; triangleIndex++)
      {
        // Vector<int> nav_indices;
        // nav_indices.resize(3);
        // // Polygon order in recast is opposite than godot's
        // nav_indices.write[0] = ((int)(bverts + tris[triangleIndex * 4 + 0]));
        // nav_indices.write[1] = ((int)(bverts + tris[triangleIndex * 4 + 2]));
        // nav_indices.write[2] = ((int)(bverts + tris[triangleIndex * 4 + 1]));
        // p_nav_mesh->add_polygon(nav_indices);
        indices.append(bverts + tris[triangleIndex * 4 + 0]);
        indices.append(bverts + tris[triangleIndex * 4 + 2]);
        indices.append(bverts + tris[triangleIndex * 4 + 1]);
      }
    }
    Array arrays;
    arrays.resize(Mesh::ARRAY_MAX);
    arrays[Mesh::ARRAY_VERTEX] = vertices;
    arrays[Mesh::ARRAY_INDEX] = indices;
    Ref<ArrayMesh> resulting_mesh = ArrayMesh::_new();
    Godot::print(
        "RecastPolygonMesh::get_mesh(): #vs: {0}, #is: {1}", vertices.size(), indices.size());
    resulting_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
    return resulting_mesh;
  }
  return nullptr;
}
