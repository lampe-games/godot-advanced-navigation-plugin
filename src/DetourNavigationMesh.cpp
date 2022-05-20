// TODO: figure out how to not use private detour method
#define private public
#include "DetourNavigationMesh.hpp"

#include <string>
#include <vector>

#include <ArrayMesh.hpp>
#include <DetourCommon.h>
#include <DetourNavMeshBuilder.h>

using namespace godot;

void DetourNavigationMesh::_register_methods()
{
  register_method("build_from_input_geometry", &DetourNavigationMesh::build_from_input_geometry);
  register_method("get_closest_point", &DetourNavigationMesh::get_closest_point);

  register_property<DetourNavigationMesh, godot::PoolByteArray>(
      "serialized_detour_navigation_mesh",
      &DetourNavigationMesh::deserialize_detour_nav_mesh,
      &DetourNavigationMesh::serialize_detour_nav_mesh,
      godot::PoolByteArray(),
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_STORAGE,
      GODOT_PROPERTY_HINT_NONE);
}

bool DetourNavigationMesh::build_from_input_geometry(
    Ref<InputGeometry> input_geometry,
    Ref<RecastPolygonMeshConfig> recast_config,
    Ref<DetourNavigationMeshConfig> detour_config)
{
  Ref<RecastPolygonMesh> recast_polygon_mesh = Ref<RecastPolygonMesh>(RecastPolygonMesh::_new());
  if (not recast_polygon_mesh->build_from_input_geometry(input_geometry, recast_config))
  {
    return false;
  }
  return build_from_polygon_mesh(recast_polygon_mesh, detour_config);
}

