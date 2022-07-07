#include "AdvancedNavigationCrowd3D.hpp"

#include <SceneTree.hpp>
#include <Viewport.hpp>

#include "AdvancedNavigationServer3D.hpp"

void AdvancedNavigationCrowd3D::_register_methods()
{
  // methods
  register_method("_ready", &AdvancedNavigationCrowd3D::_ready);
  register_method("set_navigation_mesh", &AdvancedNavigationCrowd3D::set_navigation_mesh);
  register_method("on_navigation_mesh_baked", &AdvancedNavigationCrowd3D::on_navigation_mesh_baked);

  // signals
  register_signal<AdvancedNavigationCrowd3D>("changed");
}

void AdvancedNavigationCrowd3D::_ready()
{
  // TODO: iterate all parents
  AdvancedNavigationMesh3D* navigation_mesh_node =
      Object::cast_to<AdvancedNavigationMesh3D>(get_parent());
  if (navigation_mesh_node)
  {
    set_navigation_mesh(navigation_mesh_node);
  }
}

void AdvancedNavigationCrowd3D::set_navigation_mesh(
    AdvancedNavigationMesh3D* a_navigation_mesh_node)
{
  // TODO: if node is different we should invalidate crowd
  if (navigation_mesh_node != nullptr)
  {
    navigation_mesh_node->disconnect("baked", this, "on_navigation_mesh_baked");
  }
  navigation_mesh_node = a_navigation_mesh_node;
  if (navigation_mesh_node != nullptr)
  {
    navigation_mesh_node->connect("baked", this, "on_navigation_mesh_baked");
    try_creating_crowd();
  }
}

Ref<DetourCrowd> AdvancedNavigationCrowd3D::get_crowd()
{
  return crowd;
}

void AdvancedNavigationCrowd3D::try_creating_crowd()
{
  auto a_navigation_mesh = navigation_mesh_node->get_navigation_mesh();
  if (a_navigation_mesh.is_null())
  {
    return;
  }
  if (crowd.is_valid() and crowd->get_detour_navigation_mesh_ref() == a_navigation_mesh)
  {
    return;
  }
  auto* server =
      get_tree()->get_root()->get_node<AdvancedNavigationServer3D>("AdvancedNavigationServer3D");
  if (server == nullptr)
  {
    ERR_PRINT("Could not create crowd, 'AdvancedNavigationServer3D' is missing");
    return;
  }
  if (crowd.is_valid())
  {
    server->deregister_detour_crowd(crowd);
  }
  // TODO: calculate crowd config:
  auto detour_crowd_config = Ref<DetourCrowdConfig>(DetourCrowdConfig::_new());
  auto a_crowd = a_navigation_mesh->create_crowd(detour_crowd_config);
  if (a_crowd.is_null())
  {
    ERR_PRINT("Failed creating 'DetourCrowd'");
    return;
  }
  crowd = a_crowd;
  server->register_detour_crowd(crowd);
  emit_signal("changed");
}

void AdvancedNavigationCrowd3D::on_navigation_mesh_baked()
{
  try_creating_crowd();
}
