#pragma once

#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>

// TODO: implement move oprators and make this class a template
namespace detour
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

class NavMeshQuery
{
 public:
  NavMeshQuery() { raw_ptr = dtAllocNavMeshQuery(); }
  ~NavMeshQuery()
  {
    if (raw_ptr != nullptr)
    {
      dtFreeNavMeshQuery(raw_ptr);
    }
  }

 public:
  NavMeshQuery& operator=(const NavMeshQuery&) = delete;
  NavMeshQuery(const NavMeshQuery&) = delete;

 public:
  dtNavMeshQuery* ptr() { return raw_ptr; }
  dtNavMeshQuery& ref() { return *raw_ptr; }

 private:
  dtNavMeshQuery* raw_ptr{nullptr};
};
} // namespace detour
