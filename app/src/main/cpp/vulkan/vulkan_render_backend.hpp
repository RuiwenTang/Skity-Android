#pragma once

#include <android/native_window.h>

#include <atomic>
#include <memory>

#include "common/render_backend.hpp"

namespace skity {
class GPUContext;
class GPUNativeWindowVK;
}

namespace skity::demo {

std::unique_ptr<RenderBackend> CreateVulkanRenderBackend();

class VulkanRenderBackend final : public RenderBackend {
 public:
  VulkanRenderBackend();
  ~VulkanRenderBackend() override;

  void SetNativeWindow(ANativeWindow* native_window) override;
  void OnSurfaceCreated() override;
  void OnSurfaceDestroyed() override;
  void OnSurfaceChanged(int width, int height) override;
  void SetScene(int scene) override;
  void DrawFrame() override;

 private:
  bool EnsureContext();
  bool EnsureNativeWindow();
  void ResetNativeWindow();

  std::unique_ptr<skity::GPUContext> context_;
  std::unique_ptr<skity::GPUNativeWindowVK> native_window_;
  ANativeWindow* native_window_handle_ = nullptr;
  std::atomic<int> scene_{1};
  uint32_t width_ = 0;
  uint32_t height_ = 0;
};

}  // namespace skity::demo
