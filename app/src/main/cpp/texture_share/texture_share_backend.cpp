#include "texture_share/texture_share_backend.hpp"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <android/hardware_buffer.h>
#include <android/log.h>
#include <vulkan/vulkan.h>

#include <cmath>
#include <memory>
#include <unistd.h>

#include <skity/gpu/gpu_context.hpp>
#include <skity/gpu/gpu_context_vk.hpp>
#include <skity/gpu/gpu_presenter.hpp>
#include <skity/gpu/gpu_semaphore.hpp>
#include <skity/gpu/gpu_surface.hpp>
#include <skity/graphic/image.hpp>
#include <skity/graphic/paint.hpp>
#include <skity/graphic/sampling_options.hpp>
#include <skity/render/canvas.hpp>

#define LOG_TAG "SkityTextureShare"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

namespace skity::demo {

namespace {

// EGL extension function pointers (loaded via eglGetProcAddress)
using PFN_eglGetNativeClientBufferANDROID = EGLClientBuffer (*)(const AHardwareBuffer*);
using PFN_eglCreateImageKHR = EGLImageKHR (*)(EGLDisplay, EGLContext, EGLenum, EGLClientBuffer, const EGLint*);
using PFN_eglDestroyImageKHR = EGLBoolean (*)(EGLDisplay, EGLImageKHR);
using PFN_glEGLImageTargetTexture2DOES = void (*)(GLenum, void*);

// Fence sync functions for GPU-GPU synchronization
using PFN_eglCreateSyncKHR = EGLSyncKHR (*)(EGLDisplay, EGLenum, const EGLint*);
using PFN_eglDestroySyncKHR = EGLBoolean (*)(EGLDisplay, EGLSyncKHR);
using PFN_eglDupNativeFenceFDANDROID = EGLint (*)(EGLDisplay, EGLSyncKHR);

PFN_eglGetNativeClientBufferANDROID g_eglGetNativeClientBufferANDROID = nullptr;
PFN_eglCreateImageKHR g_eglCreateImageKHR = nullptr;
PFN_eglDestroyImageKHR g_eglDestroyImageKHR = nullptr;
PFN_glEGLImageTargetTexture2DOES g_glEGLImageTargetTexture2DOES = nullptr;
PFN_eglCreateSyncKHR g_eglCreateSyncKHR = nullptr;
PFN_eglDestroySyncKHR g_eglDestroySyncKHR = nullptr;
PFN_eglDupNativeFenceFDANDROID g_eglDupNativeFenceFDANDROID = nullptr;

bool LoadEGLEXTFunctions() {
  g_eglGetNativeClientBufferANDROID =
      reinterpret_cast<PFN_eglGetNativeClientBufferANDROID>(
          eglGetProcAddress("eglGetNativeClientBufferANDROID"));
  g_eglCreateImageKHR =
      reinterpret_cast<PFN_eglCreateImageKHR>(
          eglGetProcAddress("eglCreateImageKHR"));
  g_eglDestroyImageKHR =
      reinterpret_cast<PFN_eglDestroyImageKHR>(
          eglGetProcAddress("eglDestroyImageKHR"));
  g_glEGLImageTargetTexture2DOES =
      reinterpret_cast<PFN_glEGLImageTargetTexture2DOES>(
          eglGetProcAddress("glEGLImageTargetTexture2DOES"));

  // Fence sync functions (optional, fall back to glFinish if unavailable)
  g_eglCreateSyncKHR =
      reinterpret_cast<PFN_eglCreateSyncKHR>(
          eglGetProcAddress("eglCreateSyncKHR"));
  g_eglDestroySyncKHR =
      reinterpret_cast<PFN_eglDestroySyncKHR>(
          eglGetProcAddress("eglDestroySyncKHR"));
  g_eglDupNativeFenceFDANDROID =
      reinterpret_cast<PFN_eglDupNativeFenceFDANDROID>(
          eglGetProcAddress("eglDupNativeFenceFDANDROID"));

  if (g_eglGetNativeClientBufferANDROID == nullptr ||
      g_eglCreateImageKHR == nullptr ||
      g_eglDestroyImageKHR == nullptr ||
      g_glEGLImageTargetTexture2DOES == nullptr) {
    LOGE("Failed to load required EGL extension functions: "
         "getNativeClientBuffer=%p, createImage=%p, destroyImage=%p, "
         "imageTargetTexture2D=%p",
         g_eglGetNativeClientBufferANDROID, g_eglCreateImageKHR,
         g_eglDestroyImageKHR, g_glEGLImageTargetTexture2DOES);
    return false;
  }

  if (g_eglCreateSyncKHR == nullptr || g_eglDestroySyncKHR == nullptr ||
      g_eglDupNativeFenceFDANDROID == nullptr) {
    LOGW("Fence sync functions not available, will use glFinish() as fallback");
  }
  return true;
}

}  // namespace

namespace {

constexpr int kPresentModeFifo = 0;
constexpr int kPresentModeMailbox = 1;
constexpr int kPresentModeImmediate = 2;

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

}  // namespace

std::unique_ptr<RenderBackend> CreateTextureShareBackend(
    bool enable_validation, int present_mode, int min_image_count) {
  return std::make_unique<TextureShareBackend>(enable_validation, present_mode,
                                               min_image_count);
}

// clang-format off
static const char* kVertexShader = R"(
#version 300 es
precision highp float;
layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec3 aColor;
uniform float uAngle;
out vec3 vColor;
void main() {
    float c = cos(uAngle);
    float s = sin(uAngle);
    mat2 rot = mat2(c, -s, s, c);
    vec2 pos = rot * aPosition;
    gl_Position = vec4(pos, 0.0, 1.0);
    vColor = aColor;
}
)";

