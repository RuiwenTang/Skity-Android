package org.lynxsdk.lynx.skity.dev

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

    fun createRenderer(): Long = nativeCreateRenderer()

    fun destroyRenderer(handle: Long) {
        nativeDestroyRenderer(handle)
    }

    fun setScene(handle: Long, scene: Int) {
        nativeSetScene(handle, scene)
    }

    fun onSurfaceCreated(handle: Long) {
        nativeOnSurfaceCreated(handle)
    }

    fun onSurfaceChanged(handle: Long, width: Int, height: Int) {
        nativeOnSurfaceChanged(handle, width, height)
    }

    fun drawFrame(handle: Long) {
        nativeDrawFrame(handle)
    }

    @JvmStatic
    private external fun nativeGetStatusSummary(): String

    @JvmStatic
    private external fun nativeCreateRenderer(): Long

    @JvmStatic
    private external fun nativeDestroyRenderer(handle: Long)

    @JvmStatic
    private external fun nativeSetScene(handle: Long, scene: Int)

    @JvmStatic
    private external fun nativeOnSurfaceCreated(handle: Long)

    @JvmStatic
    private external fun nativeOnSurfaceChanged(handle: Long, width: Int, height: Int)

    @JvmStatic
    private external fun nativeDrawFrame(handle: Long)
}
