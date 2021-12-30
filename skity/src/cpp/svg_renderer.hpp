
#ifndef SKITY_ANDROID_SVG_RENDERER_HPP
#define SKITY_ANDROID_SVG_RENDERER_HPP

#include "renderer.hpp"
#include "skity/svg/svg_dom.hpp"

class SVGRenderer : public Renderer {
public:
    SVGRenderer() = default;

    ~SVGRenderer() override = default;

    void draw() override;

    void init_svg(skity::Data *data) {
        svg_dom_ = skity::SVGDom::MakeFromData(data);
    }

private:
    std::unique_ptr<skity::SVGDom> svg_dom_ = {};
};



#endif //SKITY_ANDROID_SVG_RENDERER_HPP
