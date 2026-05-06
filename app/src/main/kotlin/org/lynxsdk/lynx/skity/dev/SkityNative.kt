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

    @JvmStatic
    private external fun nativeGetStatusSummary(): String
}
