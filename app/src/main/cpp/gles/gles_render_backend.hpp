#pragma once

#include <atomic>
#include <memory>

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

  void OnSurfaceCreated() override;
  void OnSurfaceChanged(int width, int height) override;
  void SetScene(int scene) override;
  void DrawFrame() override;

 private:
  void ClearFallbackFrame() const;

  std::unique_ptr<skity::GPUContext> context_;
  std::atomic<int> scene_{1};
  int width_ = 0;
  int height_ = 0;
};

}  // namespace skity::demo
