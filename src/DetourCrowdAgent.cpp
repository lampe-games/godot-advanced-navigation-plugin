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
  register_method("get_target", &DetourCrowdAgent::get_target);

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

  detour_navigation_mesh_ref = detour_crowd_ref->get_detour_navigation_mesh_ref();
  detour_crowd = std::move(a_detour_crowd);
  const_detour_crowd_agent = detour_crowd->ref().getAgent(agent_id);
  detour_crowd_agent = detour_crowd->ref().getEditableAgent(agent_id);
  detour_crowd_agent_id = agent_id;

  return true;
}

bool DetourCrowdAgent::set_target(godot::Vector3 target)
{
  return set_target_with_extents(
      target,
      Vector3(
          detour_navigation_mesh_ref->DEFAULT_SERACH_BOX_EXTENTS,
          detour_navigation_mesh_ref->DEFAULT_SERACH_BOX_EXTENTS,
          detour_navigation_mesh_ref->DEFAULT_SERACH_BOX_EXTENTS));
}

bool DetourCrowdAgent::set_target_with_extents(Vector3 target, Vector3 search_box_half_extents)
{
  // TODO: check detour_crowd validity
  Vector3 aligned_target;
  dtPolyRef polygon;
  std::tie(aligned_target, polygon) =
      detour_navigation_mesh_ref->get_closest_point_and_poly_with_extents_quiet(
          target, search_box_half_extents);
  if (aligned_target == Vector3::INF)
  {
    return false;
  }
  return detour_crowd->ref().requestMoveTarget(
      detour_crowd_agent_id, polygon, &aligned_target.coord[0]);
}

Vector3 DetourCrowdAgent::get_target() const
{
  // TODO: check validity
  if (const_detour_crowd_agent->targetState == DT_CROWDAGENT_TARGET_NONE)
  {
    return Vector3::INF;
  }
  return Vector3(
      const_detour_crowd_agent->targetPos[0],
      const_detour_crowd_agent->targetPos[1],
      const_detour_crowd_agent->targetPos[2]);
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