bool DetourNavigationMesh::build_from_polygon_mesh(
    Ref<RecastPolygonMesh> polygon_mesh,
    Ref<DetourNavigationMeshConfig> config)
{
  if (not polygon_mesh->is_baked() or config.is_null())
  {
    return false; // TODO: print proper error
  }
  const rcPolyMesh& recast_poly_mesh = polygon_mesh->get_recast_poly_mesh();
  const rcPolyMeshDetail& recast_poly_mesh_detail = polygon_mesh->get_recast_poly_mesh_detail();

  if (recast_poly_mesh.nverts == 0 or recast_poly_mesh_detail.nverts == 0)
  {
    ERR_PRINT("recast pipeline has built an empty mesh");
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

  params.walkableHeight = config->walkable_height;
  params.walkableRadius = config->walkable_radius;
  params.walkableClimb = config->walkable_climb;
  params.cs = config->cell_size;
  params.ch = config->cell_height;
  params.buildBvTree = config->build_bv_tree;

  if (config->logs)
  {
    Godot::print(
        "dtNavMeshCreateParams: (...)\n| walkableHeight: {0}\n|walkableRadius: "
        "{1}\n|walkableClimb: {2}\n|cs: {3}\n| ch: {4}\n| buildBvTree: {5}",
        params.walkableHeight,
        params.walkableRadius,
        params.walkableClimb,
        params.cs,
        params.ch,
        params.buildBvTree);
  }

  unsigned char* data{nullptr};
  int data_size{0};
  if (not dtCreateNavMeshData(&params, &data, &data_size))
  {
    ERR_PRINT("dtCreateNavMeshData() failed");
    return false;
  }

  std::unique_ptr<Detour::NavMesh> a_detour_nav_mesh = std::make_unique<Detour::NavMesh>();
  std::unique_ptr<Detour::NavMeshQuery> a_detour_nav_mesh_query =
      std::make_unique<Detour::NavMeshQuery>();
  if (a_detour_nav_mesh->ptr() == nullptr or a_detour_nav_mesh_query->ptr() == nullptr)
  {
    ERR_PRINT("Cannot allocate basic detour structures");
    return false;
  }

  if (DT_SUCCESS != a_detour_nav_mesh->ref().init(data, data_size, DT_TILE_FREE_DATA))
  {
    ERR_PRINT("dtNavMesh.init() failed");
    return false;
  }

  if (DT_SUCCESS != a_detour_nav_mesh_query->ref().init(a_detour_nav_mesh->ptr(), 65535))
  {
    ERR_PRINT("dtNavMeshQuery.init() failed");
    return false;
  }

  // mark all polygons searchable (for queries)
  const dtMeshTile* tile = a_detour_nav_mesh->ref().getTile(0);
  if (tile != nullptr)
  {
    for (int polygon_index = 0; polygon_index < tile->header->polyCount; polygon_index++)
    {
      tile->polys[polygon_index].flags = POLYGON_SEARCHABLE;
    }
  }

  detour_nav_mesh = std::move(a_detour_nav_mesh);
  detour_nav_mesh_query = std::move(a_detour_nav_mesh_query);

  if (config->logs and tile != nullptr)
  {
    Godot::print(
        "dtMeshTile:\n| polyCount: {0}\n| vertCount: {1}\n| detailMeshCount: {2}\n| "
        "detailVertCount: {3}\n| detailTriCount: {4}\n| x: {5}\n| y: {6}\n| l: {7}\n| bmin: "
        "{8}\n\\ bmax: {9}",
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
  }

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

  PoolVector3Array vertices;
  vertices.resize(tile->header->vertCount + tile->header->detailVertCount);
  PoolVector3Array::Write vertices_writer = vertices.write();
  for (int vertex_index = 0; vertex_index < tile->header->vertCount; vertex_index++)
  {
    const float* raw_vertex = &tile->verts[vertex_index * 3];
    vertices_writer[vertex_index].x = raw_vertex[0];
    vertices_writer[vertex_index].y = raw_vertex[1];
    vertices_writer[vertex_index].z = raw_vertex[2];
  }
  for (int vertex_index = 0; vertex_index < tile->header->detailVertCount; vertex_index++)
  {
    const float* raw_vertex = &tile->detailVerts[vertex_index * 3];
    vertices_writer[tile->header->vertCount + vertex_index].x = raw_vertex[0];
    vertices_writer[tile->header->vertCount + vertex_index].y = raw_vertex[1];
    vertices_writer[tile->header->vertCount + vertex_index].z = raw_vertex[2];
  }

  PoolIntArray indices;
  indices.resize(tile->header->detailTriCount * 3);
  PoolIntArray::Write indices_writer = indices.write();
  unsigned next_index_index = 0;
  const unsigned index_offsets[] = {0, 2, 1};
  for (int submesh_index = 0; submesh_index < tile->header->detailMeshCount; submesh_index++)
  {
    dtPoly& poly = tile->polys[submesh_index];
    dtPolyDetail& submesh = tile->detailMeshes[submesh_index];
    for (unsigned int triangle_index = 0; triangle_index < submesh.triCount; triangle_index++)
    {
      for (unsigned index_offset_index = 0; index_offset_index < 3; index_offset_index++)
      {
        auto vertex_index =
            tile->detailTris
                [submesh.triBase * 4 + triangle_index * 4 + index_offsets[index_offset_index]];
        if (vertex_index < poly.vertCount)
        {
          indices_writer[next_index_index++] = poly.verts[vertex_index];
        }
        else
        {
          indices_writer[next_index_index++] =
              tile->header->vertCount + (submesh.vertBase + (vertex_index - poly.vertCount));
        }
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

Vector3 DetourNavigationMesh::get_closest_point(Vector3 target_point) const
{
  // !TODO: figure out how to provide optional extens (maybe Variant?)
  if (not detour_nav_mesh_query)
  {
    return Vector3::INF;
  }
  float search_box_center[] = {target_point.x, target_point.y, target_point.z};
  // big extents yield large processing time
  // TODO: benchmark what values would be best
  float search_box_half_extents[] = {100, 100, 100}; // TODO: const
  dtQueryFilter filter; // TODO: store in object
  // filter.setIncludeFlags(POLYGON_SEARCHABLE);
  dtPolyRef closest_polygon{};
  float closest_point[3] = {0};
  auto outcome = detour_nav_mesh_query->ref().findNearestPoly(
      search_box_center, search_box_half_extents, &filter, &closest_polygon, closest_point);
  constexpr auto invalid_polygon = 0;
  if (DT_SUCCESS == outcome and closest_polygon == invalid_polygon)
  {
    return Vector3::INF;
  }
  else if (DT_SUCCESS == outcome)
  {
    return Vector3(closest_point[0], closest_point[1], closest_point[2]);
  }
  ERR_PRINT(detour_status_to_string(outcome).c_str()); // TODO: make human-readable
  return Vector3::INF;
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
  // !TODO: setup query
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

std::string DetourNavigationMesh::detour_status_to_string(dtStatus status)
{
  std::string outcome{"dtStatus"};
  outcome = status & DT_FAILURE ? outcome + "/DT_FAILURE" : outcome;
  outcome = status & DT_SUCCESS ? outcome + "/DT_SUCCESS" : outcome;
  outcome = status & DT_IN_PROGRESS ? outcome + "/DT_IN_PROGRESS" : outcome;
  outcome = status & DT_WRONG_MAGIC ? outcome + "/DT_WRONG_MAGIC" : outcome;
  outcome = status & DT_WRONG_VERSION ? outcome + "/DT_WRONG_VERSION" : outcome;
  outcome = status & DT_OUT_OF_MEMORY ? outcome + "/DT_OUT_OF_MEMORY" : outcome;
  outcome = status & DT_INVALID_PARAM ? outcome + "/DT_INVALID_PARAM" : outcome;
  outcome = status & DT_BUFFER_TOO_SMALL ? outcome + "/DT_BUFFER_TOO_SMALL" : outcome;
  outcome = status & DT_OUT_OF_NODES ? outcome + "/DT_OUT_OF_NODES" : outcome;
  outcome = status & DT_PARTIAL_RESULT ? outcome + "/DT_PARTIAL_RESULT" : outcome;
  outcome = status & DT_ALREADY_OCCUPIED ? outcome + "/DT_ALREADY_OCCUPIED" : outcome;
  return outcome;
}
