#include "vulkan/vulkan_probe.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace skity::demo {
namespace {

const char* VulkanVendorName(uint32_t vendor_id) {
  switch (vendor_id) {
    case 0x13B5:
      return "ARM";
    case 0x5143:
      return "Qualcomm";
    case 0x1010:
      return "ImgTec";
    case 0x10DE:
      return "NVIDIA";
    case 0x1002:
      return "AMD";
    case 0x8086:
      return "Intel";
    case 0x106B:
      return "Apple";
    default:
      return "Unknown";
  }
}

std::string FormatApiVersion(uint32_t version) {
  return std::string("Vulkan ") +
         std::to_string(VK_API_VERSION_MAJOR(version)) + "." +
         std::to_string(VK_API_VERSION_MINOR(version)) + "." +
         std::to_string(VK_API_VERSION_PATCH(version));
}

}  // namespace

VulkanProbeInfo ProbeVulkanDeviceInfo() {
  VulkanProbeInfo info;

  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "SkityDevProbe";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "skity";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo instance_info = {};
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo = &app_info;

  VkInstance instance = VK_NULL_HANDLE;
  if (vkCreateInstance(&instance_info, nullptr, &instance) != VK_SUCCESS ||
      instance == VK_NULL_HANDLE) {
    return info;
  }

  uint32_t physical_device_count = 0;
  if (vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr) !=
          VK_SUCCESS ||
      physical_device_count == 0) {
    vkDestroyInstance(instance, nullptr);
    return info;
  }

  std::vector<VkPhysicalDevice> physical_devices(physical_device_count,
                                                 VK_NULL_HANDLE);
  if (vkEnumeratePhysicalDevices(instance, &physical_device_count,
                                 physical_devices.data()) != VK_SUCCESS) {
    vkDestroyInstance(instance, nullptr);
    return info;
  }

  VkPhysicalDeviceProperties properties = {};
  vkGetPhysicalDeviceProperties(physical_devices[0], &properties);
  VkPhysicalDeviceMemoryProperties memory_properties = {};
  vkGetPhysicalDeviceMemoryProperties(physical_devices[0], &memory_properties);

  info.vendor_name = VulkanVendorName(properties.vendorID);
  info.renderer_name = properties.deviceName;
  info.version_name = FormatApiVersion(properties.apiVersion);

  for (uint32_t index = 0; index < memory_properties.memoryHeapCount; ++index) {
    const auto& heap = memory_properties.memoryHeaps[index];
    info.total_memory_bytes += heap.size;
    if ((heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0) {
      info.device_local_memory_bytes += heap.size;
    }
  }

  vkDestroyInstance(instance, nullptr);
  return info;
}

}  // namespace skity::demo
