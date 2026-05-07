#include "common/renderer_diagnostics.hpp"

#include <array>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <utility>

namespace skity::demo {

namespace {

constexpr double kNsPerMs = 1000000.0;
constexpr double kNsPerSecond = 1000000000.0;
constexpr double kFpsSmoothingFactor = 0.15;
constexpr double kFrameSmoothingFactor = 0.20;

uint64_t NowNs() {
  return static_cast<uint64_t>(
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count());
}

std::string SafeString(const char* value) {
  return value == nullptr || value[0] == '\0' ? "Unknown" : value;
}

}  // namespace

void RendererDiagnostics::SetBackendName(const char* backend_name) {
  std::lock_guard<std::mutex> lock(mutex_);
  backend_name_ = SafeString(backend_name);
}

void RendererDiagnostics::SetSurfaceName(const char* surface_name) {
  std::lock_guard<std::mutex> lock(mutex_);
  surface_name_ = SafeString(surface_name);
}

void RendererDiagnostics::SetValidationEnabled(bool enabled) {
  std::lock_guard<std::mutex> lock(mutex_);
  validation_enabled_ = enabled;
}

void RendererDiagnostics::SetContextReady(bool ready) {
  std::lock_guard<std::mutex> lock(mutex_);
  context_ready_ = ready;
}

void RendererDiagnostics::SetSurfaceSize(int width, int height) {
  std::lock_guard<std::mutex> lock(mutex_);
  width_ = width;
  height_ = height;
}

void RendererDiagnostics::SetGpuInfo(const char* vendor_name,
                                     const char* renderer_name,
                                     const char* version_name) {
  std::lock_guard<std::mutex> lock(mutex_);
  gpu_vendor_ = SafeString(vendor_name);
  gpu_renderer_ = SafeString(renderer_name);
  gpu_version_ = SafeString(version_name);
}

void RendererDiagnostics::SetMemoryInfo(uint64_t total_bytes,
                                        uint64_t device_local_bytes) {
  std::lock_guard<std::mutex> lock(mutex_);
  total_memory_bytes_ = total_bytes;
  device_local_memory_bytes_ = device_local_bytes;
}

void RendererDiagnostics::RecordFrame() {
  const uint64_t now_ns = NowNs();
  std::lock_guard<std::mutex> lock(mutex_);
  frames_drawn_ += 1;
  if (last_frame_time_ns_ != 0 && now_ns > last_frame_time_ns_) {
    const double delta_ns = static_cast<double>(now_ns - last_frame_time_ns_);
    const double instant_frame_ms = delta_ns / kNsPerMs;
    const double instant_fps = kNsPerSecond / delta_ns;
    frame_time_ms_ = frame_time_ms_ <= 0.0
                         ? instant_frame_ms
                         : frame_time_ms_ * (1.0 - kFrameSmoothingFactor) +
                               instant_frame_ms * kFrameSmoothingFactor;
    fps_ = fps_ <= 0.0 ? instant_fps
                       : fps_ * (1.0 - kFpsSmoothingFactor) +
                             instant_fps * kFpsSmoothingFactor;
  }
  last_frame_time_ns_ = now_ns;
}

std::string RendererDiagnostics::BuildOverlayText() const {
  std::lock_guard<std::mutex> lock(mutex_);
  std::ostringstream stream;
  stream << std::fixed << std::setprecision(1);
  stream << "Backend: " << backend_name_ << '\n';
  stream << "Surface: " << surface_name_ << "  " << width_ << "x" << height_
         << '\n';
  stream << "Context: " << (context_ready_ ? "Ready" : "Pending") << '\n';
  stream << "Validation: " << (validation_enabled_ ? "On" : "Off") << '\n';
  stream << "FPS: " << fps_ << "  Frame: " << frame_time_ms_ << " ms" << '\n';
  stream << "Frames: " << frames_drawn_ << '\n';
  stream << "GPU: " << gpu_renderer_ << '\n';
  stream << "Vendor: " << gpu_vendor_ << '\n';
  stream << "Version: " << gpu_version_;
  if (total_memory_bytes_ > 0 || device_local_memory_bytes_ > 0) {
    stream << '\n'
           << "GPU Heaps: total " << FormatBytes(total_memory_bytes_);
    if (device_local_memory_bytes_ > 0) {
      stream << "  local " << FormatBytes(device_local_memory_bytes_);
    }
  }
  return stream.str();
}

std::string RendererDiagnostics::FormatBytes(uint64_t bytes) {
  if (bytes == 0) {
    return "n/a";
  }

  static constexpr std::array<const char*, 5> kUnits = {"B", "KB", "MB", "GB",
                                                         "TB"};
  double value = static_cast<double>(bytes);
  size_t unit_index = 0;
  while (value >= 1024.0 && unit_index < kUnits.size() - 1) {
    value /= 1024.0;
    unit_index += 1;
  }

  std::ostringstream stream;
  stream << std::fixed << std::setprecision(unit_index == 0 ? 0 : 1) << value
         << ' ' << kUnits[unit_index];
  return stream.str();
}

}  // namespace skity::demo
