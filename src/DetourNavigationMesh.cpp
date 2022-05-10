#define private public
#include "DetourNavigationMesh.hpp"

#include <DetourNavMeshBuilder.h>

using namespace godot;

bool DetourNavigationMesh::build_from_polygon_mesh(godot::Ref<RecastPolygonMesh> polygon_mesh)
{
  if (not polygon_mesh->is_baked())
  {
    return false;
  }
  const rcPolyMesh& recast_poly_mesh = polygon_mesh->get_recast_poly_mesh();
  const rcPolyMeshDetail& recast_poly_mesh_detail = polygon_mesh->get_recast_poly_mesh_detail();
  dtNavMeshCreateParams params{};

  params.verts = recast_poly_mesh.verts;
  params.vertCount = recast_poly_mesh.nverts;
  params.polys = recast_poly_mesh.polys;
  params.polyFlags = recast_poly_mesh.flags;
  params.polyAreas = recast_poly_mesh.areas;
  params.polyCount = recast_poly_mesh.npolys;
  params.nvp = recast_poly_mesh.nvp;

  params.detailMeshes = recast_poly_mesh_detail.meshes;
  params.detailVerts = recast_poly_mesh_detail.verts;
  params.detailVertsCount = recast_poly_mesh_detail.nverts;
  params.detailTris = recast_poly_mesh_detail.tris;
  params.detailTriCount = recast_poly_mesh_detail.ntris;

  // TODO: config
  params.walkableHeight = 0.2 * 5; // [wu]
  params.walkableRadius = 0.3 * 2; // [wu]
  params.walkableClimb = 0.2 * 1; // [wu]
  params.cs = 0.3; // [wu] >0
  params.ch = 0.2; // [wu] >0
  params.buildBvTree = false;

  unsigned char* data{nullptr};
  int data_size{0};
  if (not dtCreateNavMeshData(&params, &data, &data_size))
  {
    ERR_PRINT("dtCreateNavMeshData() failed");
    return false;
  }

  std::unique_ptr<Detour::NavMesh> a_detour_nav_mesh = std::make_unique<Detour::NavMesh>();
  if (DT_SUCCESS != a_detour_nav_mesh->ref().init(data, data_size, DT_TILE_FREE_DATA))
  {
    ERR_PRINT("dtNavMesh.init() failed");
    return false;
  }
  detour_nav_mesh = std::move(a_detour_nav_mesh);

  return true;
}

Ref<Mesh> DetourNavigationMesh::get_detailed_mesh()
{
  if (not detour_nav_mesh)
  {
    return nullptr;
  }
  // TODO: iterate tiles
  const dtMeshTile* tile = detour_nav_mesh->ref().getTile(0);
  if (tile == nullptr)
  {
    return nullptr;
  }
  Godot::print(
      "tile, polyCount: {0} vertCount: {1} detailMeshCount: "
      "{2} detailVertCount: {3} detailTriCount: {4}",
      tile->header->polyCount,
      tile->header->vertCount,
      tile->header->detailMeshCount,
      tile->header->detailVertCount,
      tile->header->detailTriCount);
  for (int i = 0; i < tile->header->detailMeshCount; i++)
  {
    dtPolyDetail& sm = tile->detailMeshes[i];
    Godot::print(
        "submesh, vertBase: {0}, triBase: {1}, vertCount: {2}, triCount: {3}",
        sm.vertBase,
        sm.triBase,
        sm.vertCount,
        sm.triCount);
  }
  // TODO: jeżeli tile->header->detailVertCount != 0 to fail not implemented
  // TODO: dodać vertsy z float * verts [Size: dtMeshHeader::vertCount]
  // TODO: iterować submeshe:
  //       -> dodawać triangle
  return nullptr;
}
