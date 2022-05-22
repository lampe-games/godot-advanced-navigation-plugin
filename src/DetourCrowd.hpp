#pragma once

#include <memory>

#include <Godot.hpp>
#include <Resource.hpp>

#include "DetourCrowdConfig.hpp"
#include "DetourNavigationMesh.hpp"
#include "DetourWrappers.hpp"

class DetourCrowd : public godot::Resource
{
  GODOT_CLASS(DetourCrowd, godot::Resource);

 public:
  static void _register_methods();

  void _init() {}

  bool initialize(godot::Ref<DetourCrowdConfig>, godot::Ref<DetourNavigationMesh>);

 private:
  std::unique_ptr<detour::Crowd> detour_crowd{nullptr};
  std::shared_ptr<detour::NavMesh> detour_nav_mesh{nullptr};
};
