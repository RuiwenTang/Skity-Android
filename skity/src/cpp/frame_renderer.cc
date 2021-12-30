
#include "frame_renderer.hpp"

#include <GLES3/gl3.h>
#include <time.h>

#include <utility>

double skity_get_time() {
    struct timespec res = {};
    clock_gettime(CLOCK_REALTIME, &res);

    return res.tv_sec + (double) res.tv_nsec / (double) 1e9;
}

void render_frame_demo(
        skity::Canvas *canvas,
        std::vector<std::shared_ptr<skity::Pixmap>> const &images,
        std::shared_ptr<skity::Typeface> const &typeface, float mx, float my,
        float width, float height, float t);

FrameRender::FrameRender() : Renderer(),
                             fpsGraph(Perf::GRAPH_RENDER_FPS, "Frame Time"),
                             cpuGraph(Perf::GRAPH_RENDER_MS, "CPU Time") {}

void FrameRender::init_render_typeface(std::shared_ptr<skity::Typeface> typeface) {
    render_typeface_ = std::move(typeface);

    start_time_ = time_ = prev_time_ = skity_get_time();
}

void FrameRender::init_images(std::vector<std::shared_ptr<skity::Pixmap>> images) {
    render_images_ = std::move(images);

    glClearColor(0.3f, 0.3f, 0.32f, 1.f);
}

void FrameRender::draw() {
    Renderer::draw();

    time_ = skity_get_time();

    double dt = time_ - prev_time_;
    prev_time_ = time_;

    render_frame_demo(GetCanvas(), render_images_, render_typeface_, 0.f, 0.f, Width(), Height(),
                      static_cast<float>(time_ - start_time_));

    cpu_time_ = skity_get_time() - time_;
    fpsGraph.RenderGraph(GetCanvas(), 5, 5);
    cpuGraph.RenderGraph(GetCanvas(), 5 + 200 + 5, 5);

    GetCanvas()->flush();

    fpsGraph.UpdateGraph(dt);
    cpuGraph.UpdateGraph(cpu_time_);
}