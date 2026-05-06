#include <jni.h>

#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_org_lynxsdk_lynx_skity_dev_SkityNative_nativeGetStatusSummary(
    JNIEnv* env, jclass clazz) {
  (void)clazz;
  const std::string summary =
      "Native bridge loaded. Preview UI is ready for skity surface "
      "integration and backend-specific rendering hooks.";
  return env->NewStringUTF(summary.c_str());
}
