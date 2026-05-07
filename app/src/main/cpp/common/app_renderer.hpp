#pragma once

#include <android/native_window.h>
#include <memory>
#include <string>

namespace skity::demo {

class RenderBackend;

class AppRenderer {
 public:
  AppRenderer(int backend_type, bool enable_vulkan_validation,
              int vulkan_present_mode);
  ~AppRenderer();

  void SetNativeWindow(ANativeWindow* native_window);
  void OnSurfaceCreated();
  void OnSurfaceDestroyed();
  void OnSurfaceChanged(int width, int height);
  void SetScene(int scene);
  void SetMsaaSampleCount(int sample_count);
  void DrawFrame();
  std::string GetOverlayText() const;

 private:
  std::unique_ptr<RenderBackend> backend_;
};

}  // namespace skity::demo