static const char* kFragmentShader = R"(
#version 300 es
precision highp float;
in vec3 vColor;
out vec4 fragColor;
void main() {
    fragColor = vec4(vColor, 1.0);
}
)";
// clang-format on

// Triangle vertices: position (x, y) + color (r, g, b)
static constexpr float kTriangleVertices[] = {
    // position      // color
    0.0f,  0.7f,     1.0f, 0.2f, 0.2f,   // top - red
   -0.7f, -0.5f,     0.2f, 1.0f, 0.2f,   // bottom-left - green
    0.7f, -0.5f,     0.2f, 0.2f, 1.0f,   // bottom-right - blue
};

static constexpr GLuint kAttribPosition = 0;
static constexpr GLuint kAttribColor = 1;

TextureShareBackend::TextureShareBackend(bool enable_validation,
                                         int present_mode,
                                         int min_image_count)
    : validation_enabled_(enable_validation),
      present_mode_(ResolvePresentMode(present_mode)),
      min_image_count_(static_cast<uint32_t>(
          min_image_count < 2 ? 2 : min_image_count)) {
  diagnostics_.SetBackendName("GL-VK Share");
  diagnostics_.SetSurfaceName("Swapchain");
  diagnostics_.SetValidationEnabled(validation_enabled_);
  diagnostics_.SetMsaaSampleCount(1);
}

TextureShareBackend::~TextureShareBackend() {
  DestroyGLResources();
  ReleaseAHardwareBuffer();
  DestroyEGL();
  ResetNativeWindow();
}

// ---------------------------------------------------------------------------
// RenderBackend interface
// ---------------------------------------------------------------------------

void TextureShareBackend::SetNativeWindow(ANativeWindow* native_window) {
  if (native_window_handle_ == native_window) {
    return;
  }
  ResetNativeWindow();
  native_window_handle_ = native_window;
  EnsureNativeWindow();
}

void TextureShareBackend::OnSurfaceCreated() {
  EnsureVulkanContext();
  EnsureNativeWindow();
}

void TextureShareBackend::OnSurfaceDestroyed() {
  ResetNativeWindow();
  diagnostics_.SetSurfaceSize(0, 0);
}

void TextureShareBackend::OnSurfaceChanged(int width, int height) {
  uint32_t w = static_cast<uint32_t>(width);
  uint32_t h = static_cast<uint32_t>(height);

  bool size_changed = (w != width_ || h != height_);
  width_ = w;
  height_ = h;
  diagnostics_.SetSurfaceSize(width, height);

  if (size_changed) {
    DestroyGLResources();
    ReleaseAHardwareBuffer();
  }

  if (vk_native_window_ != nullptr) {
    vk_native_window_->Resize(width_, height_);
    return;
  }

  if (!InitEGL()) {
    LOGE("Failed to initialize EGL");
    return;
  }
  if (!AllocateAHardwareBuffer()) {
    LOGE("Failed to allocate AHardwareBuffer");
    return;
  }
  if (!InitGLResources()) {
    LOGE("Failed to initialize GL resources");
    return;
  }

  EnsureNativeWindow();
}

