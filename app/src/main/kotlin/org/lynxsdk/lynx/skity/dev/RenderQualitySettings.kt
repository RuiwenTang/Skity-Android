package org.lynxsdk.lynx.skity.dev

import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue

object RenderQualitySettings {
    var msaaSampleCount by mutableStateOf(1)
        private set

    fun setMsaaEnabled(enabled: Boolean) {
        msaaSampleCount = if (enabled) 4 else 1
    }

    fun isMsaaEnabled(): Boolean = msaaSampleCount > 1
}
