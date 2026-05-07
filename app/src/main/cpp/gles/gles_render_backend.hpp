#pragma once

#include <atomic>
#include <memory>

#include "common/renderer_diagnostics.hpp"
#include "common/render_backend.hpp"

namespace skity {
class GPUContext;
}

namespace skity::demo {

std::unique_ptr<RenderBackend> CreateGlesRenderBackend();

class GlesRenderBackend final : public RenderBackend {
 public:
  GlesRenderBackend();
  ~GlesRenderBackend() override;

  void SetNativeWindow(ANativeWindow* native_window) override;
  void OnSurfaceCreated() override;
  void OnSurfaceDestroyed() override;
  void OnSurfaceChanged(int width, int height) override;
  void SetScene(int scene) override;
  void SetMsaaSampleCount(int sample_count) override;
  void DrawFrame() override;
  std::string GetOverlayText() const override;

 private:
  void ClearFallbackFrame() const;

  std::unique_ptr<skity::GPUContext> context_;
  RendererDiagnostics diagnostics_;
  std::atomic<int> scene_{1};
  std::atomic<int> sample_count_{1};
  int width_ = 0;
  int height_ = 0;
};

}  // namespace skity::demo
