
#include "renderer.hpp"

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <android/log.h>
#include <cstdio>

static const char *kTAG = "SkityGL";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, kTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))

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

    const char* version;
    std::vector<const char*> prefixes = {
            "OpenGL ES-CM ",
            "OpenGL ES-CL ",
            "OpenGL ES ",
    };

    version = (const char*) glGetString(GL_VERSION);

    LOGI("glVersion = %s", version);

    for(auto pref : prefixes) {
        size_t length = std::strlen(pref);
        if (std::strncmp(version, pref, length) == 0) {
            version += length;
            break;
        }
    }

    int major, minor;
    std::sscanf(version, "%d.%d", &major, &minor);

    LOGI("major = %d | minor = %d", major, minor);
}

void Renderer::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::set_default_typeface(std::shared_ptr<skity::Typeface> typeface) {
    canvas_->setDefaultTypeface(std::move(typeface));
}
