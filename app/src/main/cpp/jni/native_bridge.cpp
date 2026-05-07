#include <jni.h>

#include <android/native_window.h>
#include <android/native_window_jni.h>

#include <memory>
#include <string>

#include "common/app_renderer.hpp"

namespace {

jlong ToHandle(std::unique_ptr<skity::demo::AppRenderer> renderer) {
  return reinterpret_cast<jlong>(renderer.release());
}

skity::demo::AppRenderer* FromHandle(jlong handle) {
  return reinterpret_cast<skity::demo::AppRenderer*>(handle);
}

}  // namespace

extern "C" JNIEXPORT jstring JNICALL
Java_org_lynxsdk_lynx_skity_dev_SkityNative_nativeGetStatusSummary(
    JNIEnv* env, jclass clazz) {
  (void)clazz;
  const std::string summary =
      "Native bridge loaded. GLES and Vulkan preview paths are both wired. "
      "Vulkan presentation depends on device/runtime support.";
  return env->NewStringUTF(summary.c_str());
}

extern "C" JNIEXPORT jlong JNICALL
Java_org_lynxsdk_lynx_skity_dev_SkityNative_nativeCreateRenderer(
    JNIEnv* env, jclass clazz, jint backend_type) {
  (void)env;
  (void)clazz;
  return ToHandle(std::make_unique<skity::demo::AppRenderer>(backend_type));
}

extern "C" JNIEXPORT void JNICALL
Java_org_lynxsdk_lynx_skity_dev_SkityNative_nativeDestroyRenderer(
    JNIEnv* env, jclass clazz, jlong handle) {
  (void)env;
  (void)clazz;
  delete FromHandle(handle);
}

extern "C" JNIEXPORT void JNICALL
Java_org_lynxsdk_lynx_skity_dev_SkityNative_nativeSetScene(
    JNIEnv* env, jclass clazz, jlong handle, jint scene) {
  (void)env;
  (void)clazz;
  auto* renderer = FromHandle(handle);
  if (renderer != nullptr) {
    renderer->SetScene(scene);
  }
}

extern "C" JNIEXPORT void JNICALL
Java_org_lynxsdk_lynx_skity_dev_SkityNative_nativeSetSurface(
    JNIEnv* env, jclass clazz, jlong handle, jobject surface) {
  (void)clazz;
  auto* renderer = FromHandle(handle);
  if (renderer == nullptr) {
    return;
  }

  if (surface == nullptr) {
    renderer->SetNativeWindow(nullptr);
    return;
  }

  ANativeWindow* native_window = ANativeWindow_fromSurface(env, surface);
  renderer->SetNativeWindow(native_window);
}

extern "C" JNIEXPORT void JNICALL
Java_org_lynxsdk_lynx_skity_dev_SkityNative_nativeOnSurfaceCreated(
    JNIEnv* env, jclass clazz, jlong handle) {
  (void)env;
  (void)clazz;
  auto* renderer = FromHandle(handle);
  if (renderer != nullptr) {
    renderer->OnSurfaceCreated();
  }
}

extern "C" JNIEXPORT void JNICALL
Java_org_lynxsdk_lynx_skity_dev_SkityNative_nativeOnSurfaceDestroyed(
    JNIEnv* env, jclass clazz, jlong handle) {
  (void)env;
  (void)clazz;
  auto* renderer = FromHandle(handle);
  if (renderer != nullptr) {
    renderer->OnSurfaceDestroyed();
  }
}

extern "C" JNIEXPORT void JNICALL
Java_org_lynxsdk_lynx_skity_dev_SkityNative_nativeOnSurfaceChanged(
    JNIEnv* env, jclass clazz, jlong handle, jint width, jint height) {
  (void)env;
  (void)clazz;
  auto* renderer = FromHandle(handle);
  if (renderer != nullptr) {
    renderer->OnSurfaceChanged(width, height);
  }
}

extern "C" JNIEXPORT void JNICALL
Java_org_lynxsdk_lynx_skity_dev_SkityNative_nativeDrawFrame(
    JNIEnv* env, jclass clazz, jlong handle) {
  (void)env;
  (void)clazz;
  auto* renderer = FromHandle(handle);
  if (renderer != nullptr) {
    renderer->DrawFrame();
  }
}
