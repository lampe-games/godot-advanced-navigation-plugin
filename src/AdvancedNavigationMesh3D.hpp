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

  static void _register_methods()
  {
    register_method("_ready", &AdvancedNavigationMesh3D::_ready);
    ;
  }
};
