
#ifndef SKITY_ANDROID_VK_RENDERER_HPP
#define SKITY_ANDROID_VK_RENDERER_HPP

#include <volk.h>
#include <skity/skity.hpp>
#include <skity/gpu/gpu_vk_context.hpp>
#include <vector>
#include <array>
#include <android/native_window.h>

struct ImageWrapper {
    VkImage image = {};
    VkImageView image_view = {};
    VkDeviceMemory memory = {};
    VkFormat format = {};
};

class VkRenderer : public skity::GPUVkContext {
public:
    VkRenderer() : skity::GPUVkContext((void *) vkGetDeviceProcAddr) {}

    virtual ~VkRenderer() = default;

    void init(int w, int h, int d, ANativeWindow *window);

    void destroy();

    void draw();

    void set_default_typeface(std::shared_ptr<skity::Typeface> typeface);

    void set_clear_color(float r, float g, float b, float a) {
        clear_color_[0] = r;
        clear_color_[1] = g;
        clear_color_[2] = b;
        clear_color_[3] = a;
    }

    VkInstance GetInstance() override;

    VkPhysicalDevice GetPhysicalDevice() override;

    VkDevice GetDevice() override;

    VkExtent2D GetFrameExtent() override;

    VkCommandBuffer GetCurrentCMD() override;

    VkRenderPass GetRenderPass() override;

    PFN_vkGetInstanceProcAddr GetInstanceProcAddr() override;

    uint32_t GetSwapchainBufferCount() override;

    uint32_t GetCurrentBufferIndex() override;

    VkQueue GetGraphicQueue() override;

    VkQueue GetComputeQueue() override;

    uint32_t GetGraphicQueueIndex() override;

    uint32_t GetComputeQueueIndex() override;

    VkSampleCountFlagBits GetSampleCount() override;

    VkFormat GetDepthStencilFormat() override;

    VkSurfaceTransformFlagBitsKHR GetSurfaceTransform() override;

protected:
    virtual void onDraw(skity::Canvas *canvas) {}

    skity::Canvas *GetCanvas() { return canvas_.get(); }

    int32_t Width() const { return width_; }

    int32_t Height() const { return height_; }

    int32_t Density() const { return density_; }

private:
    void init_vk(ANativeWindow *window);

    void create_vk_instance();

    void pick_phy_device();

    void create_device();

    void create_vk_surface(ANativeWindow *window);

    void create_swap_chain();

    uint32_t get_memory_type(uint32_t type_bits,
                             VkMemoryPropertyFlags properties);

    void create_swap_chain_views();

    void create_command_pool();

    void create_command_buffers();

    void create_sync_objects();

    void create_render_pass();

    void create_frame_buffer();

    void destroy_swap_chain_views();

    void recreate_swap_chain();

    void recreate_frame_buffer();

private:
    int32_t width_ = {};
    int32_t height_ = {};
    int32_t density_ = {};
    ANativeWindow *window_ = {};
    std::unique_ptr<skity::Canvas> canvas_ = {};
    std::array<float, 4> clear_color_ = {};
    VkInstance vk_instance_ = {};
    VkPhysicalDevice vk_phy_device_ = {};
    uint32_t graphic_queue_index_ = -1;
    uint32_t present_queue_index_ = -1;
    uint32_t compute_queue_index_ = -1;
    VkSampleCountFlagBits vk_sample_count_ = VK_SAMPLE_COUNT_1_BIT;
    VkDevice vk_device_ = {};
    VkQueue vk_graphic_queue_ = {};
    VkQueue vk_present_queue_ = {};
    VkQueue vk_compute_queue_ = {};
    VkSurfaceKHR vk_surface_ = {};
    VkSurfaceTransformFlagBitsKHR vk_surface_transform_ = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    VkSwapchainKHR vk_swap_chain_ = {};
    VkFormat swap_chain_format_ = {};
    VkFormat depth_stencil_format_{};
    VkSurfaceTransformFlagBitsKHR pretransform_flag_ = {};
    VkCompositeAlphaFlagBitsKHR surface_composite_ = {};
    VkExtent2D swap_chain_extend_ = {};
    std::vector<VkImageView> swap_chain_image_view_ = {};
    std::vector<ImageWrapper> stencil_image_ = {};
    std::vector<ImageWrapper> sampler_image_ = {};
    VkCommandPool cmd_pool_ = {};
    std::vector<VkCommandBuffer> cmd_buffers_ = {};
    std::vector<VkFence> cmd_fences_ = {};
    std::vector<VkSemaphore> present_semaphore_ = {};
    std::vector<VkSemaphore> render_semaphore_ = {};
    VkRenderPass vk_render_pass_ = {};
    std::vector<VkFramebuffer> swap_chain_frame_buffers_ = {};
    uint32_t current_frame_ = {};
    uint32_t frame_index_ = {};
};

#endif //SKITY_ANDROID_VK_RENDERER_HPP
