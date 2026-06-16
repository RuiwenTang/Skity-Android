#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <android/hardware_buffer.h>
#include <android/native_window.h>
#include <vulkan/vulkan.h>

#include <atomic>
#include <memory>

#include "common/renderer_diagnostics.hpp"
#include "common/render_backend.hpp"

namespace skity {
class GPUContext;
class GPUNativeWindowVK;
class GPUSemaphore;
class Image;
}  // namespace skity

namespace skity::demo {

std::unique_ptr<RenderBackend> CreateTextureShareBackend(
    bool enable_validation, int present_mode, int min_image_count);

class TextureShareBackend final : public RenderBackend {
 public:
  TextureShareBackend(bool enable_validation, int present_mode,
                      int min_image_count);
  ~TextureShareBackend() override;

  void SetNativeWindow(ANativeWindow* native_window) override;
  void OnSurfaceCreated() override;
  void OnSurfaceDestroyed() override;
  void OnSurfaceChanged(int width, int height) override;
  void SetScene(int scene) override;
  void SetMsaaSampleCount(int sample_count) override;
  void DrawFrame() override;
  std::string GetOverlayText() const override;

 private:
  // Vulkan side
  bool EnsureVulkanContext();
  bool EnsureNativeWindow();
  void ResetNativeWindow();

  // EGL / GL offscreen side
  bool InitEGL();
  void DestroyEGL();

  // AHardwareBuffer + GL resources
  bool AllocateAHardwareBuffer();
  void ReleaseAHardwareBuffer();
  bool InitGLResources();
  void DestroyGLResources();

  // Rendering
  void RenderGLFrame();

  // Import AHB as Vulkan texture (cached, only creates once)
  bool EnsureImportedImage();

  // --- Vulkan state ---
  std::unique_ptr<skity::GPUContext> vk_context_;
  std::unique_ptr<skity::GPUNativeWindowVK> vk_native_window_;
  ANativeWindow* native_window_handle_ = nullptr;
  uint32_t width_ = 0;
  uint32_t height_ = 0;

  // --- EGL / GL state ---
  EGLDisplay egl_display_ = EGL_NO_DISPLAY;
  EGLContext egl_context_ = EGL_NO_CONTEXT;
  EGLSurface egl_pbuffer_surface_ = EGL_NO_SURFACE;
  GLuint gl_program_ = 0;
  GLuint gl_vbo_ = 0;
  GLuint gl_texture_ = 0;
  GLuint gl_fbo_ = 0;
  EGLImageKHR egl_image_ = EGL_NO_IMAGE_KHR;

  // --- AHardwareBuffer ---
  AHardwareBuffer* ahb_ = nullptr;

  // --- Cached imported Vulkan texture ---
  std::shared_ptr<skity::Image> imported_image_;

  // --- GPU-GPU sync semaphore (GL fence → VkSemaphore) ---
  std::shared_ptr<skity::GPUSemaphore> gpu_semaphore_;

  // --- State ---
  RendererDiagnostics diagnostics_;
  std::atomic<int> sample_count_{1};
  uint64_t frame_count_ = 0;
  bool vk_context_ready_ = false;
  bool egl_initialized_ = false;
  bool gl_resources_ready_ = false;
  bool validation_enabled_ = false;
  VkPresentModeKHR present_mode_ = VK_PRESENT_MODE_FIFO_KHR;
  uint32_t min_image_count_ = 2;
};

}  // namespace skity::demo
