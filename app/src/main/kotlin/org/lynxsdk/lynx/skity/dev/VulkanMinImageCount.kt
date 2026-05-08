package org.lynxsdk.lynx.skity.dev

enum class VulkanMinImageCount(val imageCount: Int, val title: String) {
    DOUBLE(2, "2"),
    TRIPLE(3, "3"),
    QUAD(4, "4");

    companion object {
        fun fromImageCount(imageCount: Int): VulkanMinImageCount {
            return entries.firstOrNull { it.imageCount == imageCount } ?: DOUBLE
        }
    }
}
