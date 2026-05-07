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
};

}  // namespace skity::demo
