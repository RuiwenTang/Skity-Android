#pragma once

#include <cstdint>
#include <mutex>
#include <string>

namespace skity::demo {

class RendererDiagnostics {
 public:
  void SetBackendName(const char* backend_name);
  void SetSurfaceName(const char* surface_name);
  void SetPresentModeRequest(int32_t present_mode);
  void SetPresentModeActual(int32_t present_mode);
  void SetValidationEnabled(bool enabled);
  void SetMsaaSampleCount(int sample_count);
  void SetContextReady(bool ready);
  void SetSurfaceSize(int width, int height);
  void SetGpuInfo(const char* vendor_name,
                  const char* renderer_name,
                  const char* version_name);
  void SetMemoryInfo(uint64_t total_bytes, uint64_t device_local_bytes);
  void RecordFrame();
  std::string BuildOverlayText() const;

 private:
  static std::string FormatBytes(uint64_t bytes);

  mutable std::mutex mutex_;
  std::string backend_name_ = "Unknown";
  std::string surface_name_ = "Unknown";
  int32_t present_mode_request_ = 0;
  int32_t present_mode_actual_ = 0;
  std::string gpu_vendor_ = "Unknown";
  std::string gpu_renderer_ = "Unknown";
  std::string gpu_version_ = "Unknown";
  bool validation_enabled_ = false;
  int msaa_sample_count_ = 1;
  bool context_ready_ = false;
  int width_ = 0;
  int height_ = 0;
  uint64_t total_memory_bytes_ = 0;
  uint64_t device_local_memory_bytes_ = 0;
  uint64_t frames_drawn_ = 0;
  double fps_ = 0.0;
  double frame_time_ms_ = 0.0;
  uint64_t last_frame_time_ns_ = 0;
};

}  // namespace skity::demo
