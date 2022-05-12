#pragma once

#include <DetourNavMesh.h>

// TODO: implement move oprators and make this class a template
namespace Detour
{
class NavMesh
{
 public:
  NavMesh() { raw_ptr = dtAllocNavMesh(); }
  ~NavMesh()
  {
    if (raw_ptr != nullptr)
    {
      dtFreeNavMesh(raw_ptr);
    }
  }

 public:
  NavMesh& operator=(const NavMesh&) = delete;
  NavMesh(const NavMesh&) = delete;

 public:
  dtNavMesh* ptr() { return raw_ptr; }
  dtNavMesh& ref() { return *raw_ptr; }

 private:
  dtNavMesh* raw_ptr{nullptr};
};
} // namespace Detour
