#include "AdvancedNavigationAgent3D.hpp"

#include <Engine.hpp>

using namespace godot;

void AdvancedNavigationAgent3D::_register_methods()
{
  // methods
  register_method("_ready", &AdvancedNavigationAgent3D::_ready);
  register_method("set_navigation_crowd", &AdvancedNavigationAgent3D::set_navigation_crowd);
  register_method("set_position", &AdvancedNavigationAgent3D::set_position);
  register_method("set_target", &AdvancedNavigationAgent3D::set_target);
  register_method("get_position", &AdvancedNavigationAgent3D::get_position);
  register_method("get_target", &AdvancedNavigationAgent3D::get_target);
  register_method("on_new_position", &AdvancedNavigationAgent3D::on_new_position);
  register_method("on_new_velocity", &AdvancedNavigationAgent3D::on_new_velocity);
  register_method(
      "on_navigation_crowd_changed", &AdvancedNavigationAgent3D::on_navigation_crowd_changed);

  // properties
  register_property<AdvancedNavigationAgent3D, Vector3>(
      "position",
      &AdvancedNavigationAgent3D::set_position,
      &AdvancedNavigationAgent3D::get_position,
      Vector3::INF,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_NOEDITOR);
  register_property<AdvancedNavigationAgent3D, Vector3>(
      "target",
      &AdvancedNavigationAgent3D::set_target,
      &AdvancedNavigationAgent3D::get_target,
      Vector3::INF,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_NOEDITOR);

  // TODO: make sure the agent resource is recreated each time we change properties
  // TODO: add hints
  register_property<AdvancedNavigationAgent3D, bool>(
      "passive_movement",
      &AdvancedNavigationAgent3D::set_passive_movement,
      &AdvancedNavigationAgent3D::get_passive_movement,
      default_passive_movement);
  register_property<AdvancedNavigationAgent3D, float>(
      "target_desired_distance",
      &AdvancedNavigationAgent3D::target_desired_distance,
      default_target_desired_distance);
  register_property<AdvancedNavigationAgent3D, int>(
      "backend",
      &AdvancedNavigationAgent3D::backend,
      default_backend,
      GODOT_METHOD_RPC_MODE_DISABLED,
      GODOT_PROPERTY_USAGE_DEFAULT,
      GODOT_PROPERTY_HINT_ENUM,
      "DetourCrowd");
  register_property<AdvancedNavigationAgent3D, Ref<Resource>>(
      "parameters", &AdvancedNavigationAgent3D::parameters, Ref<Resource>());

  // signals
  register_signal<AdvancedNavigationAgent3D>(
      "new_position", "position", GODOT_VARIANT_TYPE_VECTOR3);
  register_signal<AdvancedNavigationAgent3D>(
      "new_velocity", "velocity", GODOT_VARIANT_TYPE_VECTOR3);
  register_signal<AdvancedNavigationAgent3D>("target_reached");
}

void AdvancedNavigationAgent3D::_init()
{
  if (parameters.is_null())
  {
    parameters = Ref<DetourCrowdAgentConfig>(DetourCrowdAgentConfig::_new());
  }
}

void AdvancedNavigationAgent3D::_ready()
{
  if (Engine::get_singleton()->is_editor_hint())
  {
    return;
  }
  // TODO: navi auto-discovery (along parents)
}

void AdvancedNavigationAgent3D::set_navigation_crowd(
    AdvancedNavigationCrowd3D* a_navigation_crowd_node)
{
  if (navigation_crowd_node != nullptr)
  {
    navigation_crowd_node->disconnect("changed", this, "on_navigation_crowd_changed");
  }
  navigation_crowd_node = a_navigation_crowd_node;
  if (navigation_crowd_node != nullptr)
  {
    navigation_crowd_node->connect("changed", this, "on_navigation_crowd_changed");
    try_fetching_crowd();
    try_creating_agent();
  }
}

