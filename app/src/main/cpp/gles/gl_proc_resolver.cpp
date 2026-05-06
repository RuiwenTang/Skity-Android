#include "gles/gl_proc_resolver.hpp"

#include <EGL/egl.h>
#include <dlfcn.h>

namespace skity::demo {

void* ResolveGLProcAddress(const char* name) {
  if (name == nullptr) {
    return nullptr;
  }

  void* proc = reinterpret_cast<void*>(eglGetProcAddress(name));
  if (proc != nullptr) {
    return proc;
  }

  proc = dlsym(RTLD_DEFAULT, name);
  if (proc != nullptr) {
    return proc;
  }

  static void* gles3 = dlopen("libGLESv3.so", RTLD_NOW | RTLD_LOCAL);
  if (gles3 != nullptr) {
    proc = dlsym(gles3, name);
    if (proc != nullptr) {
      return proc;
    }
  }

  static void* gles2 = dlopen("libGLESv2.so", RTLD_NOW | RTLD_LOCAL);
  if (gles2 != nullptr) {
    proc = dlsym(gles2, name);
  }

  return proc;
}

}  // namespace skity::demo
