#include "gles/gles_render_backend.hpp"

#include <GLES3/gl3.h>

#include <cstdint>

#include <skity/gpu/gpu_context_gl.hpp>
#include <skity/gpu/gpu_surface.hpp>

#include "common/demo_scene.hpp"
#include "common/demo_scene_renderer.hpp"
#include "gles/gl_proc_resolver.hpp"

namespace skity::demo {

std::unique_ptr<RenderBackend> CreateGlesRenderBackend() {
  return std::make_unique<GlesRenderBackend>();
}

GlesRenderBackend::GlesRenderBackend() {
  diagnostics_.SetBackendName("OpenGL ES");
  diagnostics_.SetSurfaceName("Framebuffer");
  diagnostics_.SetValidationEnabled(false);
  diagnostics_.SetMsaaSampleCount(1);
}

GlesRenderBackend::~GlesRenderBackend() = default;

void GlesRenderBackend::SetNativeWindow(ANativeWindow* native_window) {
  (void)native_window;
}

void GlesRenderBackend::OnSurfaceCreated() {
  if (context_ != nullptr) {
    return;
  }

  context_ = skity::GLContextCreate(
      reinterpret_cast<void*>(ResolveGLProcAddress));
  diagnostics_.SetContextReady(context_ != nullptr);
  diagnostics_.SetGpuInfo(
      reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
      reinterpret_cast<const char*>(glGetString(GL_RENDERER)),
      reinterpret_cast<const char*>(glGetString(GL_VERSION)));
}

void GlesRenderBackend::OnSurfaceDestroyed() {
  context_.reset();
  diagnostics_.SetContextReady(false);
  diagnostics_.SetSurfaceSize(0, 0);
  width_ = 0;
  height_ = 0;
}

void GlesRenderBackend::OnSurfaceChanged(int width, int height) {
  width_ = width;
  height_ = height;
  diagnostics_.SetSurfaceSize(width, height);
  glViewport(0, 0, width, height);
}

void GlesRenderBackend::SetScene(int scene) {
  scene_.store(scene);
}

void GlesRenderBackend::SetMsaaSampleCount(int sample_count) {
  const int normalized = sample_count <= 1 ? 1 : sample_count;
  sample_count_.store(normalized);
  diagnostics_.SetMsaaSampleCount(normalized);
}

void GlesRenderBackend::DrawFrame() {
  if (width_ <= 0 || height_ <= 0) {
    return;
  }

  if (context_ == nullptr) {
    ClearFallbackFrame();
    return;
  }

  skity::GPUSurfaceDescriptorGL surface_desc{};
  surface_desc.backend = skity::GPUBackendType::kOpenGL;
  surface_desc.width = static_cast<uint32_t>(width_);
  surface_desc.height = static_cast<uint32_t>(height_);
  surface_desc.content_scale = 1.0f;
  surface_desc.sample_count = static_cast<uint32_t>(sample_count_.load());
  surface_desc.surface_type = skity::GLSurfaceType::kFramebuffer;
  surface_desc.gl_id = 0;
  surface_desc.has_stencil_attachment = false;
  surface_desc.can_blit_from_target_fbo = false;

  auto surface = context_->CreateSurface(&surface_desc);
  if (!surface) {
    ClearFallbackFrame();
    return;
  }

  auto* canvas = surface->LockCanvas(true);
  if (canvas == nullptr) {
    ClearFallbackFrame();
    return;
  }

  DrawDemoScene(canvas, static_cast<DemoScene>(scene_.load()),
                DemoBackend::kGles, false, width_, height_);
  canvas->Flush();
  surface->Flush();
  diagnostics_.RecordFrame();
}

void GlesRenderBackend::ClearFallbackFrame() const {
  glClearColor(0.25f, 0.08f, 0.08f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

std::string GlesRenderBackend::GetOverlayText() const {
  return diagnostics_.BuildOverlayText();
}

}  // namespace skity::demo