void TextureShareBackend::SetScene(int scene) {
  // Texture share demo has a fixed scene, ignore.
  (void)scene;
}

void TextureShareBackend::SetMsaaSampleCount(int sample_count) {
  const int normalized = sample_count <= 1 ? 1 : sample_count;
  sample_count_.store(normalized);
  diagnostics_.SetMsaaSampleCount(normalized);
}

void TextureShareBackend::DrawFrame() {
  if (!EnsureNativeWindow()) {
    return;
  }

  auto* presenter = vk_native_window_->GetPresenter();
  if (presenter == nullptr) {
    return;
  }

  // 1. Render GL content into AHardwareBuffer (no glFinish — GPU-GPU sync below)
  if (gl_resources_ready_) {
    RenderGLFrame();
  }

  // 2. Create EGLSync fence after GL rendering and import as VkSemaphore
  if (gl_resources_ready_ && gpu_semaphore_ != nullptr &&
      g_eglCreateSyncKHR != nullptr && g_eglDupNativeFenceFDANDROID != nullptr) {
    EGLSyncKHR fence = g_eglCreateSyncKHR(egl_display_, EGL_SYNC_NATIVE_FENCE_ANDROID,
                                           nullptr);
    if (fence != EGL_NO_SYNC_KHR) {
      // Flush GL commands so the fence is submitted
      glFlush();

      int fd = g_eglDupNativeFenceFDANDROID(egl_display_, fence);
      // EGLSync can be destroyed immediately after duping the fd
      g_eglDestroySyncKHR(egl_display_, fence);

      if (fd >= 0) {
        skity::GPUSemaphoreImportInfoVK import_info = {};
        import_info.sync_fd = fd;  // ownership transferred to Vulkan driver
        vk_context_->ImportSemaphore(gpu_semaphore_.get(), import_info);
      } else {
        // fd dup failed, fall back to glFinish for correctness
        LOGW("eglDupNativeFenceFDANDROID returned invalid fd, using glFinish");
        glFinish();
      }
    } else {
      // Fence creation failed, fall back to glFinish
      LOGW("eglCreateSyncKHR failed, using glFinish");
      glFinish();
    }
  } else if (gl_resources_ready_) {
    // No semaphore or fence functions available, use CPU-GPU sync fallback
    glFinish();
  }

  // 3. Acquire next swapchain surface
  skity::GPUSurfaceAcquireDescriptor acquire_desc = {};
  acquire_desc.sample_count = static_cast<uint32_t>(sample_count_.load());
  acquire_desc.content_scale = 1.f;

  auto acquire_result = presenter->AcquireNextSurface(acquire_desc);
  if (acquire_result.status == skity::GPUPresenterStatus::kNeedRecreate) {
    vk_native_window_->Resize(width_, height_);
    return;
  }
  if (acquire_result.status != skity::GPUPresenterStatus::kSuccess ||
      acquire_result.surface == nullptr) {
    return;
  }

  auto surface = std::move(acquire_result.surface);

  // 4. Inject external wait semaphore (GL fence → Vulkan wait)
  if (gpu_semaphore_ != nullptr) {
    surface->AddExternalWaitSemaphore(gpu_semaphore_);
  }

  auto* canvas = surface->LockCanvas(true);
  if (canvas == nullptr) {
    return;
  }

  // 5. Draw the cached imported AHB texture
  if (EnsureImportedImage()) {
    skity::Paint paint;
    paint.SetAntiAlias(true);
    skity::Rect src =
        skity::Rect::MakeLTRB(0, 0, static_cast<float>(width_),
                              static_cast<float>(height_));
    skity::Rect dst = src;
    skity::SamplingOptions sampling(skity::FilterMode::kLinear,
                                    skity::MipmapMode::kNone);
    canvas->DrawImageRect(imported_image_, src, dst, sampling, &paint);
  }

  // 6. Draw overlay label
  {
    skity::Paint label_paint;
    label_paint.SetAntiAlias(true);
    label_paint.SetColor(0xFFFFFFFF);
    canvas->DrawSimpleText("GL -> AHB -> Vulkan", 16.f,
                           static_cast<float>(height_) - 24.f, label_paint);
  }

  canvas->Flush();
  surface->Flush();
  diagnostics_.RecordFrame();

  const auto present_result = presenter->Present(std::move(surface));
  if (present_result == skity::GPUPresenterStatus::kNeedRecreate) {
    vk_native_window_->Resize(width_, height_);
  }
}

