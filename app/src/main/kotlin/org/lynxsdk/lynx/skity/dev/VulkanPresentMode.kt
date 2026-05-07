package org.lynxsdk.lynx.skity.dev

enum class VulkanPresentMode(
    val title: String,
    val description: String
) {
    FIFO(
        "FIFO",
        "Always supported and usually the most stable choice for Android swapchains."
    ),
    MAILBOX(
        "Mailbox",
        "Lower-latency triple-buffer style presentation when the device exposes it."
    ),
    IMMEDIATE(
        "Immediate",
        "May reduce pacing overhead, but can introduce tearing and is often unsupported."
    )
}
