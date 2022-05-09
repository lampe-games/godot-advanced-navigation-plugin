#include "AdvancedNavigationServer3D.hpp"

#include <MeshInstance.hpp>
#include <PlaneMesh.hpp>

Ref<RecastPolygonMesh> AdvancedNavigationServer3D::build_polygon_mesh(
    Array& nodes_to_parse_geometry_from,
    Ref<RecastPolygonMeshConfig> config)
{
  auto triangles = get_triangles_from_geometry(nodes_to_parse_geometry_from);
  // TODO: ensure no copy
  PoolVector3Array triangle_vertices = triangles[Mesh::ARRAY_VERTEX];
  PoolIntArray triangle_indices = triangles[Mesh::ARRAY_INDEX];
  Ref<RecastPolygonMesh> rc_poly_mesh{RecastPolygonMesh::_new()};
  rc_poly_mesh->build_from_triangles(triangle_vertices, triangle_indices, config);
  return rc_poly_mesh;
}

Array AdvancedNavigationServer3D::get_triangles_from_geometry(Array& nodes_to_parse_geometry_from)
{
  // TODO: refactor
  // TODO: consider reordering here
  // TODO: check how to get triangles from meshes properly
  Array triangles;
  triangles.resize(Mesh::ARRAY_MAX);
  PoolVector3Array triangle_vertices;
  PoolIntArray triangle_indices;

  for (int node_index = 0; node_index < nodes_to_parse_geometry_from.size(); node_index++)
  {
    const auto& item = nodes_to_parse_geometry_from[node_index];
    if (not Object::cast_to<Spatial>(item))
    {
      continue;
    }
    Spatial* node = Object::cast_to<Spatial>(item);
    if (node->has_meta("advanced_navigation:debug_mesh"))
    {
      continue;
    }
    auto transform = node->get_global_transform();
    if (Object::cast_to<MeshInstance>(item))
    {
      MeshInstance* mesh_instance = Object::cast_to<MeshInstance>(item);
      Ref<Mesh> mesh = mesh_instance->get_mesh();
      if (not mesh.is_valid())
      {
        continue;
      }
      auto faces = mesh->get_faces();
      for (int face_index = 0; face_index < faces.size() / 3; face_index++)
      {
        triangle_vertices.append(transform.xform(faces[face_index * 3]));
        triangle_indices.append(triangle_vertices.size() - 1);
        triangle_vertices.append(transform.xform(faces[face_index * 3 + 1]));
        triangle_indices.append(triangle_vertices.size() - 1);
        triangle_vertices.append(transform.xform(faces[face_index * 3 + 2]));
        triangle_indices.append(triangle_vertices.size() - 1);
      }
    }
  }

  triangles[Mesh::ARRAY_VERTEX] = triangle_vertices;
  triangles[Mesh::ARRAY_INDEX] = triangle_indices;
  return triangles;
}
