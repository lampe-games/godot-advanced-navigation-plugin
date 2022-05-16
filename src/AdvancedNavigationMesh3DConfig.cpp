#include "AdvancedNavigationMesh3DConfig.hpp"

using namespace godot;

void AdvancedNavigationMesh3DConfig::_init()
{
  if (recast_config.is_null())
  {
    recast_config = Ref<RecastPolygonMeshConfig>(RecastPolygonMeshConfig::_new());
  }
}
