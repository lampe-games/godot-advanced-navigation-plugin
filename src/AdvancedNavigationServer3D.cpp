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

Ref<RecastPolygonMesh> AdvancedNavigationServer3D::build_polygon_mesh()
{
  Godot::print("AdvancedNavigationServer3D::build_polygon_mesh()");
  Ref<RecastPolygonMesh> rc_poly_mesh{RecastPolygonMesh::_new()};
  rc_poly_mesh->build_from_plane_mesh(Ref<PlaneMesh>(PlaneMesh::_new()));
  return rc_poly_mesh;
}
