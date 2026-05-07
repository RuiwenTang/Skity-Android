package org.lynxsdk.lynx.skity.dev

import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue

object VulkanDebugSettings {
    var validationRequested by mutableStateOf(false)
        private set

    fun updateValidationRequested(enabled: Boolean) {
        validationRequested = enabled
    }
}
