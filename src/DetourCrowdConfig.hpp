#pragma once

#include <Godot.hpp>
#include <Resource.hpp>

class DetourCrowdConfig : public godot::Resource
{
  GODOT_CLASS(DetourCrowdConfig, godot::Resource);

 public:
  static void _register_methods()
  {
    // TODO: add ranges
  }

  void _init() {}

  static constexpr int default_max_agents{25};
  static constexpr float default_max_agent_radius{0.6};

 public:
  int max_agents{default_max_agents}; // >=1
  float max_agent_radius{default_max_agent_radius}; // >0
};
