
#ifndef SKITY_ANDROID_VK_FRAME_RENDERER_HPP
#define SKITY_ANDROID_VK_FRAME_RENDERER_HPP

#include "vk_renderer.hpp"
#include "perf.hpp"

class VkFrameRenderer : public VkRenderer {
public:
    VkFrameRenderer() :fpsGraph(Perf::GRAPH_RENDER_FPS, "Frame Time"),
                       cpuGraph(Perf::GRAPH_RENDER_MS, "CPU Time") {}
    ~VkFrameRenderer() override = default;

    void init_render_typeface(std::shared_ptr<skity::Typeface> typeface,
                              std::shared_ptr<skity::Typeface> emoji);

    void init_images(std::vector<std::shared_ptr<skity::Pixmap>> images);

protected:
    void onDraw(skity::Canvas *canvas) override;

private:
    std::shared_ptr<skity::Typeface> render_typeface_ = {};
    std::shared_ptr<skity::Typeface> emoji_typeface_ = {};
    std::vector<std::shared_ptr<skity::Pixmap>> render_images_ = {};
    double time_ = {};
    double start_time_ = {};
    double prev_time_ = {};
    double cpu_time_ = {};
    Perf fpsGraph;
    Perf cpuGraph;
};


#endif //SKITY_ANDROID_VK_FRAME_RENDERER_HPP
