#pragma once

#include <cstdint>
#include <string>

namespace skity::demo {

struct VulkanProbeInfo {
  std::string vendor_name = "Unknown";
  std::string renderer_name = "Unknown";
  std::string version_name = "Unknown";
  uint64_t total_memory_bytes = 0;
  uint64_t device_local_memory_bytes = 0;
};

VulkanProbeInfo ProbeVulkanDeviceInfo();

}  // namespace skity::demo
