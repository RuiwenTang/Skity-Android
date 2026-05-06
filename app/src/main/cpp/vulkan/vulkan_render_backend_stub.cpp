#include "vulkan/vulkan_render_backend_stub.hpp"

#include <memory>

namespace skity::demo {
namespace {

class VulkanRenderBackendStub final : public RenderBackend {
 public:
  void OnSurfaceCreated() override {}
  void OnSurfaceChanged(int width, int height) override {
    (void)width;
    (void)height;
  }
  void SetScene(int scene) override { (void)scene; }
  void DrawFrame() override {}
};

}  // namespace

std::unique_ptr<RenderBackend> CreateVulkanRenderBackendStub() {
  return std::make_unique<VulkanRenderBackendStub>();
}

}  // namespace skity::demo
