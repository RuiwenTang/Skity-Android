
#include "svg_renderer.hpp"

void SVGRenderer::draw() {
    Renderer::draw();

    GetCanvas()->save();
    GetCanvas()->translate(50, 50);

    svg_dom_->Render(GetCanvas());

    GetCanvas()->restore();

    GetCanvas()->flush();
}