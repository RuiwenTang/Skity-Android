
#include "vk_renderer.hpp"
#include <array>
#include <vector>
#include <android/log.h>
#include <cassert>
#include <set>
#include <limits>

static const char *kTAG = "SkityVK";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, kTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))

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


/**
 * Validation Layer name
 */
static const char *kValLayerName = "VK_LAYER_KHRONOS_validation";

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
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT,
    };

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
    window_ = window;
    init_vk(window);
    this->proc_loader = (void *) vkGetDeviceProcAddr;
    canvas_ = skity::Canvas::MakeHardwareAccelationCanvas(width_, height_, density_, this);
}

void VkRenderer::destroy() {
    vkDeviceWaitIdle(vk_device_);

    canvas_.reset();

    destroy_swap_chain_views();

    vkDestroyRenderPass(vk_device_, vk_render_pass_, nullptr);
    vk_render_pass_ = VK_NULL_HANDLE;

    for (auto semp : present_semaphore_) {
        vkDestroySemaphore(vk_device_, semp, nullptr);
    }
    present_semaphore_.clear();

    for (auto semp : render_semaphore_) {
        vkDestroySemaphore(vk_device_, semp, nullptr);
    }
    render_semaphore_.clear();

    for (auto fence : cmd_fences_) {
        vkDestroyFence(vk_device_, fence, nullptr);
    }
    cmd_fences_.clear();

    vkResetCommandPool(vk_device_, cmd_pool_, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    vkDestroyCommandPool(vk_device_, cmd_pool_, nullptr);
    cmd_pool_ = VK_NULL_HANDLE;

    vkDestroySwapchainKHR(vk_device_, vk_swap_chain_, nullptr);
    vk_swap_chain_ = VK_NULL_HANDLE;

    vkDestroyDevice(vk_device_, nullptr);
    vk_device_ = VK_NULL_HANDLE;

    vkDestroySurfaceKHR(vk_instance_, vk_surface_, nullptr);
    vk_surface_ = VK_NULL_HANDLE;

    vkDestroyInstance(vk_instance_, nullptr);
    vk_instance_ = VK_NULL_HANDLE;

    __android_log_print(ANDROID_LOG_INFO, "SkityVk ", "VkRender Context clean up.");
}

void VkRenderer::draw() {
    VkResult result = vkAcquireNextImageKHR(vk_device_, vk_swap_chain_,
                                            UINT64_MAX,
                                            present_semaphore_[frame_index_],
                                            VK_NULL_HANDLE, &current_frame_);


    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        __android_log_print(ANDROID_LOG_ERROR, "SkityVK",
                            "need to handle window resize of recreate swap chain!");

        vkDeviceWaitIdle(vk_device_);

        recreate_swap_chain();
        recreate_frame_buffer();

        current_frame_ = 0;
        frame_index_ = 0;

        result = vkAcquireNextImageKHR(vk_device_, vk_swap_chain_,
                                       UINT64_MAX,
                                       present_semaphore_[frame_index_],
                                       VK_NULL_HANDLE, &current_frame_);
        assert(result == VK_SUCCESS);
    } else if (result != VK_SUCCESS) {
        assert(false);
    }

    VkCommandBuffer current_cmd = cmd_buffers_[current_frame_];
    vkResetCommandBuffer(current_cmd, 0);

    VkCommandBufferBeginInfo cmd_begin_info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    cmd_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(current_cmd, &cmd_begin_info) != VK_SUCCESS) {
        __android_log_print(ANDROID_LOG_ERROR, "SkityVK",
                            "Failed to begin cmd buffer at index : %d", current_frame_);
        return;
    }

    std::vector<VkClearValue> clear_values{3};
    clear_values[0].color = {clear_color_[0], clear_color_[1], clear_color_[2],
                             clear_color_[3]};
    clear_values[1].depthStencil = {0.f, 0};
    clear_values[2].color = {clear_color_[0], clear_color_[1], clear_color_[2],
                             clear_color_[3]};

    VkRenderPassBeginInfo render_pass_begin_info{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    render_pass_begin_info.renderPass = vk_render_pass_;
    render_pass_begin_info.framebuffer = swap_chain_frame_buffers_[current_frame_];
    render_pass_begin_info.renderArea.offset = {0, 0};
    render_pass_begin_info.renderArea.extent = swap_chain_extend_;
    render_pass_begin_info.clearValueCount = clear_values.size();
    render_pass_begin_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(current_cmd, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    onDraw(canvas_.get());

    canvas_->flush();

    vkCmdEndRenderPass(current_cmd);

    CALL_VK(vkEndCommandBuffer(current_cmd));

    VkPipelineStageFlags submit_pipeline_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                                  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

    VkSubmitInfo submit_info{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit_info.pWaitDstStageMask = &submit_pipeline_stages;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &present_semaphore_[frame_index_];
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &render_semaphore_[frame_index_];
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &current_cmd;

    vkResetFences(vk_device_, 1, &cmd_fences_[current_frame_]);

    CALL_VK(vkQueueSubmit(vk_present_queue_, 1, &submit_info, cmd_fences_[current_frame_]));

    CALL_VK(vkWaitForFences(vk_device_, 1, &cmd_fences_[current_frame_], VK_TRUE,
                            std::numeric_limits<uint64_t>::max()));

    VkPresentInfoKHR present_info{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &vk_swap_chain_;
    present_info.pImageIndices = &current_frame_;
    present_info.pWaitSemaphores = &render_semaphore_[frame_index_];
    present_info.waitSemaphoreCount = 1;

    result = vkQueuePresentKHR(vk_present_queue_, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        __android_log_print(ANDROID_LOG_ERROR, "SkityVK",
                            "need to handle window resize of recreate swap chain!");
        vkDeviceWaitIdle(vk_device_);

        recreate_swap_chain();
        recreate_frame_buffer();

        frame_index_ = 0;
        current_frame_ = 0;
        return;
    }


    frame_index_++;
    frame_index_ = frame_index_ % swap_chain_image_view_.size();
}

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
    create_render_pass();
    create_frame_buffer();
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

    // Enable just the Khronos validation layer.
    static const char *layers[] = {"VK_LAYER_KHRONOS_validation"};
    // Get the layer count using a null pointer as the last parameter.
    uint32_t instance_layer_present_count = 0;
    vkEnumerateInstanceLayerProperties(&instance_layer_present_count, nullptr);

    // Enumerate layers with a valid pointer in the last parameter.
    VkLayerProperties layer_props[instance_layer_present_count];
    vkEnumerateInstanceLayerProperties(&instance_layer_present_count, layer_props);

    // Make sure selected validation layers are available.
    VkLayerProperties *layer_props_end = layer_props + instance_layer_present_count;
    for (const char *layer:layers) {
        assert(layer_props_end !=
               std::find_if(layer_props, layer_props_end,
                            [layer](VkLayerProperties layerProperties) {
                                return strcmp(layerProperties.layerName, layer) == 0;
                            }));
    }


    VkInstanceCreateInfo create_info{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = instance_ext.size();
    create_info.ppEnabledExtensionNames = instance_ext.data();
    // validation layer cause performance issue
//    create_info.enabledLayerCount = 1;
//    create_info.ppEnabledLayerNames = layers;

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
    create_info.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    CALL_VK(vkCreateSwapchainKHR(vk_device_, &create_info, nullptr, &vk_swap_chain_));

    swap_chain_format_ = format;
    swap_chain_extend_ = surface_caps.currentExtent;
    surface_composite_ = surface_composite;
    vk_surface_transform_ = surface_caps.currentTransform;
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
        img_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
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

void VkRenderer::create_render_pass() {
    std::array<VkAttachmentDescription, 3> attachments = {};
    // color attachment
    attachments[0].format = swap_chain_format_;
    attachments[0].samples = vk_sample_count_;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // depth stencil attachment
    attachments[1].format = stencil_image_[0].format;
    attachments[1].samples = vk_sample_count_;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // color resolve attachment
    attachments[2].format = swap_chain_format_;
    attachments[2].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[2].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_reference{};
    color_reference.attachment = 0;
    color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_stencil_reference{};
    depth_stencil_reference.attachment = 1;
    depth_stencil_reference.layout =
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference resolve_reference{};
    resolve_reference.attachment = 2;
    resolve_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_reference;
    subpass.pDepthStencilAttachment = &depth_stencil_reference;
    subpass.pResolveAttachments = &resolve_reference;

    std::array<VkSubpassDependency, 2> subpass_dependencies{};
    subpass_dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpass_dependencies[0].dstSubpass = 0;
    subpass_dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subpass_dependencies[0].dstStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    subpass_dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpass_dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    subpass_dependencies[1].srcSubpass = 0;
    subpass_dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    subpass_dependencies[1].srcStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subpass_dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpass_dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    subpass_dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo create_info{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    create_info.attachmentCount = attachments.size();
    create_info.pAttachments = attachments.data();
    create_info.subpassCount = 1;
    create_info.pSubpasses = &subpass;
    create_info.dependencyCount = subpass_dependencies.size();
    create_info.pDependencies = subpass_dependencies.data();

    CALL_VK(vkCreateRenderPass(vk_device_, &create_info, nullptr, &vk_render_pass_));
}

void VkRenderer::create_frame_buffer() {
    swap_chain_frame_buffers_.resize(swap_chain_image_view_.size());

    std::array<VkImageView, 3> attachments = {};

    for (size_t i = 0; i < swap_chain_frame_buffers_.size(); i++) {
        attachments[0] = sampler_image_[i].image_view;
        attachments[1] = stencil_image_[i].image_view;
        attachments[2] = swap_chain_image_view_[i];
        VkFramebufferCreateInfo create_info{
                VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
        create_info.renderPass = vk_render_pass_;
        create_info.attachmentCount = attachments.size();
        create_info.pAttachments = attachments.data();
        create_info.width = swap_chain_extend_.width;
        create_info.height = swap_chain_extend_.height;
        create_info.layers = 1;

        CALL_VK(vkCreateFramebuffer(vk_device_, &create_info, nullptr,
                                    &swap_chain_frame_buffers_[i]));
    }
}

void VkRenderer::destroy_swap_chain_views() {
    for (auto fb : swap_chain_frame_buffers_) {
        vkDestroyFramebuffer(vk_device_, fb, nullptr);
    }
    swap_chain_frame_buffers_.clear();

    for (auto const &st : stencil_image_) {
        vkDestroyImageView(vk_device_, st.image_view, nullptr);
        vkDestroyImage(vk_device_, st.image, nullptr);
        vkFreeMemory(vk_device_, st.memory, nullptr);
    }
    stencil_image_.clear();

    for (auto const &si : sampler_image_) {
        vkDestroyImageView(vk_device_, si.image_view, nullptr);
        vkDestroyImage(vk_device_, si.image, nullptr);
        vkFreeMemory(vk_device_, si.memory, nullptr);
    }
    sampler_image_.clear();

    for (auto image_view : swap_chain_image_view_) {
        vkDestroyImageView(vk_device_, image_view, nullptr);
    }
    swap_chain_image_view_.clear();
}

void VkRenderer::recreate_swap_chain() {
    VkSwapchainKHR old_swap_chain = vk_swap_chain_;

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_phy_device_, vk_surface_, &capabilities);
    pretransform_flag_ = capabilities.currentTransform;

    uint32_t width = capabilities.currentExtent.width;
    uint32_t height = capabilities.currentExtent.height;
    if (capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR ||
        capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
        // Swap to get identity width and height
        capabilities.currentExtent.height = width;
        capabilities.currentExtent.width = height;
    }

    swap_chain_extend_ = capabilities.currentExtent;


    VkSwapchainCreateInfoKHR create_info{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    create_info.surface = vk_surface_;
    create_info.minImageCount = std::max(uint32_t(2), capabilities.minImageCount);
    create_info.imageFormat = swap_chain_format_;
    create_info.imageExtent = capabilities.currentExtent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.queueFamilyIndexCount = 1;
    create_info.pQueueFamilyIndices = &present_queue_index_;
    create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    create_info.compositeAlpha = surface_composite_;
    create_info.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    create_info.oldSwapchain = VK_NULL_HANDLE;
    create_info.preTransform = pretransform_flag_;
    create_info.oldSwapchain = old_swap_chain;

    CALL_VK(vkCreateSwapchainKHR(vk_device_, &create_info, nullptr, &vk_swap_chain_));
    vk_surface_transform_ = capabilities.currentTransform;

    if (old_swap_chain) {
        vkDestroySwapchainKHR(vk_device_, old_swap_chain, nullptr);
    }
}

void VkRenderer::recreate_frame_buffer() {
    destroy_swap_chain_views();
    create_swap_chain_views();
    create_frame_buffer();
}

VkInstance VkRenderer::GetInstance() {
    return vk_instance_;
}

VkPhysicalDevice VkRenderer::GetPhysicalDevice() {
    return vk_phy_device_;
}

VkDevice VkRenderer::GetDevice() {
    return vk_device_;
}

VkExtent2D VkRenderer::GetFrameExtent() {
    return swap_chain_extend_;
}

VkCommandBuffer VkRenderer::GetCurrentCMD() {
    return cmd_buffers_[current_frame_];
}

VkRenderPass VkRenderer::GetRenderPass() {
    return vk_render_pass_;
}

PFN_vkGetInstanceProcAddr VkRenderer::GetInstanceProcAddr() {
    return vkGetInstanceProcAddr;
}

uint32_t VkRenderer::GetSwapchainBufferCount() {
    return swap_chain_image_view_.size();
}

uint32_t VkRenderer::GetCurrentBufferIndex() {
    return current_frame_;
}

VkQueue VkRenderer::GetGraphicQueue() {
    return vk_graphic_queue_;
}

VkQueue VkRenderer::GetComputeQueue() {
    return vk_compute_queue_;
}

uint32_t VkRenderer::GetGraphicQueueIndex() {
    return graphic_queue_index_;
}

uint32_t VkRenderer::GetComputeQueueIndex() {
    return compute_queue_index_;
}

VkSampleCountFlagBits VkRenderer::GetSampleCount() {
    return vk_sample_count_;
}

VkFormat VkRenderer::GetDepthStencilFormat() {
    return depth_stencil_format_;
}

VkSurfaceTransformFlagBitsKHR VkRenderer::GetSurfaceTransform() {
    return vk_surface_transform_;
}
