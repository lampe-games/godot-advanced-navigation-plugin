#include "AdvancedNavigationServer3D.hpp"

#include <PlaneMesh.hpp>

void AdvancedNavigationServer3D::_init()
{
  Godot::print("AdvancedNavigationServer3D::_init()");
}

void AdvancedNavigationServer3D::_ready()
{
  Godot::print("AdvancedNavigationServer3D::_ready()");
}

Ref<Mesh> AdvancedNavigationServer3D::build_navigation_mesh()
{
  Godot::print("AdvancedNavigationServer3D::build_navigation_mesh()");
  return PlaneMesh::_new();
}
