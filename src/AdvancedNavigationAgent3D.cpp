#include "AdvancedNavigationAgent3D.hpp"

#include <SceneTree.hpp>
#include <Viewport.hpp>

#include "AdvancedNavigationServer3D.hpp"

using namespace godot;

void AdvancedNavigationAgent3D::_register_methods()
{
  // methods
  register_method("_ready", &AdvancedNavigationAgent3D::_ready);
  register_method("set_navigation_mesh", &AdvancedNavigationAgent3D::set_navigation_mesh);
  register_method("set_position", &AdvancedNavigationAgent3D::set_position);
  register_method("set_target", &AdvancedNavigationAgent3D::set_target);
  register_method("get_position", &AdvancedNavigationAgent3D::get_position);
  register_method("get_target", &AdvancedNavigationAgent3D::get_target);
  register_method("on_new_position", &AdvancedNavigationAgent3D::on_new_position);
  register_method("on_new_velocity", &AdvancedNavigationAgent3D::on_new_velocity);

  // properties
  register_property<AdvancedNavigationAgent3D, Vector3>(
      "position",
      &AdvancedNavigationAgent3D::set_position,
      &AdvancedNavigationAgent3D::get_position,
      Vector3::INF);
  register_property<AdvancedNavigationAgent3D, Vector3>(
      "target",
      &AdvancedNavigationAgent3D::set_target,
      &AdvancedNavigationAgent3D::get_target,
      Vector3::INF);

  // signals
  register_signal<AdvancedNavigationAgent3D>(
      "new_position", "position", GODOT_VARIANT_TYPE_VECTOR3);
  register_signal<AdvancedNavigationAgent3D>(
      "new_velocity", "velocity", GODOT_VARIANT_TYPE_VECTOR3);
}

void AdvancedNavigationAgent3D::_init()
{
  Godot::print("AdvancedNavigationAgent3D::_init()");
}

void AdvancedNavigationAgent3D::_ready()
{
  // TODO: navi auto-discovery (along parents)
  Godot::print("AdvancedNavigationAgent3D::_ready()");
}

void AdvancedNavigationAgent3D::set_navigation_mesh(
    AdvancedNavigationMesh3D* a_navigation_mesh_node)
{
  Godot::print("AdvancedNavigationAgent3D::set_navigation_mesh() {0}", a_navigation_mesh_node);
  if (navigation_mesh_node != nullptr)
  {
    navigation_mesh_node->disconnect("baked", this, "on_navigation_mesh_baked");
  }
  navigation_mesh_node = a_navigation_mesh_node;
  if (navigation_mesh_node != nullptr)
  {
    navigation_mesh_node->connect("baked", this, "on_navigation_mesh_baked");
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
  Godot::print("AdvancedNavigationAgent3D::set_target()");
  if (a_target == Vector3::INF)
  {
    // TODO: stop movement
    ERR_PRINT("Not implemented");
    return;
  }
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
  Godot::print("AdvancedNavigationAgent3D::get_position()");
  if (agent.is_valid())
  {
    return agent->get_position();
  }
  return Vector3::INF;
}

godot::Vector3 AdvancedNavigationAgent3D::get_target()
{
  Godot::print("AdvancedNavigationAgent3D::get_target()");
  return Vector3::INF;
}

void AdvancedNavigationAgent3D::try_fetching_crowd()
{
  Ref<DetourCrowd> a_crowd = navigation_mesh_node->get_crowd();
  if (a_crowd.is_null())
  {
    Godot::print("AdvancedNavigationAgent3D::try_fetching_crowd(): too early");
    return;
  }
  if (a_crowd == crowd)
  {
    Godot::print("AdvancedNavigationAgent3D::try_fetching_crowd(): already done");
    return;
  }
  crowd = a_crowd;
  agent = Ref<DetourCrowdAgent>(); // !TODO: deregister from server as well
  // !TODO: disconnect old agent's signals as well
  Godot::print("AdvancedNavigationAgent3D::try_fetching_crowd(): ok");
}

void AdvancedNavigationAgent3D::try_creating_agent()
{
  if (agent.is_valid() or crowd.is_null())
  {
    return;
  }
  if (requested_position == Vector3::INF)
  {
    Godot::print("AdvancedNavigationAgent3D::try_creating_agent(): waiting for position");
    return;
  }
  // TODO: create proper config:
  auto config = Ref<DetourCrowdAgentConfig>(DetourCrowdAgentConfig::_new());
  auto a_agent = crowd->create_agent(requested_position, config);
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
  auto* server =
      get_tree()->get_root()->get_node<AdvancedNavigationServer3D>("AdvancedNavigationServer3D");
  if (server == nullptr)
  {
    ERR_PRINT("Could not register agent, 'AdvancedNavigationServer3D' is missing");
    return;
  }
  server->register_detour_crowd_agent(crowd, agent);
  Godot::print("AdvancedNavigationAgent3D::try_creating_agent(): ok");
}

void AdvancedNavigationAgent3D::on_navigation_mesh_baked()
{
  try_fetching_crowd();
  try_creating_agent();
}

void AdvancedNavigationAgent3D::on_new_position(godot::Vector3 position)
{
  emit_signal("new_position", position);
}

void AdvancedNavigationAgent3D::on_new_velocity(godot::Vector3 velocity)
{
  emit_signal("new_velocity", velocity);
}