std::string TextureShareBackend::GetOverlayText() const {
  return diagnostics_.BuildOverlayText();
}

// ---------------------------------------------------------------------------
// Vulkan context + native window (same pattern as VulkanRenderBackend)
// ---------------------------------------------------------------------------

bool TextureShareBackend::EnsureVulkanContext() {
  if (vk_context_ready_) {
    return true;
  }

  skity::GPUContextInfoVK context_info = {};
  context_info.get_instance_proc_addr = vkGetInstanceProcAddr;
  context_info.enable_debug_runtime = validation_enabled_;
  vk_context_ = skity::CreateGPUContextVK(&context_info);
  vk_context_ready_ = vk_context_ != nullptr;

  // Create a reusable GPU semaphore for GL→Vulkan GPU-GPU sync.
  // Created once and re-imported with a new fence fd each frame.
  if (vk_context_ready_ && gpu_semaphore_ == nullptr) {
    gpu_semaphore_ = vk_context_->CreateSemaphore();
    if (gpu_semaphore_ == nullptr) {
      LOGW("CreateSemaphore failed, will fall back to glFinish for sync");
    }
  }

  diagnostics_.SetContextReady(vk_context_ready_);
  return vk_context_ready_;
}

bool TextureShareBackend::EnsureNativeWindow() {
  if (vk_native_window_ != nullptr) {
    return true;
  }

  if (native_window_handle_ == nullptr || width_ == 0 || height_ == 0 ||
      !EnsureVulkanContext()) {
    return false;
  }

  skity::GPUNativeWindowInfoVK info = {};
  info.native_window.type = skity::VKNativeWindowType::kAndroid;
  info.native_window.handle = native_window_handle_;
  info.width = width_;
  info.height = height_;
  info.present_mode = present_mode_;
  info.min_image_count = min_image_count_;

  vk_native_window_ =
      skity::CreateGPUNativeWindowVK(vk_context_.get(), &info);
  if (vk_native_window_ != nullptr &&
      vk_native_window_->GetPresenter() != nullptr) {
    diagnostics_.SetSurfaceImageCount(2);
    diagnostics_.SetPresentModeActual(
        vk_native_window_->GetPresenter()->GetPresentMode());
  } else {
    diagnostics_.SetSurfaceImageCount(0);
    diagnostics_.SetPresentModeActual(0);
  }
  return vk_native_window_ != nullptr;
}

void TextureShareBackend::ResetNativeWindow() {
  vk_native_window_.reset();
  diagnostics_.SetSurfaceImageCount(0);
  diagnostics_.SetPresentModeActual(0);
  if (native_window_handle_ != nullptr) {
    ANativeWindow_release(native_window_handle_);
    native_window_handle_ = nullptr;
  }
}

// ---------------------------------------------------------------------------
// EGL context (offscreen)
// ---------------------------------------------------------------------------

