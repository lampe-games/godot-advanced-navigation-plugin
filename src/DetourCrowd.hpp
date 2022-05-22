#pragma once

#include <Godot.hpp>
#include <Resource.hpp>

class DetourCrowd : public godot::Resource
{
  GODOT_CLASS(DetourCrowd, godot::Resource);

 public:
  static void _register_methods() {}

  void _init() {}
};
