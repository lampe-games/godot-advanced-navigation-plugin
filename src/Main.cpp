#include "AdvancedNavigationMesh3D.hpp"
#include "AdvancedNavigationServer3D.hpp"
#include "DetourNavigationMesh.hpp"
#include "RecastPolygonMesh.hpp"
#include "RecastPolygonMeshConfig.hpp"

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
  godot::register_tool_class<AdvancedNavigationMesh3D>();
  godot::register_tool_class<AdvancedNavigationServer3D>();
  godot::register_tool_class<RecastPolygonMesh>();
  godot::register_tool_class<RecastPolygonMeshConfig>();
  godot::register_tool_class<DetourNavigationMesh>();
}
