// TODO: figure out how to not use private detour method
#define private public
#include "DetourNavigationMesh.hpp"

#include <ArrayMesh.hpp>
#include <DetourCommon.h>
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

  if (recast_poly_mesh.nverts == 0 or recast_poly_mesh_detail.nverts == 0)
  {
    ERR_PRINT("recast pipeline built empty mesh");
    return false;
  }
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

  dtVcopy(params.bmin, recast_poly_mesh.bmin);
  dtVcopy(params.bmax, recast_poly_mesh.bmax);

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
  if (tile->header->detailVertCount != 0 or
      tile->header->polyCount != tile->header->detailMeshCount)
  {
    ERR_PRINT("tile->header->detailVertCount != 0 or tile->header->polyCount != "
              "tile->header->detailMeshCount, not implemented!");
    Godot::print(
        "tile, polyCount: {0} vertCount: {1} detailMeshCount: "
        "{2} detailVertCount: {3} detailTriCount: {4} x: {5} y: {6} l: {7} bmin: {8}, bmax: {9}",
        tile->header->polyCount,
        tile->header->vertCount,
        tile->header->detailMeshCount,
        tile->header->detailVertCount,
        tile->header->detailTriCount,
        tile->header->x,
        tile->header->y,
        tile->header->layer,
        Vector3(tile->header->bmin[0], tile->header->bmin[1], tile->header->bmin[2]),
        Vector3(tile->header->bmax[0], tile->header->bmax[1], tile->header->bmax[2]));
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
    return nullptr;
  }

  PoolVector3Array vertices;
  vertices.resize(tile->header->vertCount);
  PoolVector3Array::Write vertices_writer = vertices.write();
  for (int vertex_index = 0; vertex_index < tile->header->vertCount; vertex_index++)
  {
    const float* raw_vertex = &tile->verts[vertex_index * 3];
    vertices_writer[vertex_index].x = raw_vertex[0];
    vertices_writer[vertex_index].y = raw_vertex[1];
    vertices_writer[vertex_index].z = raw_vertex[2];
  }

  PoolIntArray indices;
  indices.resize(tile->header->detailTriCount * 3);
  PoolIntArray::Write indices_writer = indices.write();
  unsigned next_index_index = 0;
  for (int submesh_index = 0; submesh_index < tile->header->detailMeshCount; submesh_index++)
  {
    dtPoly& poly = tile->polys[submesh_index];
    dtPolyDetail& submesh = tile->detailMeshes[submesh_index];
    for (unsigned int triangle_index = 0; triangle_index < submesh.triCount; triangle_index++)
    {
      indices_writer[next_index_index++] =
          poly.verts[tile->detailTris[submesh.triBase * 4 + triangle_index * 4 + 0]];
      indices_writer[next_index_index++] =
          poly.verts[tile->detailTris[submesh.triBase * 4 + triangle_index * 4 + 2]];
      indices_writer[next_index_index++] =
          poly.verts[tile->detailTris[submesh.triBase * 4 + triangle_index * 4 + 1]];
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

void DetourNavigationMesh::deserialize_detour_nav_mesh(PoolByteArray serialized_detour_nav_mesh)
{
  // TODO: handle once multiple tiles supported
  if (serialized_detour_nav_mesh.size() == 0 or detour_nav_mesh)
  {
    return;
  }
  unsigned char* detour_nav_mesh_data =
      static_cast<unsigned char*>(dtAlloc(serialized_detour_nav_mesh.size(), DT_ALLOC_PERM));
  PoolByteArray::Read bytes_reader = serialized_detour_nav_mesh.read();
  std::copy(
      bytes_reader.ptr(),
      bytes_reader.ptr() + serialized_detour_nav_mesh.size(),
      detour_nav_mesh_data);
  std::unique_ptr<Detour::NavMesh> a_detour_nav_mesh = std::make_unique<Detour::NavMesh>();
  if (DT_SUCCESS !=
      a_detour_nav_mesh->ref().init(
          detour_nav_mesh_data, serialized_detour_nav_mesh.size(), DT_TILE_FREE_DATA))
  {
    ERR_PRINT("dtNavMesh.init() failed");
    return;
  }
  detour_nav_mesh = std::move(a_detour_nav_mesh);
}

PoolByteArray DetourNavigationMesh::serialize_detour_nav_mesh() const
{
  PoolByteArray serialized_detour_nav_mesh;
  if (not detour_nav_mesh)
  {
    return serialized_detour_nav_mesh;
  }
  // TODO: iterate tiles
  const dtMeshTile* tile = detour_nav_mesh->ref().getTile(0);
  if (tile == nullptr or tile->dataSize <= 0)
  {
    return serialized_detour_nav_mesh;
  }
  serialized_detour_nav_mesh.resize(tile->dataSize);
  PoolByteArray::Write bytes_writer = serialized_detour_nav_mesh.write();
  std::copy(tile->data, tile->data + tile->dataSize, bytes_writer.ptr());
  return serialized_detour_nav_mesh;
}
