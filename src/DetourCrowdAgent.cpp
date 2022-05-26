#include "DetourCrowdAgent.hpp"

using namespace godot;

void DetourCrowdAgent::_register_methods()
{
  register_property<DetourCrowdAgent, int>(
      "STATE_INVALID", nullptr, &DetourCrowdAgent::get_state_invalid, State::INVALID);
  register_property<DetourCrowdAgent, int>(
      "STATE_WALKING", nullptr, &DetourCrowdAgent::get_state_walking, State::WALKING);
  register_property<DetourCrowdAgent, int>(
      "STATE_OFFMESH", nullptr, &DetourCrowdAgent::get_state_offmesh, State::OFFMESH);

  register_method("set_target", &DetourCrowdAgent::set_target);
  register_method("set_target_with_extents", &DetourCrowdAgent::set_target_with_extents);

  register_property<DetourCrowdAgent, Vector3>(
      "position", nullptr, &DetourCrowdAgent::get_position, Vector3::INF);
  register_property<DetourCrowdAgent, int>("state", nullptr, &DetourCrowdAgent::get_state, -1);
}

bool DetourCrowdAgent::initialize(
    Vector3 position,
    godot::Ref<DetourCrowdAgentConfig> config,
    godot::Ref<DetourCrowd> detour_crowd_ref)
{
  if (detour_crowd_ref.is_null())
  {
    ERR_PRINT("'DetourCrowd' is null");
    return false;
  }
  if (config.is_null())
  {
    ERR_PRINT("'DetourCrowdAgentConfig' is null");
    return false;
  }
  auto a_detour_crowd = detour_crowd_ref->get_detour_crowd();
  if (not a_detour_crowd)
  {
    ERR_PRINT("'DetourCrowd' not initialized");
    return false;
  }

  const float* position_raw = &position.coord[0];
  dtCrowdAgentParams agent_params{}; // TODO: fill
  auto agent_id = a_detour_crowd->ref().addAgent(position_raw, &agent_params);
  if (agent_id < 0)
  {
    ERR_PRINT("addAgent() failed");
    return false;
  }

  detour_crowd = std::move(a_detour_crowd);
  const_detour_crowd_agent = detour_crowd->ref().getAgent(agent_id);
  detour_crowd_agent = detour_crowd->ref().getEditableAgent(agent_id);

  return true;
}

bool DetourCrowdAgent::set_target(godot::Vector3 target)
{
  // return set_target_with_extents(
  //     target,
  //     Vector3(DEFAULT_SERACH_BOX_EXTENTS, DEFAULT_SERACH_BOX_EXTENTS, DEFAULT_SERACH_BOX_EXTENTS));
  return false;
}

bool DetourCrowdAgent::set_target_with_extents(
    godot::Vector3 target,
    godot::Vector3 search_box_half_extents)
{
  // dtQueryFilter filter;
  // filter.setIncludeFlags(POLYGON_SEARCHABLE);
  return false;
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
