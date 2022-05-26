#include "DetourCrowd.hpp"

#include "DetourCrowdAgent.hpp"

using namespace godot;

void DetourCrowd::_register_methods()
{
  register_method("initialize", &DetourCrowd::initialize);
  register_method("create_agent", &DetourCrowd::create_agent);
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
  if (detour_crowd)
  {
    ERR_PRINT("Already initialized");
    return false;
  }

  std::shared_ptr<detour::Crowd> a_detour_crowd = std::make_shared<detour::Crowd>();
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
    ERR_PRINT("Could not initialize dtCrowd");
    return false;
  }

  detour_crowd = std::move(a_detour_crowd);
  detour_nav_mesh = std::move(a_detour_nav_mesh);

  return true;
}

Ref<DetourCrowdAgent> DetourCrowd::create_agent(
    Vector3 position,
    Ref<DetourCrowdAgentConfig> config)
{
  auto agent = Ref<DetourCrowdAgent>(DetourCrowdAgent::_new());
  if (agent->initialize(position, config, this))
  {
    return agent;
  }
  return nullptr;
}

std::shared_ptr<detour::Crowd> DetourCrowd::get_detour_crowd()
{
  return detour_crowd;
}
