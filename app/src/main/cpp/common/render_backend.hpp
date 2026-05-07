#pragma once

#include <android/native_window.h>

namespace skity::demo {

class RenderBackend {
 public:
  virtual ~RenderBackend() = default;

  virtual void SetNativeWindow(ANativeWindow* native_window) = 0;
  virtual void OnSurfaceCreated() = 0;
  virtual void OnSurfaceDestroyed() = 0;
  virtual void OnSurfaceChanged(int width, int height) = 0;
  virtual void SetScene(int scene) = 0;
  virtual void DrawFrame() = 0;
};

}  // namespace skity::demo
