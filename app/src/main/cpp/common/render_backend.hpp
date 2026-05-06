#pragma once

namespace skity::demo {

class RenderBackend {
 public:
  virtual ~RenderBackend() = default;

  virtual void OnSurfaceCreated() = 0;
  virtual void OnSurfaceChanged(int width, int height) = 0;
  virtual void SetScene(int scene) = 0;
  virtual void DrawFrame() = 0;
};

}  // namespace skity::demo
