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
  Ref<PlaneMesh> plane_mesh{PlaneMesh::_new()};
  plane_mesh->set_size(Vector2(5, 5));
  rc_poly_mesh->build_from_plane_mesh(plane_mesh);
  return rc_poly_mesh;
}
