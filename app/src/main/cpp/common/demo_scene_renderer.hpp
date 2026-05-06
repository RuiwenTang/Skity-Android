#pragma once

#include "demo_scene.hpp"

namespace skity {
class Canvas;
}

namespace skity::demo {

void DrawDemoScene(Canvas* canvas, DemoScene scene, int width, int height);

}  // namespace skity::demo
