#include "vulkan/vulkan_render_backend.hpp"

#include <vulkan/vulkan.h>

#include <memory>

#include <skity/gpu/gpu_context_vk.hpp>
#include <skity/gpu/gpu_presenter.hpp>
#include <skity/render/canvas.hpp>

#include "common/demo_scene.hpp"
#include "common/demo_scene_renderer.hpp"
#include "vulkan/vulkan_probe.hpp"

namespace skity::demo {

namespace {
#if defined(SKITY_DEV_ALLOW_VULKAN_VALIDATION) && \
    SKITY_DEV_ALLOW_VULKAN_VALIDATION
constexpr bool kValidationRuntimeAvailable = true;
#else
constexpr bool kValidationRuntimeAvailable = false;
#endif

bool ResolveValidationEnabled(bool requested) {
  return kValidationRuntimeAvailable && requested;
}

constexpr int kPresentModeFifo = 0;
constexpr int kPresentModeMailbox = 1;
constexpr int kPresentModeImmediate = 2;
constexpr uint32_t kDefaultMinImageCount = 2u;

VkPresentModeKHR ResolvePresentMode(int present_mode) {
  switch (present_mode) {
    case kPresentModeMailbox:
      return VK_PRESENT_MODE_MAILBOX_KHR;
    case kPresentModeImmediate:
      return VK_PRESENT_MODE_IMMEDIATE_KHR;
    case kPresentModeFifo:
    default:
      return VK_PRESENT_MODE_FIFO_KHR;
  }
}

uint32_t ResolveMinImageCount(int min_image_count) {
  return min_image_count < static_cast<int>(kDefaultMinImageCount)
             ? kDefaultMinImageCount
             : static_cast<uint32_t>(min_image_count);
}

}  // namespace

std::unique_ptr<RenderBackend> CreateVulkanRenderBackend(bool enable_validation,
                                                         int present_mode,
                                                         int min_image_count) {
  return std::make_unique<VulkanRenderBackend>(enable_validation, present_mode,
                                               min_image_count);
}

VulkanRenderBackend::VulkanRenderBackend(bool enable_validation,
                                         int present_mode,
                                         int min_image_count)
    : validation_enabled_(ResolveValidationEnabled(enable_validation)),
      present_mode_(ResolvePresentMode(present_mode)),
      min_image_count_(ResolveMinImageCount(min_image_count)) {
  diagnostics_.SetBackendName("Vulkan");
  diagnostics_.SetSurfaceName("Swapchain");
  diagnostics_.SetPresentModeRequest(static_cast<int32_t>(present_mode_));
  diagnostics_.SetValidationEnabled(validation_enabled_);
  diagnostics_.SetMsaaSampleCount(1);
}

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
  diagnostics_.SetSurfaceSize(0, 0);
}

void VulkanRenderBackend::OnSurfaceChanged(int width, int height) {
  width_ = static_cast<uint32_t>(width);
  height_ = static_cast<uint32_t>(height);
  diagnostics_.SetSurfaceSize(width, height);

  if (native_window_ != nullptr) {
    native_window_->Resize(width_, height_);
    return;
  }

  EnsureNativeWindow();
}

void VulkanRenderBackend::SetScene(int scene) {
  scene_.store(scene);
}

void VulkanRenderBackend::SetMsaaSampleCount(int sample_count) {
  const int normalized = sample_count <= 1 ? 1 : sample_count;
  sample_count_.store(normalized);
  diagnostics_.SetMsaaSampleCount(normalized);
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
  acquire_desc.sample_count = static_cast<uint32_t>(sample_count_.load());
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
                DemoBackend::kVulkan, validation_enabled_,
                static_cast<int>(width_), static_cast<int>(height_));
  canvas->Flush();
  surface->Flush();
  diagnostics_.RecordFrame();

  const auto present_result = presenter->Present(std::move(surface));
  if (present_result == skity::GPUPresenterStatus::kNeedRecreate) {
    native_window_->Resize(width_, height_);
  }
}

bool VulkanRenderBackend::EnsureContext() {
  if (context_ != nullptr) {
    return true;
  }

  skity::GPUContextInfoVK context_info = {};
  context_info.get_instance_proc_addr = vkGetInstanceProcAddr;
  context_info.enable_debug_runtime = validation_enabled_;
  context_ = skity::CreateGPUContextVK(&context_info);
  diagnostics_.SetContextReady(context_ != nullptr);
  UpdateProbeInfo();
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
  info.present_mode = present_mode_;
  info.min_image_count = min_image_count_;

  native_window_ = skity::CreateGPUNativeWindowVK(context_.get(), &info);
  if (native_window_ != nullptr && native_window_->GetPresenter() != nullptr) {
    diagnostics_.SetSurfaceImageCount(
        native_window_->GetPresenter()->GetImageCount());
    diagnostics_.SetPresentModeActual(
        native_window_->GetPresenter()->GetPresentMode());
  } else {
    diagnostics_.SetSurfaceImageCount(0);
    diagnostics_.SetPresentModeActual(0);
  }
  return native_window_ != nullptr;
}

void VulkanRenderBackend::ResetNativeWindow() {
  native_window_.reset();
  diagnostics_.SetSurfaceImageCount(0);
  diagnostics_.SetPresentModeActual(0);
  if (native_window_handle_ != nullptr) {
    ANativeWindow_release(native_window_handle_);
    native_window_handle_ = nullptr;
  }
}

void VulkanRenderBackend::UpdateProbeInfo() {
  if (probe_info_loaded_) {
    return;
  }

  const auto probe_info = ProbeVulkanDeviceInfo();
  diagnostics_.SetGpuInfo(probe_info.vendor_name.c_str(),
                          probe_info.renderer_name.c_str(),
                          probe_info.version_name.c_str());
  diagnostics_.SetMemoryInfo(probe_info.total_memory_bytes,
                             probe_info.device_local_memory_bytes);
  probe_info_loaded_ = true;
}

std::string VulkanRenderBackend::GetOverlayText() const {
  return diagnostics_.BuildOverlayText();
}

}  // namespace skity::demo
