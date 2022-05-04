#include <Godot.hpp>
#include <Material.hpp>
#include <MeshInstance.hpp>
#include <Reference.hpp>
#include <Spatial.hpp>
#include <SpatialMaterial.hpp>

using namespace godot;

class AdvancedNavigationMesh3D : public Spatial
{
  GODOT_CLASS(AdvancedNavigationMesh3D, Spatial);

 public:
  AdvancedNavigationMesh3D() {}

  void _init(); // `_init` must exist as it is called by Godot.
  void _ready();
  void _process();

  void bake();
  void clear();

  static void _register_methods()
  {
    register_method("_ready", &AdvancedNavigationMesh3D::_ready);
    register_method("_process", &AdvancedNavigationMesh3D::_process);

    register_method("bake", &AdvancedNavigationMesh3D::bake);
    register_method("clear", &AdvancedNavigationMesh3D::clear);
  }

 private:
  void create_debug_mesh_instance();
  void update_debug_mesh_instance(Ref<Mesh>);
  Ref<Mesh> create_debug_mesh();
  Ref<Material> create_debug_mesh_instance_material();

 private:
  MeshInstance* debug_mesh_instance{nullptr};
};
