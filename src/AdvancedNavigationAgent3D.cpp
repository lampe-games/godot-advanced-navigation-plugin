#include "AdvancedNavigationAgent3D.hpp"

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
  godot::register_property<AdvancedNavigationAgent3D, bool>(
      "passive_movement", &AdvancedNavigationAgent3D::passive_movement, default_passive_movement);
  godot::register_property<AdvancedNavigationAgent3D, float>(
      "target_desired_distance",
      &AdvancedNavigationAgent3D::target_desired_distance,
      default_target_desired_distance);
  godot::register_property<AdvancedNavigationAgent3D, float>(
      "radius", &AdvancedNavigationAgent3D::radius, DetourCrowdAgentConfig::default_radius);
  godot::register_property<AdvancedNavigationAgent3D, float>(
      "height", &AdvancedNavigationAgent3D::height, DetourCrowdAgentConfig::default_height);
  godot::register_property<AdvancedNavigationAgent3D, float>(
      "max_acceleration",
      &AdvancedNavigationAgent3D::max_acceleration,
      DetourCrowdAgentConfig::default_max_acceleration);
  godot::register_property<AdvancedNavigationAgent3D, float>(
      "max_speed",
      &AdvancedNavigationAgent3D::max_speed,
      DetourCrowdAgentConfig::default_max_speed);
  godot::register_property<AdvancedNavigationAgent3D, float>(
      "collision_query_range",
      &AdvancedNavigationAgent3D::collision_query_range,
      DetourCrowdAgentConfig::default_collision_query_range);
  godot::register_property<AdvancedNavigationAgent3D, float>(
      "path_optimization_range",
      &AdvancedNavigationAgent3D::path_optimization_range,
      DetourCrowdAgentConfig::default_path_optimization_range);
  godot::register_property<AdvancedNavigationAgent3D, float>(
      "separation_weight",
      &AdvancedNavigationAgent3D::separation_weight,
      DetourCrowdAgentConfig::default_separation_weight);
  godot::register_property<AdvancedNavigationAgent3D, bool>(
      "flags/anticipate_turns",
      &AdvancedNavigationAgent3D::anticipate_turns,
      DetourCrowdAgentConfig::default_anticipate_turns);
  godot::register_property<AdvancedNavigationAgent3D, bool>(
      "flags/obstacle_avoidance",
      &AdvancedNavigationAgent3D::obstacle_avoidance,
      DetourCrowdAgentConfig::default_obstacle_avoidance);
  godot::register_property<AdvancedNavigationAgent3D, bool>(
      "flags/separation",
      &AdvancedNavigationAgent3D::separation,
      DetourCrowdAgentConfig::default_separation);
  godot::register_property<AdvancedNavigationAgent3D, bool>(
      "flags/optimize_path_visibility",
      &AdvancedNavigationAgent3D::optimize_path_visibility,
      DetourCrowdAgentConfig::default_optimize_path_visibility);
  godot::register_property<AdvancedNavigationAgent3D, bool>(
      "flags/optimize_path_topology",
      &AdvancedNavigationAgent3D::optimize_path_topology,
      DetourCrowdAgentConfig::default_optimize_path_topology);

  // signals
  register_signal<AdvancedNavigationAgent3D>(
      "new_position", "position", GODOT_VARIANT_TYPE_VECTOR3);
  register_signal<AdvancedNavigationAgent3D>(
      "new_velocity", "velocity", GODOT_VARIANT_TYPE_VECTOR3);
  register_signal<AdvancedNavigationAgent3D>("target_reached");
}

void AdvancedNavigationAgent3D::_ready()
{
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

void AdvancedNavigationAgent3D::set_position(godot::Vector3 a_position)
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

void AdvancedNavigationAgent3D::set_target(godot::Vector3 a_target)
{
  if (agent.is_valid())
  {
    agent->set_target(a_target);
  }
  else
  {
    requested_target = a_target;
  }
}

godot::Vector3 AdvancedNavigationAgent3D::get_position()
{
  if (agent.is_valid())
  {
    return agent->get_position();
  }
  return Vector3::INF;
}

godot::Vector3 AdvancedNavigationAgent3D::get_target()
{
  if (agent.is_valid())
  {
    return agent->get_target();
  }
  return Vector3::INF;
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
  agent->connect("new_position", this, "on_new_position");
  agent->connect("new_velocity", this, "on_new_velocity");
}

Ref<DetourCrowdAgentConfig> AdvancedNavigationAgent3D::create_detour_crowd_agent_config() const
{
  Ref<DetourCrowdAgentConfig> config(DetourCrowdAgentConfig::_new());
  config->radius = radius;
  config->height = height;
  config->max_acceleration = max_acceleration;
  config->max_speed = max_speed;
  config->collision_query_range = collision_query_range;
  config->path_optimization_range = path_optimization_range;
  config->separation_weight = separation_weight;
  config->anticipate_turns = anticipate_turns;
  config->obstacle_avoidance = obstacle_avoidance;
  config->separation = separation;
  config->optimize_path_visibility = optimize_path_visibility;
  config->optimize_path_topology = optimize_path_topology;
  return config;
}

void AdvancedNavigationAgent3D::on_navigation_crowd_changed()
{
  try_fetching_crowd();
  try_creating_agent();
}

void AdvancedNavigationAgent3D::on_new_position(godot::Vector3 position)
{
  emit_signal("new_position", position);
  if (position.distance_to(get_target()) <= target_desired_distance)
  {
    if (passive_movement)
    {
      set_target(position);
    }
    else
    {
      set_target(Vector3::INF);
    }
    emit_signal("target_reached");
  }
}

void AdvancedNavigationAgent3D::on_new_velocity(godot::Vector3 velocity)
{
  emit_signal("new_velocity", velocity);
}
