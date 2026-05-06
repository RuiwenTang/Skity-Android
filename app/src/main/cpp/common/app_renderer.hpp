#pragma once

#include <memory>

namespace skity::demo {

class RenderBackend;

class AppRenderer {
 public:
  AppRenderer();
  ~AppRenderer();

  void OnSurfaceCreated();
  void OnSurfaceChanged(int width, int height);
  void SetScene(int scene);
  void DrawFrame();

 private:
  std::unique_ptr<RenderBackend> backend_;
};

}  // namespace skity::demo
