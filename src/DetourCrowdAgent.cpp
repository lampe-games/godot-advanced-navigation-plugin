#include "DetourCrowdAgent.hpp"

using namespace godot;

void DetourCrowdAgent::_register_methods()
{
  godot::register_property<DetourCrowdAgent, Vector3>(
      "position", nullptr, &DetourCrowdAgent::get_position, Vector3::INF);
  godot::register_property<DetourCrowdAgent, int>(
      "state", nullptr, &DetourCrowdAgent::get_state, -1);
}

bool DetourCrowdAgent::initialize(
    Vector3 position,
    godot::Ref<DetourCrowdAgentConfig> config,
    godot::Ref<DetourCrowd> detour_crowd)
{
  if (detour_crowd.is_null())
  {
    ERR_PRINT("'DetourCrowd' is null");
    return false;
  }
  if (config.is_null())
  {
    ERR_PRINT("'DetourCrowdAgentConfig' is null");
    return false;
  }
  auto a_raw_detour_crowd = detour_crowd->get_detour_crowd();
  if (not a_raw_detour_crowd)
  {
    ERR_PRINT("'DetourCrowd' not initialized");
    return false;
  }

  const float* position_raw = &position.coord[0];
  dtCrowdAgentParams agent_params{}; // TODO: fill
  auto agent_id = a_raw_detour_crowd->ref().addAgent(position_raw, &agent_params);
  if (agent_id < 0)
  {
    ERR_PRINT("addAgent() failed");
    return false;
  }

  raw_detour_crowd = std::move(a_raw_detour_crowd);
  const_detour_crowd_agent = raw_detour_crowd->ref().getAgent(agent_id);
  detour_crowd_agent = raw_detour_crowd->ref().getEditableAgent(agent_id);

  return true;
}

Vector3 DetourCrowdAgent::get_position() const
{
  if (const_detour_crowd_agent == nullptr)
  {
    ERR_PRINT("not initialized");
    return Vector3::INF;
  }
  const float* position_raw = const_detour_crowd_agent->npos;
  return Vector3(position_raw[0], position_raw[1], position_raw[2]);
}

int DetourCrowdAgent::get_state() const
{
  if (const_detour_crowd_agent == nullptr)
  {
    ERR_PRINT("not initialized");
    return -1;
  }
  return static_cast<int>(const_detour_crowd_agent->state);
}
