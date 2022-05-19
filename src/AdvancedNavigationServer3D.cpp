#include "AdvancedNavigationServer3D.hpp"

#include <MeshInstance.hpp>
#include <PlaneMesh.hpp>

Ref<RecastPolygonMesh> AdvancedNavigationServer3D::build_polygon_mesh(
    Ref<InputGeometry> input_geometry,
    Ref<RecastPolygonMeshConfig> config)
{
  Ref<RecastPolygonMesh> rc_poly_mesh{RecastPolygonMesh::_new()};
  if (rc_poly_mesh->build_from_input_geometry(input_geometry, config))
  {
    return rc_poly_mesh;
  }
  return nullptr;
}

Ref<DetourNavigationMesh> AdvancedNavigationServer3D::build_navigation_mesh(
    Ref<RecastPolygonMesh> polygon_mesh,
    Ref<DetourNavigationMeshConfig> config)
{
  Ref<DetourNavigationMesh> navigation_mesh{DetourNavigationMesh::_new()};
  if (navigation_mesh->build_from_polygon_mesh(polygon_mesh, config))
  {
    return navigation_mesh;
  }
  return nullptr;
}
