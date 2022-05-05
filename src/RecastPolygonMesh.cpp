#include "RecastPolygonMesh.hpp"

#include <Recast.h>

using namespace godot;

RecastPolygonMesh::RecastPolygonMesh()
{
  Godot::print("RecastPolygonMesh::RecastPolygonMesh()");
}

RecastPolygonMesh::~RecastPolygonMesh()
{
  Godot::print("RecastPolygonMesh::~RecastPolygonMesh()");
}

void RecastPolygonMesh::_init()
{
  Godot::print("RecastPolygonMesh::_init()");
}

void RecastPolygonMesh::build_from_plane_mesh(godot::Ref<godot::PlaneMesh> plane_mesh)
{
  Godot::print("RecastPolygonMesh::build_from_plane_mesh()");

  rcHeightfield* height_field = rcAllocHeightfield();
  if (height_field == nullptr)
  {
    ERR_PRINT("rcAllocHeightfield() failed");
    return;
  }

  rcContext recast_context;

  rcConfig recast_config;
  recast_config.width = 64;
  recast_config.height = 64;
  recast_config.tileSize = 64;
  recast_config.borderSize = 0;
  recast_config.cs = 0.3;
  recast_config.ch = 0.3;
  recast_config.bmin[0] = -10;
  recast_config.bmin[1] = -10;
  recast_config.bmin[2] = -10;
  recast_config.bmax[0] = 10;
  recast_config.bmax[1] = 10;
  recast_config.bmax[2] = 10;
  recast_config.walkableSlopeAngle = 45; // [dg]
  recast_config.walkableHeight = 7; // [vx], ceil (2[agent_height] / 0.3[ch])
  recast_config.walkableClimb = 1; // [vx]
  recast_config.walkableRadius = 2; // [vx]
  recast_config.maxEdgeLen = 40; // [vx], 12/0.3[ch]
  recast_config.maxSimplificationError = 1.3; // [vx]
  recast_config.minRegionArea = 2; // [vx]
  recast_config.mergeRegionArea = 4; // [vx]
  recast_config.maxVertsPerPoly = 6;
  recast_config.detailSampleDist = 1.8; // [wu]
  recast_config.detailSampleMaxError = 0.3; // [wu]

  if (not rcCreateHeightfield(
          &recast_context,
          *height_field,
          recast_config.width,
          recast_config.height,
          recast_config.bmin,
          recast_config.bmax,
          recast_config.cs,
          recast_config.ch))
  {
    ERR_PRINT("rcCreateHeightfield() failed");
    return;
  }

  // TODO: check original godot pipeline
  // TODO: rcMarkWalkableTriangles
  // TODO: rcRasterizeTriangles
  // TODO: rcFilterLowHangingWalkableObstacles
  // TODO: rcFilterLedgeSpans
  // TODO: rcFilterWalkableLowHeightSpans
  // TODO: rcBuildCompactHeightfield
  // TODO: rcErodeWalkableArea
  // TODO: rcBuildRegionsMonotone
  // TODO: Build a rcContourSet.
  // TODO: Build a rcPolyMesh.
  // TODO: Build a rcPolyMeshDetail.
  rcFreeHeightField(height_field);
}

Ref<Mesh> RecastPolygonMesh::get_mesh() const
{
  Godot::print("RecastPolygonMesh::get_mesh()");
  return PlaneMesh::_new();
}
