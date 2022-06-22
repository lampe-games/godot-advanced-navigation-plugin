#pragma once

#include <memory>

#include <Godot.hpp>
#include <Resource.hpp>

#include "DetourCrowdAgentConfig.hpp"
#include "DetourCrowdConfig.hpp"
#include "DetourNavigationMesh.hpp"
#include "DetourWrappers.hpp"

class DetourCrowdAgent;

class DetourCrowd : public godot::Resource
{
  GODOT_CLASS(DetourCrowd, godot::Resource);

 public:
  static void _register_methods();

  void _init() {}

  bool initialize(godot::Ref<DetourCrowdConfig>, godot::Ref<DetourNavigationMesh>);

  godot::Ref<DetourCrowdAgent> create_agent(
      godot::Vector3 position,
      godot::Ref<DetourCrowdAgentConfig>);

  void update(float seconds);

  godot::Ref<DetourNavigationMesh> get_detour_navigation_mesh_ref();
  std::shared_ptr<detour::Crowd> get_detour_crowd();

 private:
  godot::Ref<DetourNavigationMesh> detour_navigation_mesh_ref{nullptr};
  std::shared_ptr<detour::Crowd> detour_crowd{nullptr};
  std::shared_ptr<detour::NavMesh> detour_nav_mesh{nullptr};
};
