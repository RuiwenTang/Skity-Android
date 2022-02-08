
#ifndef SKITY_ANDROID_VK_SVG_RENDERER_HPP
#define SKITY_ANDROID_VK_SVG_RENDERER_HPP

#include "vk_renderer.hpp"
#include <skity/svg/svg_dom.hpp>

class VkSVGRender : public VkRenderer {
public:
    VkSVGRender() = default;

    ~VkSVGRender() override = default;

    void init_svg(skity::Data *data) {
        svg_dom_ = skity::SVGDom::MakeFromData(data);
    }

protected:
    void onDraw(skity::Canvas *canvas) override;
private:
    std::unique_ptr<skity::SVGDom> svg_dom_ = {};
};


#endif //SKITY_ANDROID_VK_SVG_RENDERER_HPP
