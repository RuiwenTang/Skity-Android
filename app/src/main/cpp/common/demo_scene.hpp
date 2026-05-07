#pragma once

namespace skity::demo {

enum class DemoBackend {
  kAuto = 0,
  kGles = 1,
  kVulkan = 2,
};

enum class DemoScene {
  kClear = 0,
  kShapes = 1,
  kPaths = 2,
  kGradients = 3,
  kText = 4,
  kImage = 5,
  kArcs = 6,
  kTransforms = 7,
  kStrokes = 8,
  kTiling = 9,
  kClips = 10,
  kLayers = 11,
  kTextCloud = 12,
  kStressPaths = 13,
  kDashboard = 14,
  kIconList = 15,
  kCompositeStack = 16,
};

}  // namespace skity::demo
