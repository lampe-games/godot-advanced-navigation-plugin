#include "DetourCrowd.hpp"

using namespace godot;

void DetourCrowd::_register_methods()
{
  register_method("initialize", &DetourCrowd::initialize);
}

bool DetourCrowd::initialize(
    godot::Ref<DetourCrowdConfig> config,
    godot::Ref<DetourNavigationMesh> detour_navigation_mesh)
{
  if (config.is_null())
  {
    ERR_PRINT("Provided DetourCrowdConfig is null");
    return false;
  }
  if (detour_navigation_mesh.is_null())
  {
    ERR_PRINT("Provided DetourNavigationMesh is null");
    return false;
  }

  std::unique_ptr<detour::Crowd> a_detour_crowd = std::make_unique<detour::Crowd>();
  if (a_detour_crowd->ptr() == nullptr)
  {
    ERR_PRINT("Cannot allocate basic detour crowd structures");
    return false;
  }
  auto a_detour_nav_mesh = detour_navigation_mesh->get_detour_nav_mesh();
  if (a_detour_nav_mesh == nullptr)
  {
    ERR_PRINT("Provided DetourNavigationMesh is not built");
    return false;
  }

  if (not a_detour_crowd->ref().init(
          config->max_agents, config->max_agent_radius, a_detour_nav_mesh->ptr()))
  {
    ERR_PRINT("Could not initialize drCrowd");
    return false;
  }

  detour_crowd = std::move(a_detour_crowd);
  detour_nav_mesh = std::move(a_detour_nav_mesh);

  return true;
}
