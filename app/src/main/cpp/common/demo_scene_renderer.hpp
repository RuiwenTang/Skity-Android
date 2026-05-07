#pragma once

#include "demo_scene.hpp"

namespace skity {
class Canvas;
}

namespace skity::demo {

void DrawDemoScene(Canvas* canvas, DemoScene scene, DemoBackend backend,
                   bool validation_enabled, int width, int height);

}  // namespace skity::demo
