package org.lynxsdk.lynx.skity.dev

import androidx.compose.runtime.mutableStateOf

object VulkanFramePacingSettings {
    private val modeState = mutableStateOf(VulkanFramePacingMode.CHOREOGRAPHER)

    val mode: VulkanFramePacingMode
        get() = modeState.value

    fun updateMode(value: VulkanFramePacingMode) {
        modeState.value = value
    }
}
