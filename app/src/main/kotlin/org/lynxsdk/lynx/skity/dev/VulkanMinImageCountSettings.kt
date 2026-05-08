package org.lynxsdk.lynx.skity.dev

import androidx.compose.runtime.mutableIntStateOf

object VulkanMinImageCountSettings {
    private val minImageCountState = mutableIntStateOf(VulkanMinImageCount.DOUBLE.imageCount)

    val minImageCount: VulkanMinImageCount
        get() = VulkanMinImageCount.fromImageCount(minImageCountState.intValue)

    fun updateMinImageCount(value: VulkanMinImageCount) {
        minImageCountState.intValue = value.imageCount
    }
}
