
#include "renderer.hpp"

#include <GLES3/gl3.h>
#include <EGL/egl.h>


void Renderer::init(int w, int h, int d) {
    width_ = w;
    height_ = h;
    density_ = d;

    init_gl();

    skity::GPUContext ctx{skity::GPUBackendType::kOpenGL, (void *) eglGetProcAddress};
    canvas_ = skity::Canvas::MakeHardwareAccelationCanvas(width_, height_, density_, &ctx);
}

void Renderer::init_gl() {
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClearStencil(0x0);
    glStencilMask(0xFF);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::set_default_typeface(std::shared_ptr<skity::Typeface> typeface) {
    canvas_->setDefaultTypeface(std::move(typeface));
}
