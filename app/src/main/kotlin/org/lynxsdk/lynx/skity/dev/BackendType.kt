package org.lynxsdk.lynx.skity.dev

enum class BackendType(val title: String, val description: String) {
    AUTO("Auto", "Choose the best available backend on the current device."),
    GLES("GLES", "Validate the OpenGL ES rendering path."),
    VULKAN("Vulkan", "Validate the Vulkan rendering path.")
}
