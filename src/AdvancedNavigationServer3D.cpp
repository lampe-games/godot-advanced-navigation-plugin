#include "AdvancedNavigationServer3D.hpp"

#include <MeshInstance.hpp>
#include <PlaneMesh.hpp>

void AdvancedNavigationServer3D::_register_methods()
{
  register_method(
      "create_empty_input_geometry", &AdvancedNavigationServer3D::create_empty_input_geometry);
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
}

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

Ref<InputGeometry> AdvancedNavigationServer3D::create_empty_input_geometry() const
{
  return InputGeometry::_new();
}

Ref<RecastPolygonMeshConfig> AdvancedNavigationServer3D::create_empty_recast_polygon_mesh_config()
    const
{
  return RecastPolygonMeshConfig::_new();
  ;
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
