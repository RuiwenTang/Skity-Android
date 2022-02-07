
#include "vk_renderer.hpp"
#include <vector>
#include <android/log.h>
#include <cassert>
#include <set>

// Vulkan call wrapper
#define CALL_VK(func)                                                 \
  do {                                                                \
  if (VK_SUCCESS != (func)) {                                         \
    __android_log_print(ANDROID_LOG_ERROR, "Tutorial ",               \
                        "Vulkan error. File[%s], line[%d]", __FILE__, \
                        __LINE__);                                    \
    assert(false);                                                    \
    }                                                                 \
  }while(false)


static VkSampleCountFlagBits get_max_usable_sample_count(
        VkPhysicalDeviceProperties props) {
    VkSampleCountFlags counts = props.limits.framebufferColorSampleCounts &
                                props.limits.framebufferStencilSampleCounts;

    if (counts & VK_SAMPLE_COUNT_64_BIT) return VK_SAMPLE_COUNT_64_BIT;
    if (counts & VK_SAMPLE_COUNT_32_BIT) return VK_SAMPLE_COUNT_32_BIT;
    if (counts & VK_SAMPLE_COUNT_16_BIT) return VK_SAMPLE_COUNT_16_BIT;
    if (counts & VK_SAMPLE_COUNT_8_BIT) return VK_SAMPLE_COUNT_8_BIT;
    if (counts & VK_SAMPLE_COUNT_4_BIT) return VK_SAMPLE_COUNT_4_BIT;
    if (counts & VK_SAMPLE_COUNT_2_BIT) return VK_SAMPLE_COUNT_2_BIT;

    return VK_SAMPLE_COUNT_1_BIT;
}

static VkFormat choose_swap_chain_format(VkPhysicalDevice phy_device,
                                         VkSurfaceKHR surface) {
    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(phy_device, surface, &format_count,
                                         nullptr);

    if (format_count == 0) {
        return VK_FORMAT_UNDEFINED;
    }

    std::vector<VkSurfaceFormatKHR> formats{format_count};
    vkGetPhysicalDeviceSurfaceFormatsKHR(phy_device, surface, &format_count,
                                         formats.data());

    if (format_count == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
        return VK_FORMAT_R8G8B8A8_UNORM;
    }

    for (VkSurfaceFormatKHR format : formats) {
        if (format.format == VK_FORMAT_R8G8B8A8_UNORM &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format.format;
        }
    }

    return formats[0].format;
}

static bool get_support_depth_format(VkPhysicalDevice phy_devce,
                                     VkFormat *out_format) {
    // Since all depth formats may be optional, we need to find a suitable depth
    // format to use
    // Start with the highest precision packed format
    std::vector<VkFormat> depthFormats = {
            VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM};

    for (auto &format : depthFormats) {
        VkFormatProperties formatProps;
        vkGetPhysicalDeviceFormatProperties(phy_devce, format, &formatProps);
        // Format must support depth stencil attachment for optimal tiling
        if (formatProps.optimalTilingFeatures &
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            *out_format = format;
            return true;
        }
    }

    return false;
}

void VkRenderer::init(int w, int h, int d, ANativeWindow *window) {
    width_ = w;
    height_ = h;
    density_ = d;
    init_vk(window);
}

