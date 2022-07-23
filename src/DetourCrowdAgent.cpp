#include "DetourCrowdAgent.hpp"

using namespace godot;

#define RETURN_IF_UNINITIALIZED(x)                   \
  if (detour_crowd == nullptr)                       \
  {                                                  \
    ERR_PRINT("'DetourCrowdAgent' Not initialized"); \
    return x;                                        \
  }

void DetourCrowdAgent::_register_methods()
{
  // methods
  register_property<DetourCrowdAgent, int>(
      "STATE_INVALID", nullptr, &DetourCrowdAgent::get_state_invalid, State::INVALID);
  register_property<DetourCrowdAgent, int>(
      "STATE_WALKING", nullptr, &DetourCrowdAgent::get_state_walking, State::WALKING);
  register_property<DetourCrowdAgent, int>(
      "STATE_OFFMESH", nullptr, &DetourCrowdAgent::get_state_offmesh, State::OFFMESH);

  register_method("set_target", &DetourCrowdAgent::set_target);
  register_method("set_target_with_extents", &DetourCrowdAgent::set_target_with_extents);
  register_method("get_target", &DetourCrowdAgent::get_target);
  register_method("on_crowd_updated", &DetourCrowdAgent::on_crowd_updated);

  register_property<DetourCrowdAgent, Vector3>(
      "position",
      &DetourCrowdAgent::void_set_position,
      &DetourCrowdAgent::get_position,
      Vector3::INF);
  register_property<DetourCrowdAgent, Vector3>(
      "velocity", nullptr, &DetourCrowdAgent::get_velocity, Vector3::INF);
  register_property<DetourCrowdAgent, int>("state", nullptr, &DetourCrowdAgent::get_state, -1);

  // signals
  register_signal<DetourCrowdAgent>("new_position", "position", GODOT_VARIANT_TYPE_VECTOR3);
  register_signal<DetourCrowdAgent>("new_velocity", "velocity", GODOT_VARIANT_TYPE_VECTOR3);
}

bool DetourCrowdAgent::initialize(
    Vector3 position,
    Ref<DetourCrowdAgentConfig> config,
    Ref<DetourCrowd> detour_crowd_ref)
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

  Vector3 aligned_position;
  auto a_detour_navigation_mesh_ref = detour_crowd_ref->get_detour_navigation_mesh_ref();
  std::tie(aligned_position, std::ignore) =
      a_detour_navigation_mesh_ref->get_closest_point_and_poly_with_extents_quiet(
          position,
          Vector3(
              a_detour_navigation_mesh_ref->DEFAULT_SERACH_BOX_EXTENTS,
              a_detour_navigation_mesh_ref->DEFAULT_SERACH_BOX_EXTENTS,
              a_detour_navigation_mesh_ref->DEFAULT_SERACH_BOX_EXTENTS));
  aligned_position = aligned_position == Vector3::INF ? position : aligned_position;
  const float* position_raw = &aligned_position.coord[0];
  dtCrowdAgentParams agent_params{};
  agent_params.radius = config->radius;
  agent_params.height = config->height;
  agent_params.maxAcceleration = config->max_acceleration;
  agent_params.maxSpeed = config->max_speed;
  agent_params.collisionQueryRange = config->collision_query_range;
  agent_params.pathOptimizationRange = config->path_optimization_range;
  agent_params.separationWeight = config->separation_weight;
  agent_params.updateFlags |= config->anticipate_turns ? DT_CROWD_ANTICIPATE_TURNS : 0;
  agent_params.updateFlags |= config->obstacle_avoidance ? DT_CROWD_OBSTACLE_AVOIDANCE : 0;
  agent_params.updateFlags |= config->separation ? DT_CROWD_SEPARATION : 0;
  agent_params.updateFlags |= config->optimize_path_visibility ? DT_CROWD_OPTIMIZE_VIS : 0;
  agent_params.updateFlags |= config->optimize_path_topology ? DT_CROWD_OPTIMIZE_TOPO : 0;
  agent_params.obstacleAvoidanceType = 0; // default
  agent_params.queryFilterType = 0; // default
  auto agent_id = a_detour_crowd->ref().addAgent(position_raw, &agent_params);
  if (agent_id < 0)
  {
    ERR_PRINT("addAgent() failed");
    return false;
  }

  detour_crowd_ref->connect("updated", this, "on_crowd_updated");
  detour_navigation_mesh_ref = detour_crowd_ref->get_detour_navigation_mesh_ref();
  detour_crowd = std::move(a_detour_crowd);
  const_detour_crowd_agent = detour_crowd->ref().getAgent(agent_id);
  detour_crowd_agent = detour_crowd->ref().getEditableAgent(agent_id);
  detour_crowd_agent_id = agent_id;

  return true;
}

