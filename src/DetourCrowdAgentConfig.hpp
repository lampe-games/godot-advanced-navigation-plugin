#pragma once

#include <Godot.hpp>
#include <Resource.hpp>

class DetourCrowdAgentConfig : public godot::Resource
{
  GODOT_CLASS(DetourCrowdAgentConfig, godot::Resource);

 public:
  void _init() {}

  static constexpr float default_radius{0.6};
  // static constexpr float default_height{}; // TODO:
  static constexpr float default_max_acceleration{0.6};
  static constexpr float default_max_speed = 1.2;

  static void _register_methods()
  {
    // TODO: add hints
    godot::register_property<DetourCrowdAgentConfig, float>(
        "radius", &DetourCrowdAgentConfig::radius, default_radius);
    godot::register_property<DetourCrowdAgentConfig, float>(
        "max_acceleration", &DetourCrowdAgentConfig::max_acceleration, default_max_acceleration);
    godot::register_property<DetourCrowdAgentConfig, float>(
        "max_speed", &DetourCrowdAgentConfig::max_speed, default_max_speed);
  }

 public:
  float radius{default_radius}; // >=0, 0 not working, so more likely >0
  // float height;
  float max_acceleration{default_max_acceleration}; // >= 0
  float max_speed{default_max_speed}; // >= 0
};
