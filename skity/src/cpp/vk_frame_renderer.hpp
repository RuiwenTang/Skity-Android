
#ifndef SKITY_ANDROID_VK_FRAME_RENDERER_HPP
#define SKITY_ANDROID_VK_FRAME_RENDERER_HPP

#include "vk_renderer.hpp"

class VkFrameRenderer : public VkRenderer {
public:
    VkFrameRenderer() = default;
    ~VkFrameRenderer() override = default;

protected:
    void onDraw(skity::Canvas *canvas) override;
};


#endif //SKITY_ANDROID_VK_FRAME_RENDERER_HPP
