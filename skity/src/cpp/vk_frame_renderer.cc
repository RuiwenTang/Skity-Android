
#include "vk_frame_renderer.hpp"

void render_frame_demo(
        skity::Canvas *canvas,
        std::vector<std::shared_ptr<skity::Pixmap>> const &images,
        std::shared_ptr<skity::Typeface> const &typeface,
        std::shared_ptr<skity::Typeface> const &emoji, float mx, float my,
        float width, float height, float t);

static double skity_get_time() {
    struct timespec res = {};
    clock_gettime(CLOCK_REALTIME, &res);

    return res.tv_sec + (double) res.tv_nsec / (double) 1e9;
}

void VkFrameRenderer::onDraw(skity::Canvas *canvas) {
    time_ = skity_get_time();

    double dt = time_ - prev_time_;
    prev_time_ = time_;

    render_frame_demo(GetCanvas(), render_images_, render_typeface_, emoji_typeface_, 0.f, 0.f,
                      Width(), Height(),
                      static_cast<float>(time_ - start_time_));

    cpu_time_ = skity_get_time() - time_;
    fpsGraph.RenderGraph(GetCanvas(), 5, 5);
    cpuGraph.RenderGraph(GetCanvas(), 5 + 200 + 5, 5);

    fpsGraph.UpdateGraph(dt);
    cpuGraph.UpdateGraph(cpu_time_);
}

void VkFrameRenderer::init_render_typeface(std::shared_ptr<skity::Typeface> typeface,
                                           std::shared_ptr<skity::Typeface> emoji) {
    render_typeface_ = std::move(typeface);
    emoji_typeface_ = std::move(emoji);

    start_time_ = time_ = prev_time_ = skity_get_time();
}

void VkFrameRenderer::init_images(std::vector<std::shared_ptr<skity::Pixmap>> images) {
    render_images_ = std::move(images);
}