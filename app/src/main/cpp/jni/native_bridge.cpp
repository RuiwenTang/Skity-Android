#include <jni.h>

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
      "Native bridge loaded. Real skity rendering is active for GLES previews. "
      "The Vulkan preview path is still waiting on Android presenter wiring.";
  return env->NewStringUTF(summary.c_str());
}

extern "C" JNIEXPORT jlong JNICALL
Java_org_lynxsdk_lynx_skity_dev_SkityNative_nativeCreateRenderer(
    JNIEnv* env, jclass clazz) {
  (void)env;
  (void)clazz;
  return ToHandle(std::make_unique<skity::demo::AppRenderer>());
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
