#include "vulkan/vulkan_render_backend.hpp"

#include <vulkan/vulkan.h>

#include <memory>

#include <skity/gpu/gpu_context_vk.hpp>
#include <skity/gpu/gpu_presenter.hpp>
#include <skity/render/canvas.hpp>

#include "common/demo_scene.hpp"
#include "common/demo_scene_renderer.hpp"

namespace skity::demo {

namespace {
#if defined(SKITY_DEV_ENABLE_VULKAN_VALIDATION) && \
    SKITY_DEV_ENABLE_VULKAN_VALIDATION
constexpr bool kValidationEnabled = true;
#else
constexpr bool kValidationEnabled = false;
#endif
}  // namespace

std::unique_ptr<RenderBackend> CreateVulkanRenderBackend() {
  return std::make_unique<VulkanRenderBackend>();
}

VulkanRenderBackend::VulkanRenderBackend() = default;

VulkanRenderBackend::~VulkanRenderBackend() {
  ResetNativeWindow();
}

void VulkanRenderBackend::SetNativeWindow(ANativeWindow* native_window) {
  if (native_window_handle_ == native_window) {
    return;
  }

  ResetNativeWindow();
  native_window_handle_ = native_window;
  EnsureNativeWindow();
}

void VulkanRenderBackend::OnSurfaceCreated() {
  EnsureContext();
  EnsureNativeWindow();
}

void VulkanRenderBackend::OnSurfaceDestroyed() {
  ResetNativeWindow();
}

void VulkanRenderBackend::OnSurfaceChanged(int width, int height) {
  width_ = static_cast<uint32_t>(width);
  height_ = static_cast<uint32_t>(height);

  if (native_window_ != nullptr) {
    native_window_->Resize(width_, height_);
    return;
  }

  EnsureNativeWindow();
}

void VulkanRenderBackend::SetScene(int scene) {
  scene_.store(scene);
}

void VulkanRenderBackend::DrawFrame() {
  if (!EnsureNativeWindow()) {
    return;
  }

  auto* presenter = native_window_->GetPresenter();
  if (presenter == nullptr) {
    return;
  }

  skity::GPUSurfaceAcquireDescriptor acquire_desc = {};
  acquire_desc.sample_count = 1;
  acquire_desc.content_scale = 1.f;

  auto acquire_result = presenter->AcquireNextSurface(acquire_desc);
  if (acquire_result.status == skity::GPUPresenterStatus::kNeedRecreate) {
    native_window_->Resize(width_, height_);
    return;
  }
  if (acquire_result.status != skity::GPUPresenterStatus::kSuccess ||
      acquire_result.surface == nullptr) {
    return;
  }

  auto surface = std::move(acquire_result.surface);
  auto* canvas = surface->LockCanvas(true);
  if (canvas == nullptr) {
    return;
  }

  DrawDemoScene(canvas, static_cast<DemoScene>(scene_.load()),
                DemoBackend::kVulkan, kValidationEnabled,
                static_cast<int>(width_), static_cast<int>(height_));
  canvas->Flush();
  surface->Flush();

  const auto present_result = presenter->Present(std::move(surface));
  if (present_result == skity::GPUPresenterStatus::kNeedRecreate) {
    native_window_->Resize(width_, height_);
  }
}

bool VulkanRenderBackend::EnsureContext() {
  if (context_ != nullptr) {
    return true;
  }

#if defined(SKITY_DEV_ENABLE_VULKAN_VALIDATION) && \
    SKITY_DEV_ENABLE_VULKAN_VALIDATION
  skity::GPUContextInfoVK context_info = {};
  context_info.get_instance_proc_addr = vkGetInstanceProcAddr;
  context_info.enable_debug_runtime = true;
  context_ = skity::CreateGPUContextVK(&context_info);
#else
  context_ = skity::CreateGPUContextVK(vkGetInstanceProcAddr);
#endif
  return context_ != nullptr;
}

bool VulkanRenderBackend::EnsureNativeWindow() {
  if (native_window_ != nullptr) {
    return true;
  }

  if (native_window_handle_ == nullptr || width_ == 0 || height_ == 0 ||
      !EnsureContext()) {
    return false;
  }

  skity::GPUNativeWindowInfoVK info = {};
  info.native_window.type = skity::VKNativeWindowType::kAndroid;
  info.native_window.handle = native_window_handle_;
  info.width = width_;
  info.height = height_;
  info.present_mode = VK_PRESENT_MODE_FIFO_KHR;

  native_window_ = skity::CreateGPUNativeWindowVK(context_.get(), &info);
  return native_window_ != nullptr;
}

void VulkanRenderBackend::ResetNativeWindow() {
  native_window_.reset();
  if (native_window_handle_ != nullptr) {
    ANativeWindow_release(native_window_handle_);
    native_window_handle_ = nullptr;
  }
}

}  // namespace skity::demo