void AdvancedNavigationAgent3D::set_position(Vector3 a_position)
{
  if (a_position == Vector3::INF)
  {
    // TODO: remove agent
    ERR_PRINT("Not implemented");
    return;
  }
  if (agent.is_valid())
  {
    // TODO: if agent exists: recreate it
    ERR_PRINT("Not implemented");
    return;
  }
  requested_position = a_position;
  try_creating_agent();
}

void AdvancedNavigationAgent3D::set_target(Vector3 a_target)
{
  if (agent.is_valid())
  {
    if (passive_movement and a_target == Vector3::INF)
    {
      auto position = get_position();
      agent->set_target(position);
      last_reached_target = position;
    }
    else
    {
      agent->set_target(a_target);
      last_reached_target = Vector3::INF;
    }
  }
  else
  {
    requested_target = a_target;
  }
}

void AdvancedNavigationAgent3D::set_passive_movement(bool a_passive_movement)
{
  if (a_passive_movement == passive_movement)
  {
    return;
  }
  passive_movement = a_passive_movement;
  if (agent.is_valid())
  {
    if (passive_movement and get_target() == Vector3::INF)
    {
      set_target(get_position());
    }
    else if (not passive_movement and get_target() != Vector3::INF)
    {
      set_target(Vector3::INF);
    }
  }
}

Vector3 AdvancedNavigationAgent3D::get_position()
{
  if (agent.is_valid())
  {
    return agent->get_position();
  }
  return Vector3::INF;
}

Vector3 AdvancedNavigationAgent3D::get_target()
{
  if (agent.is_valid())
  {
    return agent->get_target();
  }
  return Vector3::INF;
}

bool AdvancedNavigationAgent3D::get_passive_movement()
{
  return passive_movement;
}

void AdvancedNavigationAgent3D::try_fetching_crowd()
{
  Ref<DetourCrowd> a_crowd = navigation_crowd_node->get_crowd();
  if (a_crowd.is_null() or a_crowd == crowd)
  {
    return;
  }
  crowd = a_crowd;
  if (agent.is_valid())
  {
    agent->disconnect("new_position", this, "on_new_position");
    agent->disconnect("new_velocity", this, "on_new_velocity");
  }
  agent = Ref<DetourCrowdAgent>();
}

void AdvancedNavigationAgent3D::try_creating_agent()
{
  if (agent.is_valid() or crowd.is_null() or requested_position == Vector3::INF)
  {
    return;
  }
  auto a_requested_position = requested_position;
  auto a_agent = crowd->create_agent(requested_position, create_detour_crowd_agent_config());
  requested_position = Vector3::INF;
  if (a_agent.is_null())
  {
    ERR_PRINT("Could not create 'DetourCrowdAgent'");
    return;
  }
  agent = a_agent;
  if (requested_target != Vector3::INF)
  {
    agent->set_target(requested_target);
    requested_target = Vector3::INF;
  }
  else if (passive_movement)
  {
    agent->set_target(a_requested_position);
  }
  agent->connect("new_position", this, "on_new_position");
  agent->connect("new_velocity", this, "on_new_velocity");
}

Ref<DetourCrowdAgentConfig> AdvancedNavigationAgent3D::create_detour_crowd_agent_config() const
{
  Ref<DetourCrowdAgentConfig> config(parameters);
  if (config.is_valid())
  {
    return config;
  }
  WARN_PRINT("'parameters' is not 'DetourCrowdAgentConfig', default used instead");
  return DetourCrowdAgentConfig::_new();
}

void AdvancedNavigationAgent3D::on_navigation_crowd_changed()
{
  try_fetching_crowd();
  try_creating_agent();
}

void AdvancedNavigationAgent3D::on_new_position(Vector3 position)
{
  emit_signal("new_position", position);
  auto target = get_target();
  if (position.distance_to(target) <= target_desired_distance)
  {
    if (not passive_movement)
    {
      set_target(Vector3::INF);
      emit_signal("target_reached");
    }
    else if (target != last_reached_target)
    {
      set_target(position);
      last_reached_target = get_target();
      emit_signal("target_reached");
    }
  }
}

void AdvancedNavigationAgent3D::on_new_velocity(Vector3 velocity)
{
  emit_signal("new_velocity", velocity);
}
