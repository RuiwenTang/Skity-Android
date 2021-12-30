
#ifndef SKITY_ANDROID_STATIC_RENDERER_HPP
#define SKITY_ANDROID_STATIC_RENDERER_HPP

#include "renderer.hpp"

class StaticRenderer : public Renderer {
public:
    StaticRenderer() = default;

    ~StaticRenderer() override = default;


protected:
    void draw() override;

};


#endif //SKITY_ANDROID_STATIC_RENDERER_HPP