bool TextureShareBackend::InitEGL() {
  if (egl_initialized_) {
    return true;
  }

  // 1. Get display
  egl_display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (egl_display_ == EGL_NO_DISPLAY) {
    LOGE("eglGetDisplay failed");
    return false;
  }

  // 2. Initialize
  EGLint major = 0, minor = 0;
  if (!eglInitialize(egl_display_, &major, &minor)) {
    LOGE("eglInitialize failed: 0x%x", eglGetError());
    egl_display_ = EGL_NO_DISPLAY;
    return false;
  }
  LOGI("EGL initialized: version %d.%d", major, minor);

  // 3. Choose config (ES 3.0, RGBA8888, pbuffer)
  EGLint config_attribs[] = {
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_RED_SIZE,   8,
      EGL_GREEN_SIZE,      8,                   EGL_BLUE_SIZE,  8,
      EGL_ALPHA_SIZE,      8,                   EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
      EGL_NONE};
  EGLConfig config = nullptr;
  EGLint num_configs = 0;
  if (!eglChooseConfig(egl_display_, config_attribs, &config, 1,
                       &num_configs) ||
      num_configs == 0) {
    LOGE("eglChooseConfig failed: 0x%x", eglGetError());
    eglTerminate(egl_display_);
    egl_display_ = EGL_NO_DISPLAY;
    return false;
  }

  // 4. Create context
  EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
  egl_context_ =
      eglCreateContext(egl_display_, config, EGL_NO_CONTEXT, context_attribs);
  if (egl_context_ == EGL_NO_CONTEXT) {
    LOGE("eglCreateContext failed: 0x%x", eglGetError());
    eglTerminate(egl_display_);
    egl_display_ = EGL_NO_DISPLAY;
    return false;
  }

  // 5. Create 1x1 pbuffer (dummy surface for makeCurrent)
  EGLint pbuffer_attribs[] = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};
  egl_pbuffer_surface_ =
      eglCreatePbufferSurface(egl_display_, config, pbuffer_attribs);
  if (egl_pbuffer_surface_ == EGL_NO_SURFACE) {
    LOGE("eglCreatePbufferSurface failed: 0x%x", eglGetError());
    eglDestroyContext(egl_display_, egl_context_);
    egl_context_ = EGL_NO_CONTEXT;
    eglTerminate(egl_display_);
    egl_display_ = EGL_NO_DISPLAY;
    return false;
  }

  // 6. Make current
  if (!eglMakeCurrent(egl_display_, egl_pbuffer_surface_,
                      egl_pbuffer_surface_, egl_context_)) {
    LOGE("eglMakeCurrent failed: 0x%x", eglGetError());
    eglDestroySurface(egl_display_, egl_pbuffer_surface_);
    egl_pbuffer_surface_ = EGL_NO_SURFACE;
    eglDestroyContext(egl_display_, egl_context_);
    egl_context_ = EGL_NO_CONTEXT;
    eglTerminate(egl_display_);
    egl_display_ = EGL_NO_DISPLAY;
    return false;
  }

  // Load EGL extension functions
  if (!LoadEGLEXTFunctions()) {
    LOGE("Failed to load EGL extension functions");
    eglMakeCurrent(egl_display_, EGL_NO_SURFACE, EGL_NO_SURFACE,
                   EGL_NO_CONTEXT);
    eglDestroySurface(egl_display_, egl_pbuffer_surface_);
    egl_pbuffer_surface_ = EGL_NO_SURFACE;
    eglDestroyContext(egl_display_, egl_context_);
    egl_context_ = EGL_NO_CONTEXT;
    eglTerminate(egl_display_);
    egl_display_ = EGL_NO_DISPLAY;
    return false;
  }

  egl_initialized_ = true;
  LOGI("Offscreen EGL context ready (GL_VENDOR: %s, GL_RENDERER: %s)",
       glGetString(GL_VENDOR), glGetString(GL_RENDERER));
  return true;
}

void TextureShareBackend::DestroyEGL() {
  if (!egl_initialized_) {
    return;
  }

  // Make sure no context is current before destroying
  if (egl_display_ != EGL_NO_DISPLAY) {
    eglMakeCurrent(egl_display_, EGL_NO_SURFACE, EGL_NO_SURFACE,
                   EGL_NO_CONTEXT);
    if (egl_pbuffer_surface_ != EGL_NO_SURFACE) {
      eglDestroySurface(egl_display_, egl_pbuffer_surface_);
      egl_pbuffer_surface_ = EGL_NO_SURFACE;
    }
    if (egl_context_ != EGL_NO_CONTEXT) {
      eglDestroyContext(egl_display_, egl_context_);
      egl_context_ = EGL_NO_CONTEXT;
    }
    eglTerminate(egl_display_);
    egl_display_ = EGL_NO_DISPLAY;
  }
  egl_initialized_ = false;
}

// ---------------------------------------------------------------------------
// AHardwareBuffer
// ---------------------------------------------------------------------------

bool TextureShareBackend::AllocateAHardwareBuffer() {
  if (width_ == 0 || height_ == 0) {
    return false;
  }
  if (ahb_ != nullptr) {
    return true;  // already allocated
  }

  AHardwareBuffer_Desc desc = {};
  desc.width = width_;
  desc.height = height_;
  desc.layers = 1;
  desc.format = AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM;
  desc.usage = AHARDWAREBUFFER_USAGE_GPU_COLOR_OUTPUT |
               AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE;

  int result = AHardwareBuffer_allocate(&desc, &ahb_);
  if (result != 0 || ahb_ == nullptr) {
    LOGE("AHardwareBuffer_allocate failed: %d, size=%ux%u", result, width_,
         height_);
    return false;
  }

  LOGI("AHardwareBuffer allocated: %ux%u", width_, height_);
  return true;
}

