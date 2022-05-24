#pragma once

#include <Godot.hpp>
#include <Resource.hpp>

#include "DetourCrowd.hpp"

class DetourCrowdAgent : public godot::Resource
{
  GODOT_CLASS(DetourCrowdAgent, godot::Resource);

 public:
  enum State
  {
    INVALID = 0,
    WALKING,
    OFFMESH
  };
  static void _register_methods();

  void _init() {}

  bool initialize(
      godot::Vector3 position,
      godot::Ref<DetourCrowdAgentConfig>,
      godot::Ref<DetourCrowd>);

  godot::Vector3 get_position() const;
  int get_state() const;

  int get_state_invalid() const { return State::INVALID; }
  int get_state_walking() const { return State::WALKING; }
  int get_state_offmesh() const { return State::OFFMESH; }

 public:
  std::shared_ptr<detour::Crowd> raw_detour_crowd{nullptr};
  const dtCrowdAgent* const_detour_crowd_agent{nullptr};
  dtCrowdAgent* detour_crowd_agent{nullptr};
};
