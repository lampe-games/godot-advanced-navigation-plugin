#include "DetourCrowd.hpp"

using namespace godot;

void DetourCrowd::_register_methods()
{
  register_method("initialize", &DetourCrowd::initialize);
}

bool DetourCrowd::initialize(godot::Ref<DetourCrowdConfig>, godot::Ref<DetourNavigationMesh>)
{
  return false;
}
