#include "common/app_renderer.hpp"

#include "common/render_backend.hpp"
#include "gles/gles_render_backend.hpp"

namespace skity::demo {

AppRenderer::AppRenderer() : backend_(CreateGlesRenderBackend()) {}

AppRenderer::~AppRenderer() = default;

void AppRenderer::OnSurfaceCreated() {
  backend_->OnSurfaceCreated();
}

void AppRenderer::OnSurfaceChanged(int width, int height) {
  backend_->OnSurfaceChanged(width, height);
}

void AppRenderer::SetScene(int scene) {
  backend_->SetScene(scene);
}

void AppRenderer::DrawFrame() {
  backend_->DrawFrame();
}

}  // namespace skity::demo
