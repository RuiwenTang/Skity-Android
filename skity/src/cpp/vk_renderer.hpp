
#ifndef SKITY_ANDROID_VK_RENDERER_HPP
#define SKITY_ANDROID_VK_RENDERER_HPP

#include <volk.h>
#include <skity/skity.hpp>
#include <skity/gpu/gpu_vk_context.hpp>
#include <vector>
#include <android/native_window.h>

struct ImageWrapper {
    VkImage image = {};
    VkImageView image_view = {};
    VkDeviceMemory memory = {};
    VkFormat format = {};
};

class VkRenderer {
public:
    VkRenderer() = default;

    virtual ~VkRenderer() = default;

    void init(int w, int h, int d, ANativeWindow *window);

    void destroy();

    virtual void draw();

    void set_default_typeface(std::shared_ptr<skity::Typeface> typeface);

protected:
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

private:
    int32_t width_ = {};
    int32_t height_ = {};
    int32_t density_ = {};
    std::unique_ptr<skity::Canvas> canvas_ = {};

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
    VkSwapchainKHR vk_swap_chain_ = {};
    VkFormat swap_chain_format_ = {};
    VkFormat depth_stencil_format_{};
    VkExtent2D swap_chain_extend_ = {};
    std::vector<VkImageView> swap_chain_image_view_ = {};
    std::vector<ImageWrapper> stencil_image_ = {};
    std::vector<ImageWrapper> sampler_image_ = {};
    VkCommandPool cmd_pool_ = {};
    std::vector<VkCommandBuffer> cmd_buffers_ = {};
    std::vector<VkFence> cmd_fences_ = {};
    std::vector<VkSemaphore> present_semaphore_ = {};
    std::vector<VkSemaphore> render_semaphore_ = {};
};

#endif //SKITY_ANDROID_VK_RENDERER_HPP
