//
// Created by TangRuiwen on 2022/2/8.
//

#include "vk_svg_renderer.hpp"

void VkSVGRender::onDraw(skity::Canvas *canvas) {
    GetCanvas()->save();
    GetCanvas()->translate(50, 50);

    svg_dom_->Render(GetCanvas());

    GetCanvas()->restore();
}