void VkRenderer::destroy() {
    canvas_.reset();

    for (auto semp : present_semaphore_) {
        vkDestroySemaphore(vk_device_, semp, nullptr);
    }
    for (auto semp : render_semaphore_) {
        vkDestroySemaphore(vk_device_, semp, nullptr);
    }
    for (auto fence : cmd_fences_) {
        vkDestroyFence(vk_device_, fence, nullptr);
    }

    vkResetCommandPool(vk_device_, cmd_pool_, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    vkDestroyCommandPool(vk_device_, cmd_pool_, nullptr);

    for (auto const &st : stencil_image_) {
        vkDestroyImageView(vk_device_, st.image_view, nullptr);
        vkDestroyImage(vk_device_, st.image, nullptr);
        vkFreeMemory(vk_device_, st.memory, nullptr);
    }

    for (auto const &si : sampler_image_) {
        vkDestroyImageView(vk_device_, si.image_view, nullptr);
        vkDestroyImage(vk_device_, si.image, nullptr);
        vkFreeMemory(vk_device_, si.memory, nullptr);
    }

    vkDestroySwapchainKHR(vk_device_, vk_swap_chain_, nullptr);
    vkDestroyDevice(vk_device_, nullptr);
    vkDestroySurfaceKHR(vk_instance_, vk_surface_, nullptr);
    vkDestroyInstance(vk_instance_, nullptr);
}

void VkRenderer::draw() {}

void VkRenderer::set_default_typeface(std::shared_ptr<skity::Typeface> typeface) {
    canvas_->setDefaultTypeface(std::move(typeface));
}

void VkRenderer::init_vk(ANativeWindow *window) {
    VkResult result = volkInitialize();
    assert(result == VK_SUCCESS);

    create_vk_instance();
    create_vk_surface(window);
    pick_phy_device();
    create_device();
    create_swap_chain();
    create_swap_chain_views();
    create_command_pool();
    create_command_buffers();
    create_sync_objects();
}

void VkRenderer::create_vk_instance() {
    VkApplicationInfo app_info{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.pApplicationName = "Skity VkRenderer";
    app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.pEngineName = "Skity";
    app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.apiVersion = VK_API_VERSION_1_1;

    std::vector<const char *> instance_ext{};
    instance_ext.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
    instance_ext.emplace_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);

    VkInstanceCreateInfo create_info{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = instance_ext.size();
    create_info.ppEnabledExtensionNames = instance_ext.data();

    CALL_VK(vkCreateInstance(&create_info, nullptr, &vk_instance_));

    volkLoadInstance(vk_instance_);
}

void VkRenderer::pick_phy_device() {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(vk_instance_, &device_count, nullptr);

    assert(device_count > 0);

    std::vector<VkPhysicalDevice> available_devices{device_count};
    vkEnumeratePhysicalDevices(vk_instance_, &device_count, available_devices.data());

    int32_t graphic_queue_family = -1;
    int32_t present_queue_family = -1;
    int32_t compute_queue_family = -1;

    for (size_t i = 0; i < available_devices.size(); i++) {
        uint32_t queue_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(available_devices[i], &queue_count,
                                                 nullptr);

        std::vector<VkQueueFamilyProperties> queue_family_properties{queue_count};
        vkGetPhysicalDeviceQueueFamilyProperties(available_devices[i], &queue_count,
                                                 queue_family_properties.data());

        auto graphic_it = std::find_if(
                queue_family_properties.begin(), queue_family_properties.end(),
                [](VkQueueFamilyProperties props) {
                    return props.queueFlags & VK_QUEUE_GRAPHICS_BIT;
                });

        auto present_it = std::find_if(
                queue_family_properties.begin(), queue_family_properties.end(),
                [](VkQueueFamilyProperties props) {
                    return props.queueFlags & VK_QUEUE_PROTECTED_BIT;
                });

        auto compute_it = std::find_if(
                queue_family_properties.begin(), queue_family_properties.end(),
                [](VkQueueFamilyProperties props) {
                    return props.queueFlags & VK_QUEUE_COMPUTE_BIT;
                });

        if (graphic_it != queue_family_properties.end() &&
            compute_it != queue_family_properties.end()) {
            vk_phy_device_ = available_devices[i];
            graphic_queue_family =
                    std::distance(graphic_it, queue_family_properties.begin());

            present_queue_family =
                    std::distance(present_it, queue_family_properties.begin());

            compute_queue_family =
                    std::distance(compute_it, queue_family_properties.begin());
            break;
        }
    }

    VkPhysicalDeviceProperties phy_props;
    vkGetPhysicalDeviceProperties(vk_phy_device_, &phy_props);

    __android_log_print(ANDROID_LOG_INFO, "SkityVk ", "picked gpu name : %s", phy_props.deviceName);

    {
        // query all extensions
        uint32_t entry_count = 0;
        vkEnumerateDeviceExtensionProperties(vk_phy_device_, nullptr, &entry_count,
                                             nullptr);
        std::vector<VkExtensionProperties> entries{entry_count};
        vkEnumerateDeviceExtensionProperties(vk_phy_device_, nullptr, &entry_count,
                                             entries.data());
        for (auto ext : entries) {
            __android_log_print(ANDROID_LOG_INFO, "SkityVk ", "ext name : %s", ext.extensionName);
        }
    }

    if (graphic_queue_family == -1 || present_queue_family == -1) {
        __android_log_print(ANDROID_LOG_INFO, "SkityVk ",
                            "Can not find GPU contains Graphic support");
        assert(false);
    }

    VkBool32 support = 0;
    vkGetPhysicalDeviceSurfaceSupportKHR(vk_phy_device_, graphic_queue_family,
                                         vk_surface_, &support);

    if (support == VK_TRUE) {
        present_queue_family = graphic_queue_family;
    }

    graphic_queue_index_ = graphic_queue_family;
    present_queue_index_ = present_queue_family;
    compute_queue_index_ = compute_queue_family;

    vk_sample_count_ = get_max_usable_sample_count(phy_props);

    __android_log_print(ANDROID_LOG_INFO, "SkityVk ", "queue family [ %d, %d, %d ]",
                        graphic_queue_index_, present_queue_index_, compute_queue_index_);
    __android_log_print(ANDROID_LOG_INFO, "SkityVk ", "sample count = %x", vk_sample_count_);
}

void VkRenderer::create_device() {
    std::vector<VkDeviceQueueCreateInfo> queue_create_info{};

    std::set<uint32_t> queue_families = {
            (uint32_t) graphic_queue_index_,
            (uint32_t) present_queue_index_,
            (uint32_t) compute_queue_index_,
    };
    float queue_priority = 1.f;

    for (uint32_t family : queue_families) {
        VkDeviceQueueCreateInfo create_info{
                VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        create_info.queueFamilyIndex = family;
        create_info.queueCount = 1;
        create_info.pQueuePriorities = &queue_priority;

        queue_create_info.emplace_back(create_info);
    }

    VkPhysicalDeviceFeatures device_features{};

    std::vector<const char *> required_device_extension{
            VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    {
        uint32_t count;
        vkEnumerateDeviceExtensionProperties(vk_phy_device_, nullptr, &count,
                                             nullptr);

        std::vector<VkExtensionProperties> properties(count);
        vkEnumerateDeviceExtensionProperties(vk_phy_device_, nullptr, &count,
                                             properties.data());

        auto it = std::find_if(
                properties.begin(), properties.end(), [](VkExtensionProperties prop) {
                    return std::strcmp(prop.extensionName, "VK_KHR_portability_subset") ==
                           0;
                });

        if (it != properties.end()) {
            // VUID-VkDeviceCreateInfo-pProperties-04451
            required_device_extension.emplace_back("VK_KHR_portability_subset");
        }
    }

    VkDeviceCreateInfo create_info{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    create_info.pQueueCreateInfos = queue_create_info.data();
    create_info.queueCreateInfoCount = queue_create_info.size();
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = required_device_extension.size();
    create_info.ppEnabledExtensionNames = required_device_extension.data();

    CALL_VK(vkCreateDevice(vk_phy_device_, &create_info, nullptr, &vk_device_));

    volkLoadDevice(vk_device_);

    vkGetDeviceQueue(vk_device_, graphic_queue_index_, 0, &vk_graphic_queue_);
    vkGetDeviceQueue(vk_device_, present_queue_index_, 0, &vk_present_queue_);
    vkGetDeviceQueue(vk_device_, compute_queue_index_, 0, &vk_compute_queue_);
}

void VkRenderer::create_vk_surface(ANativeWindow *window) {
    VkAndroidSurfaceCreateInfoKHR create_info{VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR};

    create_info.window = window;

    CALL_VK(vkCreateAndroidSurfaceKHR(vk_instance_, &create_info, nullptr, &vk_surface_));
}

void VkRenderer::create_swap_chain() {
    VkFormat format = choose_swap_chain_format(vk_phy_device_, vk_surface_);

    VkSurfaceCapabilitiesKHR surface_caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_phy_device_, vk_surface_,
                                              &surface_caps);

    VkCompositeAlphaFlagBitsKHR surface_composite;
    if (surface_caps.supportedCompositeAlpha &
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {
        surface_composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    } else if (surface_caps.supportedCompositeAlpha &
               VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
        surface_composite = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
    } else if (surface_caps.supportedCompositeAlpha &
               VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {
        surface_composite = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
    } else {
        surface_composite = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    }

    VkSwapchainCreateInfoKHR create_info{
            VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    create_info.surface = vk_surface_;
    create_info.minImageCount = std::max(uint32_t(2), surface_caps.minImageCount);
    create_info.imageFormat = format;
    create_info.imageExtent = surface_caps.currentExtent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.queueFamilyIndexCount = 1;
    create_info.pQueueFamilyIndices = &present_queue_index_;
    create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    create_info.compositeAlpha = surface_composite;
    create_info.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    CALL_VK(vkCreateSwapchainKHR(vk_device_, &create_info, nullptr, &vk_swap_chain_));

    swap_chain_format_ = format;
    swap_chain_extend_ = surface_caps.currentExtent;
}

uint32_t VkRenderer::get_memory_type(uint32_t type_bits,
                                     VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(vk_phy_device_, &memory_properties);

    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
        if ((type_bits & 1) == 1) {
            if ((memory_properties.memoryTypes[i].propertyFlags & properties) ==
                properties) {
                return i;
            }
        }

        type_bits >>= 1;
    }

    return 0;
}

void VkRenderer::create_swap_chain_views() {
    uint32_t image_count = 0;
    vkGetSwapchainImagesKHR(vk_device_, vk_swap_chain_, &image_count, nullptr);

    std::vector<VkImage> swap_chain_image{image_count};
    vkGetSwapchainImagesKHR(vk_device_, vk_swap_chain_, &image_count,
                            swap_chain_image.data());

    sampler_image_.resize(image_count);
    for (size_t i = 0; i < sampler_image_.size(); i++) {
        VkImageCreateInfo img_create_info{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
        img_create_info.imageType = VK_IMAGE_TYPE_2D;
        img_create_info.format = swap_chain_format_;
        img_create_info.extent = {swap_chain_extend_.width,
                                  swap_chain_extend_.height, 1};
        img_create_info.mipLevels = 1;
        img_create_info.arrayLayers = 1;
        img_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        img_create_info.samples = vk_sample_count_;
        img_create_info.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        img_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        CALL_VK(vkCreateImage(vk_device_, &img_create_info, nullptr,
                              &sampler_image_[i].image) != VK_SUCCESS);

        VkMemoryRequirements mem_reqs{};
        vkGetImageMemoryRequirements(vk_device_, sampler_image_[i].image,
                                     &mem_reqs);
        VkMemoryAllocateInfo mem_alloc{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
        mem_alloc.allocationSize = mem_reqs.size;
        mem_alloc.memoryTypeIndex = get_memory_type(
                mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        CALL_VK(vkAllocateMemory(vk_device_, &mem_alloc, nullptr,
                                 &sampler_image_[i].memory) != VK_SUCCESS);

        CALL_VK(vkBindImageMemory(vk_device_, sampler_image_[i].image,
                                  sampler_image_[i].memory, 0) != VK_SUCCESS);

        // image view for MSAA
        VkImageViewCreateInfo view_info{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        view_info.image = sampler_image_[i].image;
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = swap_chain_format_;
        view_info.components.r = VK_COMPONENT_SWIZZLE_R;
        view_info.components.g = VK_COMPONENT_SWIZZLE_G;
        view_info.components.b = VK_COMPONENT_SWIZZLE_B;
        view_info.components.a = VK_COMPONENT_SWIZZLE_A;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.layerCount = 1;

        CALL_VK(vkCreateImageView(vk_device_, &view_info, nullptr,
                                  &sampler_image_[i].image_view) != VK_SUCCESS);

        sampler_image_[i].format = swap_chain_format_;
    }

    // create image view for color buffer submit to screen
    swap_chain_image_view_.resize(image_count);
    for (uint32_t i = 0; i < image_count; i++) {
        VkImageViewCreateInfo create_info{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.image = swap_chain_image[i];
        create_info.format = swap_chain_format_;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;
        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

        CALL_VK(vkCreateImageView(vk_device_, &create_info, nullptr,
                                  &swap_chain_image_view_[i]) != VK_SUCCESS);
    }
    if (!get_support_depth_format(vk_phy_device_, &depth_stencil_format_)) {
        assert(false);
    }
    // create image and image-view for stencil buffer
    VkImageCreateInfo image_create_info{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.format = depth_stencil_format_;
    image_create_info.extent = {swap_chain_extend_.width,
                                swap_chain_extend_.height, 1};
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.samples = vk_sample_count_;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    stencil_image_.resize(image_count);

    for (size_t i = 0; i < stencil_image_.size(); i++) {
        CALL_VK(vkCreateImage(vk_device_, &image_create_info, nullptr,
                              &stencil_image_[i].image) != VK_SUCCESS);

        VkMemoryRequirements mem_reqs{};
        vkGetImageMemoryRequirements(vk_device_, stencil_image_[i].image,
                                     &mem_reqs);

        VkMemoryAllocateInfo mem_alloc{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
        mem_alloc.allocationSize = mem_reqs.size;
        mem_alloc.memoryTypeIndex = get_memory_type(
                mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        CALL_VK(vkAllocateMemory(vk_device_, &mem_alloc, nullptr,
                                 &stencil_image_[i].memory) != VK_SUCCESS);

        CALL_VK(vkBindImageMemory(vk_device_, stencil_image_[i].image,
                                  stencil_image_[i].memory, 0) != VK_SUCCESS);

        VkImageViewCreateInfo image_view_create_info{
                VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.image = stencil_image_[i].image;
        image_view_create_info.format = depth_stencil_format_;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;
        image_view_create_info.subresourceRange.aspectMask =
                VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

        CALL_VK(vkCreateImageView(vk_device_, &image_view_create_info, nullptr,
                                  &stencil_image_[i].image_view) != VK_SUCCESS);

        stencil_image_[i].format = depth_stencil_format_;
    }
}

void VkRenderer::create_command_pool() {
    VkCommandPoolCreateInfo create_info{
            VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    create_info.queueFamilyIndex = graphic_queue_index_;
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CALL_VK(vkCreateCommandPool(vk_device_, &create_info, nullptr, &cmd_pool_) !=
            VK_SUCCESS);
}

void VkRenderer::create_command_buffers() {
    cmd_buffers_.resize(swap_chain_image_view_.size());

    VkCommandBufferAllocateInfo allocate_info{
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    allocate_info.commandPool = cmd_pool_;
    allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocate_info.commandBufferCount = static_cast<uint32_t>(cmd_buffers_.size());

    CALL_VK(vkAllocateCommandBuffers(vk_device_, &allocate_info,
                                     cmd_buffers_.data()) != VK_SUCCESS);
}

void VkRenderer::create_sync_objects() {
    VkFenceCreateInfo create_info{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    cmd_fences_.resize(cmd_buffers_.size());

    for (size_t i = 0; i < cmd_fences_.size(); i++) {
        CALL_VK(vkCreateFence(vk_device_, &create_info, nullptr, &cmd_fences_[i]) !=
                VK_SUCCESS);
    }

    present_semaphore_.resize(cmd_buffers_.size());
    render_semaphore_.resize(cmd_buffers_.size());

    VkSemaphoreCreateInfo semaphore_create_info{
            VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    for (size_t i = 0; i < present_semaphore_.size(); i++) {
        CALL_VK(vkCreateSemaphore(vk_device_, &semaphore_create_info, nullptr,
                                  &present_semaphore_[i]) != VK_SUCCESS);
    }
    for (size_t i = 0; i < render_semaphore_.size(); i++) {
        CALL_VK(vkCreateSemaphore(vk_device_, &semaphore_create_info, nullptr,
                                  &render_semaphore_[i]) != VK_SUCCESS);
    }
}
