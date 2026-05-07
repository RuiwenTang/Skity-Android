#pragma once

#include <android/native_window.h>
#include <memory>

namespace skity::demo {

class RenderBackend;

class AppRenderer {
 public:
  explicit AppRenderer(int backend_type);
  ~AppRenderer();

  void SetNativeWindow(ANativeWindow* native_window);
  void OnSurfaceCreated();
  void OnSurfaceDestroyed();
  void OnSurfaceChanged(int width, int height);
  void SetScene(int scene);
  void DrawFrame();

 private:
  std::unique_ptr<RenderBackend> backend_;
};

}  // namespace skity::demo
