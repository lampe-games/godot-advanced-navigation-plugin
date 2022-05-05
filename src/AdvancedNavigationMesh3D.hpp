#pragma once

#include <Godot.hpp>
#include <Material.hpp>
#include <MeshInstance.hpp>
#include <Reference.hpp>
#include <Spatial.hpp>
#include <SpatialMaterial.hpp>

using namespace godot;

class AdvancedNavigationMesh3D : public Spatial
{
  GODOT_CLASS(AdvancedNavigationMesh3D, Spatial);

 public:
  void _init();
  void _ready();

  void bake();
  void clear();

  static void _register_methods()
  {
    register_method("_ready", &AdvancedNavigationMesh3D::_ready);

    register_method("bake", &AdvancedNavigationMesh3D::bake);
    register_method("clear", &AdvancedNavigationMesh3D::clear);

    register_property<AdvancedNavigationMesh3D, bool>(
        "baked",
        &AdvancedNavigationMesh3D::baked,
        false,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_STORAGE,
        GODOT_PROPERTY_HINT_NONE);
  }

 private:
  void create_debug_mesh_instance();
  void update_debug_mesh_instance(Ref<Mesh>);
  Ref<Mesh> create_debug_mesh();
  Ref<Mesh> load_debug_mesh();
  Ref<Material> create_debug_mesh_instance_material();

 private:
  MeshInstance* debug_mesh_instance{nullptr};
  bool baked{false};
};
