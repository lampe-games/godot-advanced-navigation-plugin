#include "InputGeometry.hpp"

#include <BoxShape.hpp>
#include <CubeMesh.hpp>
#include <Mesh.hpp>
#include <MeshInstance.hpp>
#include <Shape.hpp>
#include <Spatial.hpp>
#include <StaticBody.hpp>

using namespace godot;

void InputGeometry::_register_methods()
{
  register_method("add_nodes", &InputGeometry::add_nodes);
  register_method("add_resources", &InputGeometry::add_resources);
  register_method("add_resources_with_transforms", &InputGeometry::add_resources_with_transforms);
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
    if (not Object::cast_to<MeshInstance>(node) and not Object::cast_to<StaticBody>(node))
    {
      continue;
    }
    // TODO: don't add duplicates
    nodes_to_parse.append(node);
  }
}

void InputGeometry::add_resources(godot::Array resources)
{
  godot::Array transforms;
  // TODO: figure out if better way possible:
  for (int resource_index = 0; resource_index < resources.size(); resource_index++)
  {
    transforms.append(Transform());
  }
  add_resources_with_transforms(resources, transforms);
}

void InputGeometry::add_resources_with_transforms(godot::Array resources, godot::Array transforms)
{
  if (resources.size() > transforms.size())
  {
    ERR_PRINT("Some transforms missing");
  }
  if (resources.size() < transforms.size())
  {
    ERR_PRINT("Too much transforms provided");
  }
  for (int resource_index = 0; resource_index < resources.size(); resource_index++)
  {
    // TODO: handle static colliders
    if (Ref<Mesh>(resources[resource_index]).is_valid() and
        transforms[resource_index].get_type() == Variant::TRANSFORM)
    {
      resources_to_parse.append(resources[resource_index]);
      resources_to_parse_transforms.append(transforms[resource_index]);
    }
  }
}

Array InputGeometry::get_ccw_triangles()
{
  // TODO: refactor
  // TODO: do not process duplicates (same resources w/ same transforms)
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
    else if (Object::cast_to<StaticBody>(item))
    {
      StaticBody* static_body = Object::cast_to<StaticBody>(item);
      auto shape_owners = static_body->get_shape_owners();
      for (int shape_owner_index = 0; shape_owner_index < shape_owners.size(); shape_owner_index++)
      {
        auto shape_owner_id = shape_owners[shape_owner_index];
        const auto shape_transform = static_body->get_global_transform() *
            static_body->shape_owner_get_transform(shape_owner_id);
        for (int shape_index = 0;
             shape_index < static_body->shape_owner_get_shape_count(shape_owner_id);
             shape_index++)
        {
          Ref<Shape> shape = static_body->shape_owner_get_shape(shape_owner_id, shape_index);
          if (Object::cast_to<BoxShape>(*shape))
          {
            BoxShape* box = Object::cast_to<BoxShape>(*shape);
            Ref<CubeMesh> cube_mesh;
            cube_mesh.instance();
            cube_mesh->set_size(box->get_extents() * 2.0);
            copy_mesh_to_arrays_ccw(
                **cube_mesh, shape_transform, triangle_vertices, triangle_indices);
          }
          else
          {
            ERR_PRINT(
                String("'{0}' converter to mesh is not implemented").format(Array::make(shape)));
            // TODO: implement others using 3035b9c44c5b026e2bf6f6faa6589271f90de320 from Godot
          }
        }
      }
    }
  }

  // extract geometry from resources
  for (int resource_index = 0; resource_index < resources_to_parse.size(); resource_index++)
  {
    Ref<Mesh> mesh = resources_to_parse[resource_index];
    if (mesh.is_valid())
    {
      copy_mesh_to_arrays_ccw(
          **mesh,
          static_cast<Transform>(resources_to_parse_transforms[resource_index]),
          triangle_vertices,
          triangle_indices);
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
