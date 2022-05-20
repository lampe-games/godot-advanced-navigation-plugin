#pragma once

#include <Godot.hpp>
#include <Mesh.hpp>
#include <Resource.hpp>

class InputGeometry : public godot::Resource
{
  GODOT_CLASS(InputGeometry, godot::Resource);

 public:
  static void _register_methods();

  void _init() {}

  void add_nodes(godot::Array nodes);
  void add_resources(godot::Array resources);

  godot::Array get_ccw_triangles();

 private:
  void copy_mesh_to_arrays_ccw(
      const godot::Mesh&,
      const godot::Transform& global_transform,
      godot::PoolVector3Array& triangle_vertices,
      godot::PoolIntArray& triangle_indices);

 private:
  godot::Array nodes_to_parse;
  godot::Array resources_to_parse;
};
