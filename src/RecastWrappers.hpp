#pragma once

#include <Recast.h>

namespace Recast
{
class Heightfield
{
 public:
  Heightfield() { raw_ptr = rcAllocHeightfield(); }
  ~Heightfield()
  {
    if (raw_ptr != nullptr)
    {
      rcFreeHeightField(raw_ptr);
    }
  }

 public:
  Heightfield& operator=(const Heightfield&) = delete;
  Heightfield(const Heightfield&) = delete;

 public:
  rcHeightfield* ptr() { return raw_ptr; }
  rcHeightfield& ref() { return *raw_ptr; }

 private:
  rcHeightfield* raw_ptr{nullptr};
};

class CompactHeightfield
{
 public:
  CompactHeightfield() { raw_ptr = rcAllocCompactHeightfield(); }
  ~CompactHeightfield()
  {
    if (raw_ptr != nullptr)
    {
      rcFreeCompactHeightfield(raw_ptr);
    }
  }

 public:
  CompactHeightfield& operator=(const CompactHeightfield&) = delete;
  CompactHeightfield(const CompactHeightfield&) = delete;

 public:
  rcCompactHeightfield* ptr() { return raw_ptr; }
  rcCompactHeightfield& ref() { return *raw_ptr; }

 private:
  rcCompactHeightfield* raw_ptr{nullptr};
};

class ContourSet
{
 public:
  ContourSet() { raw_ptr = rcAllocContourSet(); }
  ~ContourSet()
  {
    if (raw_ptr != nullptr)
    {
      rcFreeContourSet(raw_ptr);
    }
  }

 public:
  ContourSet& operator=(const ContourSet&) = delete;
  ContourSet(const ContourSet&) = delete;

 public:
  rcContourSet* ptr() { return raw_ptr; }
  rcContourSet& ref() { return *raw_ptr; }

 private:
  rcContourSet* raw_ptr{nullptr};
};

class PolyMesh
{
 public:
  PolyMesh() { raw_ptr = rcAllocPolyMesh(); }
  ~PolyMesh()
  {
    if (raw_ptr != nullptr)
    {
      rcFreePolyMesh(raw_ptr);
    }
  }

 public:
  PolyMesh& operator=(const PolyMesh&) = delete;
  PolyMesh(const PolyMesh&) = delete;

 public:
  rcPolyMesh* ptr() { return raw_ptr; }
  rcPolyMesh& ref() { return *raw_ptr; }

 private:
  rcPolyMesh* raw_ptr{nullptr};
};

class PolyMeshDetail
{
 public:
  PolyMeshDetail() { raw_ptr = rcAllocPolyMeshDetail(); }
  ~PolyMeshDetail()
  {
    if (raw_ptr != nullptr)
    {
      rcFreePolyMeshDetail(raw_ptr);
    }
  }

 public:
  PolyMeshDetail& operator=(const PolyMeshDetail&) = delete;
  PolyMeshDetail(const PolyMeshDetail&) = delete;

 public:
  rcPolyMeshDetail* ptr() { return raw_ptr; }
  rcPolyMeshDetail& ref() { return *raw_ptr; }

 private:
  rcPolyMeshDetail* raw_ptr{nullptr};
};
} // namespace Recast
