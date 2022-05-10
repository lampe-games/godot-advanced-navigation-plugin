#pragma once

#include <Godot.hpp>
#include <Resource.hpp>

class DetourNavigationMesh : public godot::Resource
{
  GODOT_CLASS(DetourNavigationMesh, godot::Resource);

 public:
  void _init() {}

  static void _register_methods()
  {
    godot::register_property<DetourNavigationMesh, bool>(
        "dummy",
        &DetourNavigationMesh::dummy,
        false,
        GODOT_METHOD_RPC_MODE_DISABLED,
        GODOT_PROPERTY_USAGE_STORAGE,
        GODOT_PROPERTY_HINT_NONE);
  }

 private:
  bool dummy{false};
};
