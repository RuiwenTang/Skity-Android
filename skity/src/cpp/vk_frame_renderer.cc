
#include "vk_frame_renderer.hpp"

void VkFrameRenderer::onDraw(skity::Canvas *canvas) {
    skity::Paint paint;
    paint.setStyle(skity::Paint::kFill_Style);
    paint.setColor(skity::Color_RED);

    canvas->drawRect(skity::Rect::MakeXYWH(10, 10, 300, 300), paint);
}