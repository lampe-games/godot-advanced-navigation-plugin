#pragma once

#include <Godot.hpp>
#include <Mesh.hpp>
#include <PlaneMesh.hpp>
#include <Resource.hpp>

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
};
