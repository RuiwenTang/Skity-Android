#include "common/app_renderer.hpp"

#include "common/render_backend.hpp"
#include "gles/gles_render_backend.hpp"
#include "vulkan/vulkan_render_backend.hpp"

namespace skity::demo {

namespace {

constexpr int kBackendAuto = 0;
constexpr int kBackendGles = 1;
constexpr int kBackendVulkan = 2;

std::unique_ptr<RenderBackend> CreateRenderBackend(
    int backend_type,
    bool enable_vulkan_validation,
    int vulkan_present_mode) {
  switch (backend_type) {
    case kBackendVulkan:
      return CreateVulkanRenderBackend(enable_vulkan_validation,
                                       vulkan_present_mode);
    case kBackendAuto:
    case kBackendGles:
    default:
      return CreateGlesRenderBackend();
  }
}

}  // namespace

AppRenderer::AppRenderer(int backend_type, bool enable_vulkan_validation,
                         int vulkan_present_mode)
    : backend_(CreateRenderBackend(backend_type, enable_vulkan_validation,
                                   vulkan_present_mode)) {}

AppRenderer::~AppRenderer() = default;

void AppRenderer::SetNativeWindow(ANativeWindow* native_window) {
  backend_->SetNativeWindow(native_window);
}

void AppRenderer::OnSurfaceCreated() {
  backend_->OnSurfaceCreated();
}

void AppRenderer::OnSurfaceDestroyed() {
  backend_->OnSurfaceDestroyed();
}

void AppRenderer::OnSurfaceChanged(int width, int height) {
  backend_->OnSurfaceChanged(width, height);
}

void AppRenderer::SetScene(int scene) {
  backend_->SetScene(scene);
}

void AppRenderer::SetMsaaSampleCount(int sample_count) {
  backend_->SetMsaaSampleCount(sample_count);
}

void AppRenderer::DrawFrame() {
  backend_->DrawFrame();
}

std::string AppRenderer::GetOverlayText() const {
  return backend_->GetOverlayText();
}

}  // namespace skity::demo
