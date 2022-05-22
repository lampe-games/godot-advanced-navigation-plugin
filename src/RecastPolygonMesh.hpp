#pragma once

#include <memory>

#include <Godot.hpp>
#include <Mesh.hpp>
#include <Resource.hpp>

#include "InputGeometry.hpp"
#include "RecastPolygonMeshConfig.hpp"
#include "RecastWrappers.hpp"

// TODO: rename to RecastPolygonMeshBundle iff RecastPolyMesh/RecastPolyMeshDetail are introduced

class RecastPolygonMesh : public godot::Resource
{
  GODOT_CLASS(RecastPolygonMesh, godot::Resource);

 public:
  void _init() {}

  bool build_from_input_geometry(godot::Ref<InputGeometry>, godot::Ref<RecastPolygonMeshConfig>);
  // TODO: build_from_cw_triangles(arrays)
  // TODO: build_from_ccw_triangles(arrays)
  bool build_from_raw_triangles(
      const float* vertices, // [(x, y, z) * vertices_num]
      const int vertices_num,
      const int* indices, // [(vertA, vertB, vertC) * triangles_num], must be CCW (Godot uses CW)
      const int triangles_num,
      godot::Ref<RecastPolygonMeshConfig>);

  bool is_baked() { return simple_recast_mesh and detailed_recast_mesh; }

  godot::Ref<godot::Mesh> get_height_field_mesh() const;
  godot::Ref<godot::Mesh> get_poly_mesh() const;
  godot::Ref<godot::Mesh> get_poly_mesh_detail() const;

  static void _register_methods()
  {
    // register_method("build_from_plane_mesh", &RecastPolygonMesh::build_from_plane_mesh);
    // register_method("get_mesh", &RecastPolygonMesh::get_mesh);
  }

 public:
  const rcPolyMesh& get_recast_poly_mesh() { return simple_recast_mesh->ref(); }
  const rcPolyMeshDetail& get_recast_poly_mesh_detail() { return detailed_recast_mesh->ref(); }

 private:
  std::unique_ptr<recast::Heightfield> height_field_solid{nullptr};
  std::unique_ptr<recast::PolyMesh> simple_recast_mesh{nullptr};
  std::unique_ptr<recast::PolyMeshDetail> detailed_recast_mesh{nullptr};
};
