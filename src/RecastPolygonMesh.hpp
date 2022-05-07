#pragma once

#include <memory>

#include <Godot.hpp>
#include <Mesh.hpp>
#include <PlaneMesh.hpp>
#include <Resource.hpp>

#include "RecastWrappers.hpp"

class RecastPolygonMesh : public godot::Resource
{
  GODOT_CLASS(RecastPolygonMesh, godot::Resource);

 public:
  RecastPolygonMesh();
  ~RecastPolygonMesh();

  void _init();

  void build_from_plane_mesh(godot::Ref<godot::PlaneMesh>);
  godot::Ref<godot::Mesh> get_mesh() const;

  static void _register_methods()
  {
    register_method("build_from_plane_mesh", &RecastPolygonMesh::build_from_plane_mesh);
    register_method("get_mesh", &RecastPolygonMesh::get_mesh);
    ;
  }

 private:
  std::unique_ptr<Recast::PolyMesh> simple_recast_mesh{nullptr};
  std::unique_ptr<Recast::PolyMeshDetail> detailed_recast_mesh{nullptr};
};
