
#ifndef SKITY_ANDROID_FRAME_RENDERER_HPP
#define SKITY_ANDROID_FRAME_RENDERER_HPP

#include "renderer.hpp"
#include "perf.hpp"

#include <vector>
#include <memory>

class FrameRender : public Renderer {
public:
    FrameRender();

    ~FrameRender() override = default;


    void init_render_typeface(std::shared_ptr<skity::Typeface> typeface);

    void init_images(std::vector<std::shared_ptr<skity::Pixmap>> images);

protected:
    void draw() override;

private:
    std::shared_ptr<skity::Typeface> render_typeface_ = {};
    std::vector<std::shared_ptr<skity::Pixmap>> render_images_ = {};
    double time_ = {};
    double start_time_ = {};
    double prev_time_ = {};
    double cpu_time_ = {};
    Perf fpsGraph;
    Perf cpuGraph;
};


#endif //SKITY_ANDROID_FRAME_RENDERER_HPP
