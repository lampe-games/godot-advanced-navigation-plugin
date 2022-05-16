#pragma once

#include <Godot.hpp>
#include <Material.hpp>
#include <MeshInstance.hpp>
#include <Reference.hpp>
#include <Spatial.hpp>
#include <SpatialMaterial.hpp>

#include "DetourNavigationMesh.hpp"
#include "RecastPolygonMesh.hpp"

using namespace godot;

class AdvancedNavigationMesh3D : public Spatial
{
  GODOT_CLASS(AdvancedNavigationMesh3D, Spatial);

 public:
  enum DebugMeshType
  {
    DEBUG_MESH_TYPE_RECAST_POLY = 0,
    DEBUG_MESH_TYPE_RECAST_POLY_DETAIL,
    DEBUG_MESH_TYPE_DETOUR_NAVMESH,
  };

  void _init();
  void _ready();

  void bake();
  void clear();

  static constexpr int default_debug_mesh_type{DEBUG_MESH_TYPE_DETOUR_NAVMESH};

  static void _register_methods()
  {
    register_method("_ready", &AdvancedNavigationMesh3D::_ready);

    register_method("bake", &AdvancedNavigationMesh3D::bake);
    register_method("clear", &AdvancedNavigationMesh3D::clear);

    // properties

    register_property<AdvancedNavigationMesh3D, Ref<RecastPolygonMeshConfig>>(
        "config",
        &AdvancedNavigationMesh3D::config,
        Ref<RecastPolygonMeshConfig>(),
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_RESOURCE_TYPE,
        "Resource");
    godot::register_property<AdvancedNavigationMesh3D, int>(
        "debug/mesh_type",
        &AdvancedNavigationMesh3D::debug_mesh_type,
        default_debug_mesh_type,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_ENUM,
        "Recast Poly,Recast Poly Detail,Detour Navmesh");

    // storage

    register_property<AdvancedNavigationMesh3D, Ref<DetourNavigationMesh>>(
        "navigation_mesh",
        &AdvancedNavigationMesh3D::navigation_mesh,
        Ref<RecastPolygonMeshConfig>(),
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_STORAGE,
        GODOT_PROPERTY_HINT_NONE);
  }

 private:
  void create_debug_mesh_instance();
  void update_debug_mesh_instance(Ref<Mesh>);
  Ref<Mesh> get_debug_mesh();
  Ref<Material> create_debug_mesh_instance_material();

 private:
  Ref<RecastPolygonMeshConfig> config{nullptr};
  int debug_mesh_type{default_debug_mesh_type};

 private:
  MeshInstance* debug_mesh_instance{nullptr};
  Ref<RecastPolygonMesh> polygon_mesh{nullptr};
  Ref<DetourNavigationMesh> navigation_mesh{nullptr};
};
