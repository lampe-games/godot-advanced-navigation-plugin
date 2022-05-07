#include "RecastPolygonMesh.hpp"

#include <string>

#include <ArrayMesh.hpp>

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

class RecastContext : public rcContext
{
 public:
  void doLog(const rcLogCategory /*category*/, const char* msg, const int /*len*/)
  {
    Godot::print("RecastContext::doLog(): {0}", msg);
  }
  // void doStartTimer(const rcTimerLabel label) {
  //   Godot::print("RecastContext::doStartTimer() {0}", label);
  // }
  // void doStopTimer(const rcTimerLabel label) {
  //   Godot::print("RecastContext::doStartTimer() {0}", label);
  // }
};

void RecastPolygonMesh::build_from_plane_mesh(Ref<PlaneMesh> plane_mesh)
{
  Godot::print("RecastPolygonMesh::build_from_plane_mesh()");

  Array mesh_arrays = plane_mesh->get_mesh_arrays();
  PoolVector3Array mesh_vertices = mesh_arrays[Mesh::ARRAY_VERTEX];
  PoolVector3Array::Read xvertices = mesh_vertices.read();
  PoolIntArray mesh_indices = mesh_arrays[Mesh::ARRAY_INDEX];
  PoolIntArray::Read indices = mesh_indices.read();

  const int vertices_num = mesh_vertices.size();
  float vertices[vertices_num * 3] = {0};
  unsigned nextNewVertexIndex = 0;
  for (unsigned vertexIndex = 0; vertexIndex < vertices_num; vertexIndex++)
  {
    vertices[nextNewVertexIndex++] = xvertices[vertexIndex].x;
    vertices[nextNewVertexIndex++] = xvertices[vertexIndex].y;
    vertices[nextNewVertexIndex++] = xvertices[vertexIndex].z;
  }
  const int triangles_num = mesh_indices.size() / 3;
  PoolIntArray reordered_mesh_indices;
  for (unsigned indexIndex = 0; indexIndex < mesh_indices.size() / 3; indexIndex++)
  {
    reordered_mesh_indices.append(indices[indexIndex * 3 + 0]);
    reordered_mesh_indices.append(indices[indexIndex * 3 + 2]);
    reordered_mesh_indices.append(indices[indexIndex * 3 + 1]);
  }
  PoolIntArray::Read reordered_indices = reordered_mesh_indices.read();
  // const int* triangles = reordered_indices.ptr();
  const int* triangles = reordered_indices.ptr();
  Godot::print(
      "RecastPolygonMesh::build_from_plane_mesh() vertices: {0}, indices: {1}, rindices: {2}, {3}, "
      "{4}, {5}",
      mesh_vertices.size(),
      mesh_indices.size(),
      reordered_mesh_indices.size(),
      mesh_vertices,
      mesh_indices,
      reordered_mesh_indices);

  // const int vertices_num = 4;
  // float vertices[] = {0, 0, 0, 5, 0, 0, 0, 0, -5, 0, 0, 5};
  // const int triangles_num = 2;
  // int triangles[] = {0, 1, 2, 0, 3, 1};

  // const int vertices_num = 4;
  // float vertices[] = {5, 0, 5, -5, 0, 5, 5, 0, -5, -5, 0, -5};
  // const int triangles_num = 2;
  // int triangles[] = {0, 2, 1, 1, 2, 3};

  for (int i = 0; i < vertices_num; i++)
  {
    Godot::print(
        "wtf: {0}, {1}, {2}", vertices[i * 3 + 0], vertices[i * 3 + 1], vertices[i * 3 + 2]);
  }
  Godot::print(
      "wtf: {0}, {1}, {2}, {3}, {4}, {5}",
      triangles[0],
      triangles[1],
      triangles[2],
      triangles[3],
      triangles[4],
      triangles[5]);

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
    return;
  }

  // rcContext recast_context;
  RecastContext recast_context;
  // recast_context.enableLog(true);
  // recast_context.log(RC_LOG_WARNING, "xxx");

  // rcConfig recast_config;
  // memset(&recast_config, 0, sizeof(recast_config));
  // // recast_config.width = 64;
  // // recast_config.height = 64;
  // recast_config.tileSize = 64;
  // recast_config.borderSize = 0;
  // recast_config.cs = 0.3;
  // recast_config.ch = 0.3;
  // // recast_config.bmin[0] = -10;
  // // recast_config.bmin[1] = -10;
  // // recast_config.bmin[2] = -10;
  // // recast_config.bmax[0] = 10;
  // // recast_config.bmax[1] = 10;
  // // recast_config.bmax[2] = 10;
  // recast_config.walkableSlopeAngle = 45; // [dg]
  // recast_config.walkableHeight = 7; // [vx], ceil (2[agent_height] / 0.3[ch])
  // recast_config.walkableClimb = 1; // [vx]
  // recast_config.walkableRadius = 2; // [vx]
  // recast_config.maxEdgeLen = 40; // [vx], 12/0.3[ch]
  // recast_config.maxSimplificationError = 1.3; // [vx]
  // recast_config.minRegionArea = 2; // [vx]
  // recast_config.mergeRegionArea = 4; // [vx]
  // recast_config.maxVertsPerPoly = 6;
  // recast_config.detailSampleDist = 1.8; // [wu]
  // recast_config.detailSampleMaxError = 0.3; // [wu]

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
  Godot::print(
      "RecastPolygonMesh::build_from_plane_mesh() bmin: ({0},{1},{2}), bmax: ({3},{4},{5})",
      recast_config.bmin[0],
      recast_config.bmin[1],
      recast_config.bmin[2],
      recast_config.bmax[0],
      recast_config.bmax[1],
      recast_config.bmax[2]);

  rcCalcGridSize(
      recast_config.bmin,
      recast_config.bmax,
      recast_config.cs,
      &recast_config.width,
      &recast_config.height);
  Godot::print((std::string("RecastPolygonMesh::build_from_plane_mesh() w:") +
                std::to_string(recast_config.width) + std::string(", h:") +
                std::to_string(recast_config.height))
                   .c_str());

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
    return;
  }

  Godot::print(
      "RecastPolygonMesh::build_from_plane_mesh() hf: {0}, {1}, {2}, {3}",
      height_field->ref().width,
      height_field->ref().height,
      height_field->ref().cs,
      height_field->ref().ch);

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
  // unsigned char areas[] = {1, 2};

  Godot::print(
      "RecastPolygonMesh::build_from_plane_mesh() vertices_num: {0}, triangles_num: {1}, {2} {3} "
      "{4}",
      vertices_num,
      triangles_num,
      vertices[0],
      vertices[1],
      vertices[2]);
  // if (not rcRasterizeTriangles(
  //         &recast_context,
  //         vertices,
  //         areas,
  //         triangles_num,
  //         height_field->ref(),
  //         recast_config.walkableClimb))
  // {
  //   ERR_PRINT("rcRasterizeTriangles() failed");
  //   return;
  // }
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
    return;
  }

  // rcContext* ctx = &recast_context;
  // ctx->log(RC_LOG_WARNING, "xxx2");

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
    return;
  }

  Godot::print(
      "RecastPolygonMesh::build_from_plane_mesh() chf_1: {0}",
      compact_height_field->ref().spanCount);

  if (not rcErodeWalkableArea(
          &recast_context, recast_config.walkableRadius, compact_height_field->ref()))
  {
    ERR_PRINT("rcErodeWalkableArea() failed");
    return;
  }

  // TODO: implement all partitioning types
  if (not rcBuildDistanceField(&recast_context, compact_height_field->ref()))
  {
    ERR_PRINT("rcBuildDistanceField() failed");
    return;
  }
  if (not rcBuildRegions(
          &recast_context,
          compact_height_field->ref(),
          0,
          recast_config.minRegionArea,
          recast_config.mergeRegionArea))
  {
    ERR_PRINT("rcBuildRegions() failed");
    return;
  }
  // if (not !rcBuildRegionsMonotone(
  //         &recast_context,
  //         *compact_height_field->ptr(),
  //         0,
  //         recast_config.minRegionArea,
  //         recast_config.mergeRegionArea))
  // {
  //   ERR_PRINT("rcBuildRegionsMonotone() failed");
  //   return;
  // }
  Godot::print(
      "RecastPolygonMesh::build_from_plane_mesh() chf_2: {0}",
      compact_height_field->ref().spanCount);

  if (not rcBuildContours(
          &recast_context,
          compact_height_field->ref(),
          recast_config.maxSimplificationError,
          recast_config.maxEdgeLen,
          contour_set->ref()))
  {
    ERR_PRINT("rcBuildContours() failed");
    return;
  }

  Godot::print("RecastPolygonMesh::build_from_plane_mesh: nconts: {0}", contour_set->ptr()->nconts);

  if (not rcBuildPolyMesh(
          &recast_context, contour_set->ref(), recast_config.maxVertsPerPoly, poly_mesh->ref()))
  {
    ERR_PRINT("rcBuildPolyMesh() failed");
    return;
  }

  Godot::print(
      "RecastPolygonMesh::build_from_plane_mesh: nverts: {0}, npolys: {1}",
      poly_mesh->ptr()->nverts,
      poly_mesh->ptr()->npolys);

  if (not rcBuildPolyMeshDetail(
          &recast_context,
          poly_mesh->ref(),
          compact_height_field->ref(),
          recast_config.detailSampleDist,
          recast_config.detailSampleMaxError,
          poly_mesh_detail->ref()))
  {
    ERR_PRINT("rcBuildPolyMeshDetail() failed");
    return;
  }
  Godot::print(
      "RecastPolygonMesh::build_from_plane_mesh: nverts: {0}, nmeshes: {1}",
      poly_mesh_detail->ptr()->nverts,
      poly_mesh_detail->ptr()->nmeshes);
  simple_recast_mesh = std::move(poly_mesh);
  detailed_recast_mesh = std::move(poly_mesh_detail);
}

Ref<Mesh> RecastPolygonMesh::get_mesh() const
{
  Godot::print("RecastPolygonMesh::get_mesh()");
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
    // return PlaneMesh::_new();
    return resulting_mesh;
  }
  return nullptr;
}
