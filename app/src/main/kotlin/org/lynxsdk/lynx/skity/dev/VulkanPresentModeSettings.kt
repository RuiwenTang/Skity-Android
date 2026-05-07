package org.lynxsdk.lynx.skity.dev

import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue

object VulkanPresentModeSettings {
    var presentMode by mutableStateOf(VulkanPresentMode.FIFO)
        private set

    fun updatePresentMode(mode: VulkanPresentMode) {
        presentMode = mode
    }
}
