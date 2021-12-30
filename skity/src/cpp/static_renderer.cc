
#include "static_renderer.hpp"


void draw_canvas(skity::Canvas *canvas);


void StaticRenderer::draw() {
    Renderer::draw();

    auto canvas = GetCanvas();

    draw_canvas(canvas);

    canvas->flush();
}