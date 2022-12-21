#include "DetourCrowdAgent.hpp"

using namespace godot;

#define RETURN_IF_UNINITIALIZED(x)                   \
  if (detour_crowd == nullptr)                       \
  {                                                  \
    ERR_PRINT("'DetourCrowdAgent' Not initialized"); \
    return x;                                        \
  }
#define RETURN_IF_DISABLED(x) \
  if (not is_enabled())       \
  {                           \
    return x;                 \
  }

void DetourCrowdAgent::_register_methods()
{
  // methods
  register_method("enable", &DetourCrowdAgent::enable);
  register_method("disable", &DetourCrowdAgent::disable);
  register_method("set_target", &DetourCrowdAgent::set_target);
  register_method("set_target_with_extents", &DetourCrowdAgent::set_target_with_extents);
  register_method("get_target", &DetourCrowdAgent::get_target);
  register_method("on_crowd_updated", &DetourCrowdAgent::on_crowd_updated);

  // properties
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

  // enums
  register_property<DetourCrowdAgent, int>(
      "STATE_UNINITIALIZED",
      nullptr,
      &DetourCrowdAgent::get_state_uninitialized,
      State::UNINITIALIZED);
  register_property<DetourCrowdAgent, int>(
      "STATE_DISABLED", nullptr, &DetourCrowdAgent::get_state_disabled, State::DISABLED);
  register_property<DetourCrowdAgent, int>(
      "STATE_INVALID", nullptr, &DetourCrowdAgent::get_state_invalid, State::INVALID);
  register_property<DetourCrowdAgent, int>(
      "STATE_WALKING", nullptr, &DetourCrowdAgent::get_state_walking, State::WALKING);
  register_property<DetourCrowdAgent, int>(
      "STATE_OFFMESH", nullptr, &DetourCrowdAgent::get_state_offmesh, State::OFFMESH);
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
  if (detour_crowd != nullptr)
  {
    ERR_PRINT("Already initialized");
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
  detour_crowd_agent.emplace(
      agent_id,
      *detour_crowd->ref().getAgent(agent_id),
      *detour_crowd->ref().getEditableAgent(agent_id));

  return true;
}

bool DetourCrowdAgent::enable()
{
  RETURN_IF_UNINITIALIZED(false);
  if (is_enabled())
  {
    return false;
  }

  const float* position_raw = &pending_position->coord[0];
  auto agent_id = detour_crowd->ref().addAgent(position_raw, &*pending_params);
  if (agent_id < 0)
  {
    WARN_PRINT("Cannot enable agent");
    return false;
  }
  detour_crowd_agent.emplace(
      agent_id,
      *detour_crowd->ref().getAgent(agent_id),
      *detour_crowd->ref().getEditableAgent(agent_id));
  pending_position.reset();
  pending_params.reset();

  if (pending_target.has_value())
  {
    detour_crowd->ref().requestMoveTarget(
        detour_crowd_agent->id, *pending_target_polygon, &pending_target->coord[0]);
  }

  return true;
}

void DetourCrowdAgent::disable()
{
  RETURN_IF_UNINITIALIZED();
  RETURN_IF_DISABLED();
  pending_position.emplace(get_position());
  pending_params.emplace(detour_crowd_agent->cref.params);
  auto current_target = get_target();
  if (current_target != Vector3::INF)
  {
    pending_target.emplace(current_target);
    pending_target_polygon.emplace(detour_crowd_agent->cref.targetRef);
  }
  detour_crowd->ref().removeAgent(detour_crowd_agent->id);
  detour_crowd_agent.reset();
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
    WARN_PRINT("Cannot set position to Vector3::INF");
    return false;
  }
  if (not is_enabled())
  {
    Vector3 aligned_position;
    std::tie(aligned_position, std::ignore) =
        detour_navigation_mesh_ref->get_closest_point_and_poly_with_extents_quiet(
            position,
            Vector3(
                detour_navigation_mesh_ref->DEFAULT_SERACH_BOX_EXTENTS,
                detour_navigation_mesh_ref->DEFAULT_SERACH_BOX_EXTENTS,
                detour_navigation_mesh_ref->DEFAULT_SERACH_BOX_EXTENTS));
    aligned_position = aligned_position == Vector3::INF ? position : aligned_position;
    pending_position.emplace(aligned_position);
    return true;
  }
  disable();
  set_position_with_extents(position, search_box_half_extents);
  if (not enable())
  {
    ERR_PRINT("Could not re-enable agent after changing its position");
    return false;
  }
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
  RETURN_IF_DISABLED(false);
  if (target == Vector3::INF)
  {
    return detour_crowd->ref().resetMoveTarget(detour_crowd_agent->id);
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
      detour_crowd_agent->id, polygon, &aligned_target.coord[0]);
}

Vector3 DetourCrowdAgent::get_target() const
{
  RETURN_IF_UNINITIALIZED(Vector3::INF);
  RETURN_IF_DISABLED(Vector3::INF);
  if (detour_crowd_agent->cref.targetState == DT_CROWDAGENT_TARGET_NONE)
  {
    return Vector3::INF;
  }
  return Vector3(
      detour_crowd_agent->cref.targetPos[0],
      detour_crowd_agent->cref.targetPos[1],
      detour_crowd_agent->cref.targetPos[2]);
}

Vector3 DetourCrowdAgent::get_position() const
{
  RETURN_IF_UNINITIALIZED(Vector3::INF);
  RETURN_IF_DISABLED(Vector3::INF);
  const float* position_raw = detour_crowd_agent->cref.npos;
  return Vector3(position_raw[0], position_raw[1], position_raw[2]);
}

Vector3 DetourCrowdAgent::get_velocity() const
{
  RETURN_IF_UNINITIALIZED(Vector3::INF);
  RETURN_IF_DISABLED(Vector3::INF);
  const float* velocity_raw = detour_crowd_agent->cref.vel;
  return Vector3(velocity_raw[0], velocity_raw[1], velocity_raw[2]);
}

int DetourCrowdAgent::get_state() const
{
  RETURN_IF_UNINITIALIZED(State::UNINITIALIZED);
  RETURN_IF_DISABLED(State::DISABLED);
  switch (detour_crowd_agent->cref.state)
  {
    case DT_CROWDAGENT_STATE_INVALID:
      return State::INVALID;
    case DT_CROWDAGENT_STATE_WALKING:
      return State::WALKING;
    case DT_CROWDAGENT_STATE_OFFMESH:
      return State::OFFMESH;
    default:
      ERR_PRINT("Unknown detour state");
      return -1;
  }
}

void DetourCrowdAgent::on_crowd_updated()
{
  RETURN_IF_DISABLED();
  if (detour_crowd_agent->cref.targetState == DT_CROWDAGENT_TARGET_VALID)
  {
    emit_signal("new_position", get_position());
    emit_signal("new_velocity", get_velocity());
  }
}
