package org.lynxsdk.lynx.skity.dev

import android.view.Surface

object SkityNative {
    private var loaded = false
    private var loadError = ""

    init {
        try {
            System.loadLibrary("skity-dev")
            loaded = true
        } catch (error: UnsatisfiedLinkError) {
            loadError = error.message ?: "Unknown load error"
        }
    }

    fun isLoaded(): Boolean = loaded

    fun getLoadError(): String = loadError

    fun getStatusSummary(): String {
        if (!loaded) {
            return "Native bridge unavailable: $loadError"
        }
        return nativeGetStatusSummary()
    }

    fun createRenderer(
        backend: BackendType,
        enableVulkanValidation: Boolean = false
    ): Long = nativeCreateRenderer(backend.ordinal, enableVulkanValidation)

    fun destroyRenderer(handle: Long) {
        nativeDestroyRenderer(handle)
    }

    fun setSurface(handle: Long, surface: Surface?) {
        nativeSetSurface(handle, surface)
    }

    fun setScene(handle: Long, scene: Int) {
        nativeSetScene(handle, scene)
    }

    fun setMsaaSampleCount(handle: Long, sampleCount: Int) {
        nativeSetMsaaSampleCount(handle, sampleCount)
    }

    fun onSurfaceCreated(handle: Long) {
        nativeOnSurfaceCreated(handle)
    }

    fun onSurfaceDestroyed(handle: Long) {
        nativeOnSurfaceDestroyed(handle)
    }

    fun onSurfaceChanged(handle: Long, width: Int, height: Int) {
        nativeOnSurfaceChanged(handle, width, height)
    }

    fun drawFrame(handle: Long) {
        nativeDrawFrame(handle)
    }

    fun getRendererOverlay(handle: Long): String {
        if (handle == 0L) {
            return ""
        }
        return nativeGetRendererOverlay(handle)
    }

    @JvmStatic
    private external fun nativeGetStatusSummary(): String

    @JvmStatic
    private external fun nativeCreateRenderer(
        backendType: Int,
        enableVulkanValidation: Boolean
    ): Long

    @JvmStatic
    private external fun nativeDestroyRenderer(handle: Long)

    @JvmStatic
    private external fun nativeSetSurface(handle: Long, surface: Surface?)

    @JvmStatic
    private external fun nativeSetScene(handle: Long, scene: Int)

    @JvmStatic
    private external fun nativeSetMsaaSampleCount(handle: Long, sampleCount: Int)

    @JvmStatic
    private external fun nativeOnSurfaceCreated(handle: Long)

    @JvmStatic
    private external fun nativeOnSurfaceDestroyed(handle: Long)

    @JvmStatic
    private external fun nativeOnSurfaceChanged(handle: Long, width: Int, height: Int)

    @JvmStatic
    private external fun nativeDrawFrame(handle: Long)

    @JvmStatic
    private external fun nativeGetRendererOverlay(handle: Long): String
}