void TextureShareBackend::ReleaseAHardwareBuffer() {
  if (ahb_ != nullptr) {
    AHardwareBuffer_release(ahb_);
    ahb_ = nullptr;
  }
}

// ---------------------------------------------------------------------------
// GL resources (EGLImage + FBO + shader + VBO)
// ---------------------------------------------------------------------------

static GLuint CompileShader(GLenum type, const char* source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);
  GLint compiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    GLint info_len = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
    if (info_len > 0) {
      char* buf = new char[info_len];
      glGetShaderInfoLog(shader, info_len, nullptr, buf);
      LOGE("Shader compile error: %s", buf);
      delete[] buf;
    }
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}

bool TextureShareBackend::InitGLResources() {
  if (gl_resources_ready_) {
    return true;
  }
  if (!egl_initialized_ || ahb_ == nullptr) {
    return false;
  }

  // Make EGL context current
  if (!eglMakeCurrent(egl_display_, egl_pbuffer_surface_,
                      egl_pbuffer_surface_, egl_context_)) {
    LOGE("eglMakeCurrent failed for GL resource init: 0x%x", eglGetError());
    return false;
  }

  // 1. Create GL texture
  glGenTextures(1, &gl_texture_);
  glBindTexture(GL_TEXTURE_2D, gl_texture_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // 2. Create EGLImage from AHardwareBuffer
  EGLClientBuffer client_buf = g_eglGetNativeClientBufferANDROID(ahb_);
  if (client_buf == nullptr) {
    LOGE("eglGetNativeClientBufferANDROID failed");
    DestroyGLResources();
    return false;
  }

  EGLint image_attribs[] = {EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE};
  egl_image_ = g_eglCreateImageKHR(egl_display_, EGL_NO_CONTEXT,
                                   EGL_NATIVE_BUFFER_ANDROID, client_buf,
                                   image_attribs);
  if (egl_image_ == EGL_NO_IMAGE_KHR) {
    LOGE("eglCreateImageKHR failed: 0x%x", eglGetError());
    DestroyGLResources();
    return false;
  }

  // 3. Bind EGLImage to GL texture
  g_glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, egl_image_);
  glBindTexture(GL_TEXTURE_2D, 0);

  // 4. Create FBO
  glGenFramebuffers(1, &gl_fbo_);
  glBindFramebuffer(GL_FRAMEBUFFER, gl_fbo_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         gl_texture_, 0);
  GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fbo_status != GL_FRAMEBUFFER_COMPLETE) {
    LOGE("FBO incomplete: 0x%x", fbo_status);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    DestroyGLResources();
    return false;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // 5. Compile shaders
  GLuint vert = CompileShader(GL_VERTEX_SHADER, kVertexShader);
  GLuint frag = CompileShader(GL_FRAGMENT_SHADER, kFragmentShader);
  if (vert == 0 || frag == 0) {
    if (vert) glDeleteShader(vert);
    if (frag) glDeleteShader(frag);
    DestroyGLResources();
    return false;
  }

  gl_program_ = glCreateProgram();
  glAttachShader(gl_program_, vert);
  glAttachShader(gl_program_, frag);
  glBindAttribLocation(gl_program_, kAttribPosition, "aPosition");
  glBindAttribLocation(gl_program_, kAttribColor, "aColor");
  glLinkProgram(gl_program_);

  GLint linked = 0;
  glGetProgramiv(gl_program_, GL_LINK_STATUS, &linked);
  if (!linked) {
    GLint info_len = 0;
    glGetProgramiv(gl_program_, GL_INFO_LOG_LENGTH, &info_len);
    if (info_len > 0) {
      char* buf = new char[info_len];
      glGetProgramInfoLog(gl_program_, info_len, nullptr, buf);
      LOGE("Program link error: %s", buf);
      delete[] buf;
    }
    glDeleteProgram(gl_program_);
    gl_program_ = 0;
    glDeleteShader(vert);
    glDeleteShader(frag);
    DestroyGLResources();
    return false;
  }
  glDeleteShader(vert);
  glDeleteShader(frag);

  // 6. Create VBO
  glGenBuffers(1, &gl_vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, gl_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kTriangleVertices), kTriangleVertices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  gl_resources_ready_ = true;
  LOGI("GL resources initialized (FBO=%u, texture=%u, program=%u)", gl_fbo_,
       gl_texture_, gl_program_);
  return true;
}

