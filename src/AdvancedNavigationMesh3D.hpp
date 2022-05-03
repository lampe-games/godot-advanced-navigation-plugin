#include <Godot.hpp>
#include <Reference.hpp>
#include <Spatial.hpp>

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

  static void _register_methods()
  {
    register_method("_ready", &AdvancedNavigationMesh3D::_ready);
    register_method("_process", &AdvancedNavigationMesh3D::_process);

    register_method("bake", &AdvancedNavigationMesh3D::bake);
  }
};
