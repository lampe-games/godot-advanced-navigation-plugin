#pragma once

#include <optional>

#include <Godot.hpp>
#include <Resource.hpp>

#include "DetourCrowd.hpp"

class DetourCrowdAgent : public godot::Resource
{
  GODOT_CLASS(DetourCrowdAgent, godot::Resource);

 public:
  enum State
  {
    UNINITIALIZED = 0,
    DISABLED,
    INVALID,
    WALKING,
    OFFMESH,
  };

  static void _register_methods();

  void _init() {}

  bool initialize(
      godot::Vector3 position,
      godot::Ref<DetourCrowdAgentConfig>,
      godot::Ref<DetourCrowd>);

  bool enable();
  void disable();
  bool is_enabled() const { return detour_crowd_agent.has_value(); }

  void void_set_position(godot::Vector3 target);
  bool set_position(godot::Vector3 target);
  bool set_position_with_extents(godot::Vector3 target, godot::Vector3 search_box_half_extents);
  godot::Vector3 get_position() const;

  bool set_target(godot::Vector3 target);
  bool set_target_with_extents(godot::Vector3 target, godot::Vector3 search_box_half_extents);
  godot::Vector3 get_target() const;

  godot::Vector3 get_velocity() const;
  int get_state() const;

  int get_state_uninitialized() const { return State::UNINITIALIZED; }
  int get_state_disabled() const { return State::DISABLED; }
  int get_state_invalid() const { return State::INVALID; }
  int get_state_walking() const { return State::WALKING; }
  int get_state_offmesh() const { return State::OFFMESH; }

 private:
  void on_crowd_updated();

 private:
  godot::Ref<DetourNavigationMesh> detour_navigation_mesh_ref{nullptr};
  std::shared_ptr<detour::Crowd> detour_crowd{nullptr};
  std::optional<detour::CrowdAgent> detour_crowd_agent{};
  std::optional<godot::Vector3> pending_position{};
  std::optional<godot::Vector3> pending_target{};
  std::optional<dtPolyRef> pending_target_polygon{};
  std::optional<dtCrowdAgentParams> pending_params{};
};
