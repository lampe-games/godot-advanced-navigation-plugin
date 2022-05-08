#pragma once

#include <memory>

#include <Godot.hpp>
#include <Mesh.hpp>
#include <Resource.hpp>

#include "RecastWrappers.hpp"

// TODO: rename to RecastPolygonMeshBundle iff RecastPolyMesh/RecastPolyMeshDetail are introduced

class RecastPolygonMesh : public godot::Resource
{
  GODOT_CLASS(RecastPolygonMesh, godot::Resource);

 public:
  RecastPolygonMesh();
  ~RecastPolygonMesh();

  void _init();

  // TODO: pass parameters
  // TODO: build_from_triangles(arrays)
  // TODO: build_from_reordered_triangles(arrays) or CCW/CW instead of reordered
  bool build_from_triangles(godot::PoolVector3Array& vertices, godot::PoolIntArray& indices);
  bool build_from_raw_triangles(
      const float* vertices, // [(x, y, z) * vertices_num]
      const int vertices_num,
      const int* indices, // [(vertA, vertB, vertC) * triangles_num], must be reordered
      const int triangles_num);

  godot::Ref<godot::Mesh> get_poly_mesh() const;
  godot::Ref<godot::Mesh> get_poly_mesh_detail() const;

  static void _register_methods()
  {
    // register_method("build_from_plane_mesh", &RecastPolygonMesh::build_from_plane_mesh);
    // register_method("get_mesh", &RecastPolygonMesh::get_mesh);
  }

 private:
  std::unique_ptr<Recast::PolyMesh> simple_recast_mesh{nullptr};
  std::unique_ptr<Recast::PolyMeshDetail> detailed_recast_mesh{nullptr};
};
