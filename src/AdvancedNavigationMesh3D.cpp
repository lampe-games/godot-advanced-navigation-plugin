#include "AdvancedNavigationMesh3D.hpp"

#include <Engine.hpp>
#include <MeshInstance.hpp>
#include <PlaneMesh.hpp>
#include <SceneTree.hpp>

void AdvancedNavigationMesh3D::_init()
{
  Godot::print("AdvancedNavigationMesh3D::_init()");
}

void AdvancedNavigationMesh3D::_ready()
{
  Godot::print("AdvancedNavigationMesh3D::_ready()");
  if (get_tree()->is_debugging_navigation_hint() || Engine::get_singleton()->is_editor_hint())
  {
    create_debug_mesh_instance();
  }
}

void AdvancedNavigationMesh3D::_process()
{
  Godot::print("AdvancedNavigationMesh3D::_process()");
  set_process(false);
}

void AdvancedNavigationMesh3D::bake()
{
  Godot::print("AdvancedNavigationMesh3D::bake()");
  if (get_tree()->is_debugging_navigation_hint() || Engine::get_singleton()->is_editor_hint())
  {
    update_debug_mesh_instance(create_debug_mesh());
  }
}

void AdvancedNavigationMesh3D::clear()
{
  Godot::print("AdvancedNavigationMesh3D::clear()");
  if (get_tree()->is_debugging_navigation_hint() || Engine::get_singleton()->is_editor_hint())
  {
    update_debug_mesh_instance(Ref<Mesh>());
  }
}

void AdvancedNavigationMesh3D::create_debug_mesh_instance()
{
  Godot::print("AdvancedNavigationMesh3D::create_debug_mesh_instance()");
  debug_mesh_instance = MeshInstance::_new();
  debug_mesh_instance->set_material_override(create_debug_mesh_instance_material());
  add_child(debug_mesh_instance);
}

void AdvancedNavigationMesh3D::update_debug_mesh_instance(Ref<Mesh> mesh)
{
  Godot::print("AdvancedNavigationMesh3D::update_debug_mesh_instance()");
  debug_mesh_instance->set_mesh(mesh);
}

Ref<Mesh> AdvancedNavigationMesh3D::create_debug_mesh()
{
  return PlaneMesh::_new();
}

Ref<Material> AdvancedNavigationMesh3D::create_debug_mesh_instance_material()
{
  Ref<SpatialMaterial> material = SpatialMaterial::_new();
  material->set_flag(SpatialMaterial::FLAG_UNSHADED, true);
  material->set_feature(SpatialMaterial::FEATURE_TRANSPARENT, true);
  material->set_flag(SpatialMaterial::FLAG_SRGB_VERTEX_COLOR, true);
  material->set_flag(SpatialMaterial::FLAG_ALBEDO_FROM_VERTEX_COLOR, true);
  material->set_albedo(Color(1.0, 0.0, 0.0, 0.4));
  return material;
}
