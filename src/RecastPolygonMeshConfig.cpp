#include "RecastPolygonMeshConfig.hpp"

using namespace godot;

void RecastPolygonMeshConfig::_init()
{
  bmin = Vector3(-10, -10, -10);
  bmax = Vector3(10, 10, 10);
}
