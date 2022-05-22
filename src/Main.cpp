#include "AdvancedNavigationMesh3D.hpp"
#include "AdvancedNavigationServer3D.hpp"
#include "DetourCrowd.hpp"
#include "DetourNavigationMesh.hpp"
#include "InputGeometry.hpp"
#include "RecastPolygonMesh.hpp"

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options* o)
{
  godot::Godot::gdnative_init(o);
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options* o)
{
  godot::Godot::gdnative_terminate(o);
}

extern "C" void GDN_EXPORT godot_nativescript_init(void* handle)
{
  godot::Godot::nativescript_init(handle);
  // nodes:
  godot::register_tool_class<AdvancedNavigationMesh3D>();
  godot::register_tool_class<AdvancedNavigationServer3D>();
  // resources:
  godot::register_tool_class<InputGeometry>();
  godot::register_tool_class<RecastPolygonMeshConfig>();
  godot::register_tool_class<RecastPolygonMesh>();
  godot::register_tool_class<DetourNavigationMeshConfig>();
  godot::register_tool_class<DetourNavigationMesh>();
  godot::register_tool_class<DetourCrowdConfig>();
  godot::register_tool_class<DetourCrowd>();
}
