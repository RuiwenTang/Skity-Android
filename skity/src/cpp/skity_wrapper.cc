#include "skity/skity.hpp"

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <jni.h>

class Renderer {
public:
    Renderer() = default;

    ~Renderer() = default;

    void init(int w, int h, int d);

    void draw();

private:
    void init_gl();

private:
    int32_t width_ = {};
    int32_t height_ = {};
    int32_t density_ = {};
    std::unique_ptr<skity::Canvas> canvas_ = {};
};


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

    skity::Paint paint;
    paint.setStyle(skity::Paint::kFill_Style);
    paint.setAntiAlias(true);
    paint.setStrokeWidth(4.f);
    paint.SetFillColor(0x42 / 255.f, 0x85 / 255.f, 0xF4 / 255.f, 1.f);

    skity::Rect rect = skity::Rect::MakeXYWH(10, 10, 100, 160);
    canvas_->drawRect(rect, paint);

    skity::RRect oval;
    oval.setOval(rect);
    oval.offset(40, 80);
    paint.SetFillColor(0xDB / 255.f, 0x44 / 255.f, 0x37 / 255.f, 1.f);
    canvas_->drawRRect(oval, paint);

    paint.SetFillColor(0x0F / 255.f, 0x9D / 255.f, 0x58 / 255.f, 1.f);
    canvas_->drawCircle(180, 50, 25, paint);

    rect.offset(80, 50);
    paint.SetStrokeColor(0xF4 / 255.f, 0xB4 / 255.f, 0x0, 1.f);
    paint.setStyle(skity::Paint::kStroke_Style);
    canvas_->drawRoundRect(rect, 10, 10, paint);

    canvas_->flush();
}


extern "C"
JNIEXPORT jlong JNICALL
Java_com_skity_graphic_Renderer_nativeInit(JNIEnv *env, jobject thiz, jint width, jint height,
                                           jint density) {
    // TODO: implement nativeInit()
    auto render = new Renderer;

    render->init(width, height, density);

    return (jlong) render;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_skity_graphic_Renderer_nativeDraw(JNIEnv *env, jobject thiz, jlong handler) {
    // TODO: implement nativeDraw()
    auto render = (Renderer *) handler;

    render->draw();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_skity_graphic_Renderer_nativeDestroy(JNIEnv *env, jobject thiz, jlong handler) {
    // TODO: implement nativeDestroy()
    auto render = (Renderer *) handler;

    delete render;
}