void DetourCrowdAgent::void_set_position(Vector3 position)
{
  set_position(position);
}

bool DetourCrowdAgent::set_position(Vector3 position)
{
  return set_position_with_extents(
      position,
      Vector3(
          detour_navigation_mesh_ref->DEFAULT_SERACH_BOX_EXTENTS,
          detour_navigation_mesh_ref->DEFAULT_SERACH_BOX_EXTENTS,
          detour_navigation_mesh_ref->DEFAULT_SERACH_BOX_EXTENTS));
}

bool DetourCrowdAgent::set_position_with_extents(Vector3 position, Vector3 search_box_half_extents)
{
  RETURN_IF_UNINITIALIZED(false);
  if (position == Vector3::INF)
  {
    // TODO: remove agent from crowd
    ERR_PRINT("Not implemented");
    return false;
  }
  dtCrowdAgentParams agent_params = const_detour_crowd_agent->params;
  detour_crowd->ref().removeAgent(detour_crowd_agent_id);
  Vector3 aligned_position;
  std::tie(aligned_position, std::ignore) =
      detour_navigation_mesh_ref->get_closest_point_and_poly_with_extents_quiet(
          position,
          Vector3(
              detour_navigation_mesh_ref->DEFAULT_SERACH_BOX_EXTENTS,
              detour_navigation_mesh_ref->DEFAULT_SERACH_BOX_EXTENTS,
              detour_navigation_mesh_ref->DEFAULT_SERACH_BOX_EXTENTS));
  aligned_position = aligned_position == Vector3::INF ? position : aligned_position;
  const float* position_raw = &aligned_position.coord[0];
  auto agent_id = detour_crowd->ref().addAgent(position_raw, &agent_params);
  if (agent_id < 0)
  {
    // TODO: invalidate whole agent
    ERR_PRINT("Not implemented");
    return false;
  }
  const_detour_crowd_agent = detour_crowd->ref().getAgent(agent_id);
  detour_crowd_agent = detour_crowd->ref().getEditableAgent(agent_id);
  detour_crowd_agent_id = agent_id;
  return true;
}

bool DetourCrowdAgent::set_target(Vector3 target)
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
  RETURN_IF_UNINITIALIZED(false);
  if (target == Vector3::INF)
  {
    return detour_crowd->ref().resetMoveTarget(detour_crowd_agent_id);
  }
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
  RETURN_IF_UNINITIALIZED(Vector3::INF);
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
  RETURN_IF_UNINITIALIZED(Vector3::INF);
  const float* position_raw = const_detour_crowd_agent->npos;
  return Vector3(position_raw[0], position_raw[1], position_raw[2]);
}

Vector3 DetourCrowdAgent::get_velocity() const
{
  RETURN_IF_UNINITIALIZED(Vector3::INF);
  const float* velocity_raw = const_detour_crowd_agent->vel;
  return Vector3(velocity_raw[0], velocity_raw[1], velocity_raw[2]);
}

int DetourCrowdAgent::get_state() const
{
  RETURN_IF_UNINITIALIZED(-1);
  return static_cast<int>(const_detour_crowd_agent->state);
}

void DetourCrowdAgent::on_crowd_updated()
{
  if (detour_crowd_agent != nullptr &&
      const_detour_crowd_agent->targetState == DT_CROWDAGENT_TARGET_VALID)
  {
    emit_signal("new_position", get_position());
    emit_signal("new_velocity", get_velocity());
  }
}
