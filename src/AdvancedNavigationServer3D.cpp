#include "AdvancedNavigationServer3D.hpp"

#include <MeshInstance.hpp>
#include <PlaneMesh.hpp>

using namespace godot;

void AdvancedNavigationServer3D::_register_methods()
{
  register_method("_physics_process", &AdvancedNavigationServer3D::_physics_process);
  register_method("register_detour_crowd", &AdvancedNavigationServer3D::register_detour_crowd);
  register_method("deregister_detour_crowd", &AdvancedNavigationServer3D::deregister_detour_crowd);
  register_method(
      "register_detour_crowd_agent", &AdvancedNavigationServer3D::register_detour_crowd_agent);
  register_method(
      "deregister_detour_crowd_agent", &AdvancedNavigationServer3D::deregister_detour_crowd_agent);
  register_method(
      "create_empty_input_geometry", &AdvancedNavigationServer3D::create_empty_input_geometry);
  register_method(
      "get_empty_input_geometry", &AdvancedNavigationServer3D::create_empty_input_geometry);
  register_method(
      "create_empty_recast_polygon_mesh_config",
      &AdvancedNavigationServer3D::create_empty_recast_polygon_mesh_config);
  register_method(
      "create_empty_detour_navigation_mesh_config",
      &AdvancedNavigationServer3D::create_empty_detour_navigation_mesh_config);
  register_method(
      "create_empty_detour_navigation_mesh",
      &AdvancedNavigationServer3D::create_empty_detour_navigation_mesh);
  register_method(
      "create_empty_detour_crowd_config",
      &AdvancedNavigationServer3D::create_empty_detour_crowd_config);
  register_method(
      "create_empty_detour_crowd", &AdvancedNavigationServer3D::create_empty_detour_crowd);
  register_method(
      "create_empty_detour_crowd_agent_config",
      &AdvancedNavigationServer3D::create_empty_detour_crowd_agent_config);
}

void AdvancedNavigationServer3D::_physics_process(float delta)
{
  for (auto crowd_it = crowds_to_update.begin(); crowd_it != crowds_to_update.end(); crowd_it++)
  {
    godot::Ref<DetourCrowd> crowd = *crowd_it;
    crowd->update(delta);
    if (crowd_agents_to_update.count(crowd) > 0)
    {
      for (auto agent_it = crowd_agents_to_update[crowd].begin();
           agent_it != crowd_agents_to_update[crowd].end();
           agent_it++)
      {
        godot::Ref<DetourCrowdAgent> agent = *agent_it;
        agent->update();
      }
    }
  }
}

void AdvancedNavigationServer3D::register_detour_crowd(Ref<DetourCrowd> crowd)
{
  if (crowd.is_valid())
  {
    crowds_to_update.insert(crowd);
  }
}

void AdvancedNavigationServer3D::deregister_detour_crowd(Ref<DetourCrowd> crowd)
{
  if (crowd.is_valid())
  {
    crowds_to_update.erase(crowd);
  }
}

void AdvancedNavigationServer3D::register_detour_crowd_agent(
    godot::Ref<DetourCrowd> crowd,
    godot::Ref<DetourCrowdAgent> agent)
{
  crowd_agents_to_update[crowd].insert(agent);
}

void AdvancedNavigationServer3D::deregister_detour_crowd_agent(
    godot::Ref<DetourCrowd> crowd,
    godot::Ref<DetourCrowdAgent> agent)
{
  if (crowd_agents_to_update.count(crowd) == 0)
  {
    return;
  }
  crowd_agents_to_update[crowd].erase(agent);
  if (crowd_agents_to_update[crowd].empty())
  {
    crowd_agents_to_update.erase(crowd);
  }
}

Ref<InputGeometry> AdvancedNavigationServer3D::create_empty_input_geometry() const
{
  return InputGeometry::_new();
}

Ref<RecastPolygonMeshConfig> AdvancedNavigationServer3D::create_empty_recast_polygon_mesh_config()
    const
{
  return RecastPolygonMeshConfig::_new();
}

Ref<DetourNavigationMeshConfig> AdvancedNavigationServer3D::
    create_empty_detour_navigation_mesh_config() const
{
  return DetourNavigationMeshConfig::_new();
}

Ref<DetourNavigationMesh> AdvancedNavigationServer3D::create_empty_detour_navigation_mesh() const
{
  return DetourNavigationMesh::_new();
}

Ref<DetourCrowdConfig> AdvancedNavigationServer3D::create_empty_detour_crowd_config() const
{
  return DetourCrowdConfig::_new();
}

Ref<DetourCrowd> AdvancedNavigationServer3D::create_empty_detour_crowd() const
{
  return DetourCrowd::_new();
}

Ref<DetourCrowdAgentConfig> AdvancedNavigationServer3D::create_empty_detour_crowd_agent_config()
    const
{
  return DetourCrowdAgentConfig::_new();
}
