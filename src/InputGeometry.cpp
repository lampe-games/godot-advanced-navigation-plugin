#include "InputGeometry.hpp"

#include <Mesh.hpp>
#include <MeshInstance.hpp>
#include <Spatial.hpp>

using namespace godot;

void InputGeometry::add_nodes(godot::Array& nodes)
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

void InputGeometry::add_resources(godot::Array& resources)
{
  // godot::helpers::append_all(resources_to_parse, resources);
  // TODO: implement
}

Array InputGeometry::get_ccw_triangles()
{
  // TODO: refactor
  // TODO: check how to get triangles from meshes properly
  Array triangles;
  triangles.resize(Mesh::ARRAY_MAX);
  PoolVector3Array triangle_vertices;
  PoolIntArray triangle_indices;

  for (int node_index = 0; node_index < nodes_to_parse.size(); node_index++)
  {
    const auto& item = nodes_to_parse[node_index];
    if (Object::cast_to<MeshInstance>(item))
    {
      MeshInstance* mesh_instance = Object::cast_to<MeshInstance>(item);
      Ref<Mesh> mesh = mesh_instance->get_mesh();
      if (not mesh.is_valid())
      {
        continue;
      }
      auto transform = mesh_instance->get_global_transform();
      auto faces = mesh->get_faces();
      for (int face_index = 0; face_index < faces.size() / 3; face_index++)
      {
        triangle_vertices.append(transform.xform(faces[face_index * 3]));
        triangle_indices.append(triangle_vertices.size() - 1);
        triangle_vertices.append(transform.xform(faces[face_index * 3 + 2]));
        triangle_indices.append(triangle_vertices.size() - 1);
        triangle_vertices.append(transform.xform(faces[face_index * 3 + 1]));
        triangle_indices.append(triangle_vertices.size() - 1);
      }
    }
  }

  triangles[Mesh::ARRAY_VERTEX] = triangle_vertices;
  triangles[Mesh::ARRAY_INDEX] = triangle_indices;

  return triangles;
}
