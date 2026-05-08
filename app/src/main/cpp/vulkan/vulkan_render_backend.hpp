#pragma once

#include <android/native_window.h>
#include <vulkan/vulkan.h>

#include <atomic>
#include <memory>

#include "common/renderer_diagnostics.hpp"
#include "common/render_backend.hpp"

namespace skity {
class GPUContext;
class GPUNativeWindowVK;
}

namespace skity::demo {

std::unique_ptr<RenderBackend> CreateVulkanRenderBackend(
    bool enable_validation, int present_mode, int min_image_count);

class VulkanRenderBackend final : public RenderBackend {
 public:
  VulkanRenderBackend(bool enable_validation, int present_mode,
                      int min_image_count);
  ~VulkanRenderBackend() override;

  void SetNativeWindow(ANativeWindow* native_window) override;
  void OnSurfaceCreated() override;
  void OnSurfaceDestroyed() override;
  void OnSurfaceChanged(int width, int height) override;
  void SetScene(int scene) override;
  void SetMsaaSampleCount(int sample_count) override;
  void DrawFrame() override;
  std::string GetOverlayText() const override;

 private:
  bool EnsureContext();
  bool EnsureNativeWindow();
  void ResetNativeWindow();
  void UpdateProbeInfo();

  std::unique_ptr<skity::GPUContext> context_;
  std::unique_ptr<skity::GPUNativeWindowVK> native_window_;
  ANativeWindow* native_window_handle_ = nullptr;
  RendererDiagnostics diagnostics_;
  std::atomic<int> scene_{1};
  std::atomic<int> sample_count_{1};
  uint32_t width_ = 0;
  uint32_t height_ = 0;
  bool probe_info_loaded_ = false;
  bool validation_enabled_ = false;
  VkPresentModeKHR present_mode_ = VK_PRESENT_MODE_FIFO_KHR;
  uint32_t min_image_count_ = 2;
};

}  // namespace skity::demo
