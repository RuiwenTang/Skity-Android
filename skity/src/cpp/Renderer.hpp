
#ifndef SKITY_ANDROID_RENDERER_HPP
#define SKITY_ANDROID_RENDERER_HPP

#include "skity/skity.hpp"
#include "skity/gpu/gpu_context.hpp"

class Renderer {
public:
    Renderer() = default;

    virtual ~Renderer() = default;

    void init(int w, int h, int d);

    virtual void draw();

    void set_default_typeface(std::shared_ptr<skity::Typeface> typeface);

protected:
    skity::Canvas *GetCanvas() { return canvas_.get(); }

    int32_t Width() const { return width_; }

    int32_t Height() const { return height_; }

    int32_t Density() const { return density_; }

private:
    void init_gl();

private:
    int32_t width_ = {};
    int32_t height_ = {};
    int32_t density_ = {};
    std::unique_ptr<skity::Canvas> canvas_ = {};
};

#endif //SKITY_ANDROID_RENDERER_HPP
