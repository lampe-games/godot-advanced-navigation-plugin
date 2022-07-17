#include "DetourCrowd.hpp"

#include "DetourCrowdAgent.hpp"

using namespace godot;

void DetourCrowd::_register_methods()
{
  // methods
  register_method("initialize", &DetourCrowd::initialize);
  register_method("create_agent", &DetourCrowd::create_agent);
  register_method("update", &DetourCrowd::update);

  // signals
  register_signal<DetourCrowd>("updated");
}

bool DetourCrowd::initialize(
    godot::Ref<DetourCrowdConfig> config,
    godot::Ref<DetourNavigationMesh> a_detour_navigation_mesh_ref)
{
  if (config.is_null())
  {
    ERR_PRINT("Provided DetourCrowdConfig is null");
    return false;
  }
  if (a_detour_navigation_mesh_ref.is_null())
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
  auto a_detour_nav_mesh = a_detour_navigation_mesh_ref->get_detour_nav_mesh();
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

  detour_navigation_mesh_ref = a_detour_navigation_mesh_ref;
  detour_crowd = std::move(a_detour_crowd);
  detour_nav_mesh = std::move(a_detour_nav_mesh);
  max_agent_radius = config->max_agent_radius;

  return true;
}

Ref<DetourCrowdAgent> DetourCrowd::create_agent(
    Vector3 position,
    Ref<DetourCrowdAgentConfig> config)
{
  // TODO: return if uninitialized
  if (is_full())
  {
    ERR_PRINT("Cannot create new agent - limit of 25 agents reached");
    return nullptr;
  }
  if (config.is_null())
  {
    ERR_PRINT("Cannot create new agent - config is null");
    return nullptr;
  }
  if (config->radius > max_agent_radius)
  {
    ERR_PRINT(String("Cannot create new agent - agent's radius ({0}) exceeds crowd's limit ({1})")
                  .format(Array::make(config->radius, max_agent_radius)));
    return nullptr;
  }
  auto agent = Ref<DetourCrowdAgent>(DetourCrowdAgent::_new());
  if (agent->initialize(position, config, this))
  {
    return agent;
  }
  return nullptr;
}

void DetourCrowd::update(float seconds)
{
  // TODO: return if uninitialized
  detour_crowd->ref().update(seconds, nullptr);
  emit_signal("updated");
}

bool DetourCrowd::is_full()
{
  // TODO: return if uninitialized
  auto agents_limit = detour_crowd->ref().getAgentCount();
  dtCrowdAgent* active_agents_array[agents_limit];
  auto active_agents = detour_crowd->ref().getActiveAgents(active_agents_array, agents_limit);
  return active_agents == agents_limit;
}

Ref<DetourNavigationMesh> DetourCrowd::get_detour_navigation_mesh_ref()
{
  return detour_navigation_mesh_ref;
}

std::shared_ptr<detour::Crowd> DetourCrowd::get_detour_crowd()
{
  return detour_crowd;
}
