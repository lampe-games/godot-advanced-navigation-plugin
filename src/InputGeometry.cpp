#include "InputGeometry.hpp"

#include <Mesh.hpp>
#include <MeshInstance.hpp>
#include <Spatial.hpp>

using namespace godot;

void InputGeometry::_register_methods()
{
  register_method("add_nodes", &InputGeometry::add_nodes);
  register_method("add_resources", &InputGeometry::add_resources);
}

void InputGeometry::add_nodes(godot::Array nodes)
{
  for (int node_index = 0; node_index < nodes.size(); node_index++)
  {
    const auto& item = nodes[node_index];
    if (not Object::cast_to<Spatial>(item))
    {
      continue;
    }
    Spatial* node = Object::cast_to<Spatial>(item);
    if (node->has_meta("advanced_navigation:debug_mesh"))
    {
      continue;
    }
    // TODO: handle other nodes as well
    if (not Object::cast_to<MeshInstance>(node))
    {
      continue;
    }
    nodes_to_parse.append(node);
  }
}

void InputGeometry::add_resources(godot::Array resources)
{
  for (int resource_index = 0; resource_index < resources.size(); resource_index++)
  {
    // TODO: handle static colliders
    if (Ref<Mesh>(resources[resource_index]).is_valid())
    {
      resources_to_parse.append(resources[resource_index]);
    }
  }
}

Array InputGeometry::get_ccw_triangles()
{
  // TODO: refactor
  // TODO: check how to get triangles from meshes properly
  // TODO: do not process duplicates
  Array triangles;
  triangles.resize(Mesh::ARRAY_MAX);
  PoolVector3Array triangle_vertices;
  PoolIntArray triangle_indices;

  // extract geometry from nodes
  for (int node_index = 0; node_index < nodes_to_parse.size(); node_index++)
  {
    const auto& item = nodes_to_parse[node_index];
    if (Object::cast_to<MeshInstance>(item))
    {
      MeshInstance* mesh_instance = Object::cast_to<MeshInstance>(item);
      Ref<Mesh> mesh = mesh_instance->get_mesh();
      if (mesh.is_valid())
      {
        copy_mesh_to_arrays_ccw(
            **mesh, mesh_instance->get_global_transform(), triangle_vertices, triangle_indices);
      }
    }
  }

  // extract geometry from resources
  for (int resource_index = 0; resource_index < resources_to_parse.size(); resource_index++)
  {
    Ref<Mesh> mesh = resources_to_parse[resource_index];
    if (mesh.is_valid())
    {
      copy_mesh_to_arrays_ccw(**mesh, Transform(), triangle_vertices, triangle_indices);
    }
  }

  triangles[Mesh::ARRAY_VERTEX] = triangle_vertices;
  triangles[Mesh::ARRAY_INDEX] = triangle_indices;

  return triangles;
}

void InputGeometry::copy_mesh_to_arrays_ccw(
    const Mesh& mesh,
    const Transform& global_transform,
    PoolVector3Array& triangle_vertices,
    PoolIntArray& triangle_indices)
{
  auto faces = mesh.get_faces();
  for (int face_index = 0; face_index < faces.size() / 3; face_index++)
  {
    triangle_vertices.append(global_transform.xform(faces[face_index * 3]));
    triangle_indices.append(triangle_vertices.size() - 1);
    triangle_vertices.append(global_transform.xform(faces[face_index * 3 + 2]));
    triangle_indices.append(triangle_vertices.size() - 1);
    triangle_vertices.append(global_transform.xform(faces[face_index * 3 + 1]));
    triangle_indices.append(triangle_vertices.size() - 1);
  }
}
