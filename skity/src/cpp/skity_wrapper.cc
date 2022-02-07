#include "skity/skity.hpp"
#include "skity/svg/svg_dom.hpp"

#include "renderer.hpp"
#include "vk_renderer.hpp"
#include "static_renderer.hpp"
#include "svg_renderer.hpp"
#include "frame_renderer.hpp"

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/native_window_jni.h>
#include <android/bitmap.h>

#include <utility>

#define SKITY_DEFAULT_FONT "Roboto Mono Nerd Font Complete.ttf"

extern "C"
JNIEXPORT jlong JNICALL
Java_com_skity_graphic_Renderer_nativeInit(JNIEnv *env, jobject thiz, jint width, jint height,
                                           jint density) {
    auto render = new StaticRenderer;

    render->init(width, height, density);

    return (jlong) render;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_skity_graphic_Renderer_nativeDraw(JNIEnv *env, jobject thiz, jlong handler) {
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

extern "C"
JNIEXPORT void JNICALL
Java_com_skity_graphic_Renderer_nativeLoadDefaultAssets(JNIEnv *env, jobject thiz, jlong handler,
                                                        jobject asset_manager) {
    auto render = (Renderer *) handler;
    auto am = AAssetManager_fromJava(env, asset_manager);

    auto font_asset = AAssetManager_open(am, SKITY_DEFAULT_FONT, AASSET_MODE_BUFFER);

    if (!font_asset) {
        return;
    }

    const void *buf = AAsset_getBuffer(font_asset);
    ssize_t length = AAsset_getLength(font_asset);

    auto font_data = skity::Data::MakeWithCopy(buf, length);

    render->set_default_typeface(skity::Typeface::MakeFromData(font_data));

    AAsset_close(font_asset);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_skity_graphic_GLSVGRender_nativeInitSVG(JNIEnv *env, jobject thiz, jint width, jint height,
                                                 jint density, jobject context) {
    auto render = new SVGRenderer;

    render->init(width, height, density);

    return (jlong) render;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_skity_graphic_GLSVGRender_nativeLoadSVG(JNIEnv *env, jobject thiz, jlong handler,
                                                 jobject asset_manager) {
    auto svg_render = (SVGRenderer *) handler;

    auto am = AAssetManager_fromJava(env, asset_manager);

    auto svg_asset = AAssetManager_open(am, "images/tiger.svg", AASSET_MODE_BUFFER);

    if (!svg_asset) {
        return;
    }

    const void *buf = AAsset_getBuffer(svg_asset);
    ssize_t length = AAsset_getLength(svg_asset);

    auto svg_data = skity::Data::MakeWithCopy(buf, length);

    svg_render->init_svg(svg_data.get());

    AAsset_close(svg_asset);
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_skity_graphic_GLFrameRender_nativeInitFrame(JNIEnv *env, jobject thiz, jint width,
                                                     jint height, jint density, jobject context) {
    auto render = new FrameRender;

    render->init(width, height, density);

    return (jlong) render;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_skity_graphic_GLFrameRender_nativeInitTypefaces(JNIEnv *env, jobject thiz,
                                                         jlong native_handle,
                                                         jobject asset_manager) {
    auto render = (FrameRender *) native_handle;
    auto am = AAssetManager_fromJava(env, asset_manager);

    auto font_asset = AAssetManager_open(am, SKITY_DEFAULT_FONT, AASSET_MODE_BUFFER);

    if (!font_asset) {
        return;
    }

    const void *buf = AAsset_getBuffer(font_asset);
    ssize_t length = AAsset_getLength(font_asset);

    auto font_data = skity::Data::MakeWithCopy(buf, length);

    render->set_default_typeface(skity::Typeface::MakeFromData(font_data));

    AAsset_close(font_asset);


    font_asset = AAssetManager_open(am, "Roboto-Regular.ttf", AASSET_MODE_BUFFER);

    auto emoji_asset = AAssetManager_open(am, "NotoEmoji-Regular.ttf", AASSET_MODE_BUFFER);

    if (!font_asset || !emoji_asset) {
        return;
    }

    buf = AAsset_getBuffer(font_asset);
    length = AAsset_getLength(font_asset);

    auto emoji_buf = AAsset_getBuffer(emoji_asset);
    auto emoji_buf_length = AAsset_getLength(emoji_asset);

    font_data = skity::Data::MakeWithCopy(buf, length);

    auto emoji_font_data = skity::Data::MakeWithCopy(emoji_buf, emoji_buf_length);

    render->init_render_typeface(skity::Typeface::MakeFromData(font_data),
                                 skity::Typeface::MakeFromData(emoji_font_data));

    AAsset_close(font_asset);
    AAsset_close(emoji_asset);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_skity_graphic_GLFrameRender_nativeInitImages(JNIEnv *env, jobject thiz,
                                                      jlong native_handle, jobject images) {
    auto list_class = env->GetObjectClass(images);
    auto size_method = env->GetMethodID(list_class, "size", "()I");
    auto get_method = env->GetMethodID(list_class, "get", "(I)Ljava/lang/Object;");

    std::vector<std::shared_ptr<skity::Pixmap>> skity_images = {};

    int size = env->CallIntMethod(images, size_method);
    for (int i = 0; i < size; i++) {
        auto bitmap = env->CallObjectMethod(images, get_method, i);

        AndroidBitmapInfo info;
        AndroidBitmap_getInfo(env, bitmap, &info);

        void *addr = nullptr;
        AndroidBitmap_lockPixels(env, bitmap, &addr);

        auto data = skity::Data::MakeWithCopy(addr, info.height * info.stride);

        skity_images.emplace_back(std::make_shared<skity::Pixmap>(
                data, info.stride, info.width, info.height
        ));

        AndroidBitmap_unlockPixels(env, bitmap);
    }

    auto render = (FrameRender *) native_handle;

    render->init_images(skity_images);
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_skity_graphic_VkFrameRender_nativeInit(JNIEnv *env, jobject thiz, jint width, jint height,
                                                jint density, jobject surface) {
    auto render = new VkRenderer();

    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);

    render->init(width, height, density, window);

    return (jlong) render;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_skity_graphic_VkRenderer_nativeLoadDefaultAssets(JNIEnv *env, jobject thiz, jlong handler,
                                                          jobject asset_manager) {
    // TODO: implement nativeLoadDefaultAssets()
}
extern "C"
JNIEXPORT void JNICALL
Java_com_skity_graphic_VkRenderer_nativeDraw(JNIEnv *env, jobject thiz, jlong handler) {
    // TODO: implement nativeDraw()
}
extern "C"
JNIEXPORT void JNICALL
Java_com_skity_graphic_VkRenderer_nativeDestroy(JNIEnv *env, jobject thiz, jlong handler) {
    auto render = (VkRenderer *) handler;

    render->destroy();

    delete render;
}