#include "AdvancedNavigationMesh3D.hpp"

#include <string>

#include <Engine.hpp>
#include <MeshInstance.hpp>
#include <PlaneMesh.hpp>
#include <SceneTree.hpp>
#include <Viewport.hpp>

#include "AdvancedNavigationServer3D.hpp"

void AdvancedNavigationMesh3D::_init()
{
  Godot::print("AdvancedNavigationMesh3D::_init()");
}

void AdvancedNavigationMesh3D::_ready()
{
  Godot::print("AdvancedNavigationMesh3D::_ready()");
  Godot::print(String("AdvancedNavigationMesh3D::baked ") + String(std::to_string(baked).c_str()));
  if (get_tree()->is_debugging_navigation_hint() or Engine::get_singleton()->is_editor_hint())
  {
    create_debug_mesh_instance();
    update_debug_mesh_instance(load_debug_mesh());
  }
}

void AdvancedNavigationMesh3D::bake()
{
  Godot::print("AdvancedNavigationMesh3D::bake()");
  if (get_tree()->is_debugging_navigation_hint() or Engine::get_singleton()->is_editor_hint())
  {
    update_debug_mesh_instance(create_debug_mesh());
    baked = true;
  }
}

void AdvancedNavigationMesh3D::clear()
{
  Godot::print("AdvancedNavigationMesh3D::clear()");
  if (get_tree()->is_debugging_navigation_hint() or Engine::get_singleton()->is_editor_hint())
  {
    update_debug_mesh_instance(Ref<Mesh>());
    baked = false;
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
  auto* server =
      get_tree()->get_root()->get_node<AdvancedNavigationServer3D>("AdvancedNavigationServer3D");
  if (server)
  {
    // return server->build_navigation_mesh();
    return server->build_polygon_mesh()->get_poly_mesh();
  }
  return nullptr;
}

Ref<Mesh> AdvancedNavigationMesh3D::load_debug_mesh()
{
  if (not baked)
  {
    return nullptr;
  }
  auto* server =
      get_tree()->get_root()->get_node<AdvancedNavigationServer3D>("AdvancedNavigationServer3D");
  if (server)
  {
    // return server->build_navigation_mesh();
    return server->build_polygon_mesh()->get_poly_mesh();
  }
  return nullptr;
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
