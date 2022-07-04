#include "AdvancedNavigationAgent3D.hpp"

using namespace godot;

void AdvancedNavigationAgent3D::_register_methods()
{
  register_method("_ready", &AdvancedNavigationAgent3D::_ready);
  register_method("set_navigation_mesh", &AdvancedNavigationAgent3D::set_navigation_mesh);
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
    try_creating_agent();
  }
}

void AdvancedNavigationAgent3D::try_creating_agent()
{
  Ref<DetourCrowd> a_crowd = navigation_mesh_node->get_crowd();
  if (a_crowd.is_null())
  {
    Godot::print("AdvancedNavigationAgent3D::try_creating_agent(): too early");
    return;
  }
  // TODO: implement
  Godot::print("AdvancedNavigationAgent3D::try_creating_agent(): ok");
}

void AdvancedNavigationAgent3D::on_navigation_mesh_baked()
{
  try_creating_agent();
}
