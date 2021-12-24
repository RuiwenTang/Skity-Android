#include "skity/skity.hpp"

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <jni.h>

static void draw_basic_example(skity::Canvas* canvas) {
    skity::Paint paint;
    paint.setStyle(skity::Paint::kFill_Style);
    paint.setAntiAlias(true);
    paint.setStrokeWidth(4.f);
    paint.SetFillColor(0x42 / 255.f, 0x85 / 255.f, 0xF4 / 255.f, 1.f);

    skity::Rect rect = skity::Rect::MakeXYWH(10, 10, 100, 160);
    canvas->drawRect(rect, paint);

    skity::RRect oval;
    oval.setOval(rect);
    oval.offset(40, 80);
    paint.SetFillColor(0xDB / 255.f, 0x44 / 255.f, 0x37 / 255.f, 1.f);
    canvas->drawRRect(oval, paint);

    paint.SetFillColor(0x0F / 255.f, 0x9D / 255.f, 0x58 / 255.f, 1.f);
    canvas->drawCircle(180, 50, 25, paint);

    rect.offset(80, 50);
    paint.SetStrokeColor(0xF4 / 255.f, 0xB4 / 255.f, 0x0, 1.f);
    paint.setStyle(skity::Paint::kStroke_Style);
    canvas->drawRoundRect(rect, 10, 10, paint);
}

// same as https://fiddle.skia.org/c/@discrete_path
static void draw_path_effect_example(skity::Canvas* canvas) {
    const float R = 115.2f, C = 128.f;
    skity::Path path;
    path.moveTo(C + R, C);
    for (int32_t i = 1; i < 8; i++) {
        float a = 2.6927937f * i;
        path.lineTo(C + R * std::cos(a), C + R * std::sin(a));
    }

    skity::Paint paint;
    paint.setPathEffect(skity::PathEffect::MakeDiscretePathEffect(10.f, 4.f));
    paint.setStyle(skity::Paint::kStroke_Style);
    paint.setStrokeWidth(2.f);
    paint.setAntiAlias(true);
    paint.SetStrokeColor(0x42 / 255.f, 0x85 / 255.f, 0xF4 / 255.f, 1.f);
    canvas->drawPath(path, paint);
}

static void draw_dash_start_example(skity::Canvas* canvas) {
    skity::Path path;
    path.moveTo(199, 34);
    path.lineTo(253, 143);
    path.lineTo(374, 160);
    path.lineTo(287, 244);
    path.lineTo(307, 365);
    path.lineTo(199, 309);
    path.lineTo(97, 365);
    path.lineTo(112, 245);
    path.lineTo(26, 161);
    path.lineTo(146, 143);
    path.close();

    skity::Paint paint;
    paint.setStrokeWidth(3.f);
    paint.setStrokeJoin(skity::Paint::kRound_Join);
    paint.setStrokeCap(skity::Paint::kRound_Cap);
    paint.SetStrokeColor(0, 0, 1, 1);
    paint.SetFillColor(150.f / 255.f, 150.f / 255.f, 1.f, 1.f);
    paint.setAntiAlias(true);
    paint.setStyle(skity::Paint::kStrokeAndFill_Style);
    float pattern[2] = {10.f, 10.f};
    paint.setPathEffect(skity::PathEffect::MakeDashPathEffect(pattern, 2, 0));

    canvas->drawPath(path, paint);
}

// same as https://fiddle.skia.org/c/844ab7d5e63876f6c889b33662ece8d5
void draw_linear_gradient_example(skity::Canvas* canvas) {
    skity::Paint p;
    p.setStyle(skity::Paint::kFill_Style);

    skity::Vec4 colors[] = {
            skity::Vec4{0.f, 1.f, 1.f, 0.f},
            skity::Vec4{0.f, 0.f, 1.f, 1.f},
            skity::Vec4{1.f, 0.f, 0.f, 1.f},
    };
    float positions[] = {0.f, 0.65f, 1.f};

    for (int i = 0; i < 4; i++) {
        float blockX = (i % 2) * 100.f;
        float blockY = (i / 2) * 100.f;

        std::vector<skity::Point> pts = {
                skity::Point{blockX, blockY, 0.f, 1.f},
                skity::Point{blockX + 50, blockY + 100, 0.f, 1.f},
        };

        skity::Matrix matrix = glm::identity<skity::Matrix>();
        int flag = 0;
        if (i % 2 == 1) {
            flag = 1;
        }
        if (i / 2 == 1) {
            matrix *= glm::translate(glm::identity<glm::mat4>(),
                                     glm::vec3(blockX, blockY, 0));
            matrix *= glm::rotate(glm::identity<glm::mat4>(), glm::radians(45.f),
                                  glm::vec3(0, 0, 1));
            matrix *= glm::translate(glm::identity<glm::mat4>(),
                                     glm::vec3(-blockX, -blockY, 0));
        }
        auto lgs =
                skity::Shader::MakeLinear(pts.data(), colors, positions, 3, flag);
        lgs->SetLocalMatrix(matrix);
        p.setShader(lgs);
        auto r = skity::Rect::MakeLTRB(blockX, blockY, blockX + 100, blockY + 100);
        canvas->drawRect(r, p);
    }

    skity::Path circle;
    circle.addCircle(220, 350, 100);
    skity::Paint paint;
    paint.setStyle(skity::Paint::kFill_Style);
    paint.setAntiAlias(true);
    skity::Point center{220, 350, 0, 1};
    skity::Vec4 radialColors[] = {skity::Vec4{1.f, 1.f, 1.f, 1.f},
                                  skity::Vec4{0.f, 0.f, 0.f, 1.f}};
    float pts[] = {0.f, 1.f};
    auto rgs = skity::Shader::MakeRadial(center, 150.f, radialColors, pts, 2);
    paint.setShader(rgs);

    canvas->drawPath(circle, paint);
}

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

    auto canvas = canvas_.get();

    draw_basic_example(canvas);

    canvas->save();
    canvas->translate(300, 0);
    draw_path_effect_example(canvas);
    canvas->restore();

    canvas->save();
    canvas->translate(0, 300);
    draw_dash_start_example(canvas);
    canvas->restore();

//    canvas->save();
//    canvas->translate(520, 0);
//    draw_simple_text(canvas);
//    canvas->restore();

    canvas->save();
    canvas->translate(400, 300);
    draw_linear_gradient_example(canvas);
    canvas->restore();

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