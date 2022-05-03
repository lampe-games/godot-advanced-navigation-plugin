#include "AdvancedNavigationMesh3D.hpp"

void AdvancedNavigationMesh3D::_init()
{
  Godot::print("AdvancedNavigationMesh3D::_init()");
}

void AdvancedNavigationMesh3D::_ready()
{
  Godot::print("AdvancedNavigationMesh3D::_ready()");
}

void AdvancedNavigationMesh3D::_process()
{
  Godot::print("AdvancedNavigationMesh3D::_process()");
  set_process(false);
}

void AdvancedNavigationMesh3D::bake()
{
  Godot::print("AdvancedNavigationMesh3D::bake()");
  set_process(false);
}
