#pragma once

#include <Godot.hpp>
#include <Resource.hpp>

#include "RecastPolygonMeshConfig.hpp"

class AdvancedNavigationMesh3DConfig : public godot::Resource
{
  GODOT_CLASS(AdvancedNavigationMesh3DConfig, godot::Resource);

 public:
  void _init();

  static void _register_methods()
  {
    godot::register_property<AdvancedNavigationMesh3DConfig, godot::Ref<RecastPolygonMeshConfig>>(
        "recast_config",
        &AdvancedNavigationMesh3DConfig::recast_config,
        godot::Ref<RecastPolygonMeshConfig>(),
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_DEFAULT,
        GODOT_PROPERTY_HINT_RESOURCE_TYPE,
        "Resource");
  }

 public:
  godot::Ref<RecastPolygonMeshConfig> recast_config{nullptr};
};