void TextureShareBackend::DestroyGLResources() {
  // Clear cached imported image (depends on GL resources via AHB)
  imported_image_.reset();

  if (!egl_initialized_) {
    return;
  }

  // Make EGL context current to delete GL objects
  eglMakeCurrent(egl_display_, egl_pbuffer_surface_, egl_pbuffer_surface_,
                 egl_context_);

  if (gl_fbo_ != 0) {
    glDeleteFramebuffers(1, &gl_fbo_);
    gl_fbo_ = 0;
  }
  if (gl_texture_ != 0) {
    glDeleteTextures(1, &gl_texture_);
    gl_texture_ = 0;
  }
  if (gl_vbo_ != 0) {
    glDeleteBuffers(1, &gl_vbo_);
    gl_vbo_ = 0;
  }
  if (gl_program_ != 0) {
    glDeleteProgram(gl_program_);
    gl_program_ = 0;
  }
  if (egl_image_ != EGL_NO_IMAGE_KHR) {
    g_eglDestroyImageKHR(egl_display_, egl_image_);
    egl_image_ = EGL_NO_IMAGE_KHR;
  }

  gl_resources_ready_ = false;
}

// ---------------------------------------------------------------------------
// Cached AHB → Vulkan texture import
// ---------------------------------------------------------------------------

bool TextureShareBackend::EnsureImportedImage() {
  if (imported_image_ != nullptr) {
    return true;
  }
  if (ahb_ == nullptr || vk_context_ == nullptr) {
    return false;
  }

  skity::GPUBackendTextureExtInfoAHB ahb_ext = {};
  ahb_ext.type = skity::GPUBackendTextureExtType::kAndroidHardwareBuffer;
  ahb_ext.hardware_buffer = ahb_;

  skity::GPUBackendTextureInfoVK tex_info = {};
  tex_info.backend = skity::GPUBackendType::kVulkan;
  tex_info.ext = &ahb_ext;
  tex_info.width = width_;
  tex_info.height = height_;

  auto texture = vk_context_->WrapTexture(&tex_info);
  if (!texture) {
    LOGW("WrapTexture returned null for AHB");
    return false;
  }

  imported_image_ = skity::Image::MakeHWImage(std::move(texture));
  if (!imported_image_) {
    LOGW("Failed to create Image from imported texture");
    return false;
  }

  LOGI("AHB imported as Vulkan texture (cached)");
  return true;
}

// ---------------------------------------------------------------------------
// GL frame rendering
// ---------------------------------------------------------------------------

void TextureShareBackend::RenderGLFrame() {
  if (!eglMakeCurrent(egl_display_, egl_pbuffer_surface_,
                      egl_pbuffer_surface_, egl_context_)) {
    LOGW("eglMakeCurrent failed in RenderGLFrame: 0x%x", eglGetError());
    return;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, gl_fbo_);
  glViewport(0, 0, static_cast<GLint>(width_), static_cast<GLint>(height_));

  // Dark background
  glClearColor(0.04f, 0.07f, 0.16f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw rotating triangle
  glUseProgram(gl_program_);
  GLint angle_loc = glGetUniformLocation(gl_program_, "uAngle");
  glUniform1f(angle_loc, static_cast<float>(frame_count_) * 0.02f);

  glBindBuffer(GL_ARRAY_BUFFER, gl_vbo_);

  // Position attribute (2 floats, stride = 5 floats)
  glEnableVertexAttribArray(kAttribPosition);
  glVertexAttribPointer(kAttribPosition, 2, GL_FLOAT, GL_FALSE,
                        5 * sizeof(float), nullptr);

  // Color attribute (3 floats, offset = 2 floats)
  glEnableVertexAttribArray(kAttribColor);
  glVertexAttribPointer(kAttribColor, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        reinterpret_cast<const void*>(2 * sizeof(float)));

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray(kAttribPosition);
  glDisableVertexAttribArray(kAttribColor);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glUseProgram(0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  frame_count_++;

  // Note: No glFinish() here. GPU-GPU synchronization is handled in DrawFrame()
  // via EGLSync → fence fd → VkSemaphore import. This avoids the CPU stall that
  // glFinish() would cause.
}

}  // namespace skity::demo
