package org.lynxsdk.lynx.skity.dev

import android.os.Handler
import android.os.HandlerThread
import android.os.Looper
import android.view.Choreographer
import android.view.Surface

object SharedRendererRegistry {
    val vulkanSession: SharedVulkanRendererSession by lazy {
        SharedVulkanRendererSession()
    }
}

class SharedVulkanRendererSession {
    private val renderThread = HandlerThread("SkityVulkanRenderThread").apply { start() }
    private val renderHandler = Handler(renderThread.looper)
    private val mainHandler = Handler(Looper.getMainLooper())

    @Volatile
    private var resumed = false

    @Volatile
    private var surfaceReady = false

    @Volatile
    private var scene: DemoScene = DemoScene.SHAPES

    @Volatile
    private var validationRequested = false
    @Volatile
    private var presentMode = VulkanPresentMode.FIFO
    @Volatile
    private var minImageCount = VulkanMinImageCount.DOUBLE.imageCount

    @Volatile
    private var currentSurface: Surface? = null

    @Volatile
    private var currentWidth = 0

    @Volatile
    private var currentHeight = 0
    @Volatile
    private var msaaSampleCount = 1

    private var rendererHandle: Long = 0L
    @Volatile
    private var frameCallbackScheduled = false

    private val frameCallback = object : Choreographer.FrameCallback {
        override fun doFrame(frameTimeNanos: Long) {
            frameCallbackScheduled = false
            if (resumed && surfaceReady && rendererHandle != 0L) {
                renderHandler.post {
                    if (resumed && surfaceReady && rendererHandle != 0L) {
                        SkityNative.drawFrame(rendererHandle)
                    }
                }
                requestFrame()
            }
        }
    }

    fun attachSurface(surface: Surface, scene: DemoScene) {
        this.scene = scene
        currentSurface = surface
        renderHandler.post {
            ensureRenderer()
            SkityNative.setSurface(rendererHandle, surface)
            SkityNative.setScene(rendererHandle, scene.ordinal)
            SkityNative.setMsaaSampleCount(rendererHandle, msaaSampleCount)
            SkityNative.onSurfaceCreated(rendererHandle)
            surfaceReady = true
            requestFrame()
        }
    }

    fun detachSurface() {
        surfaceReady = false
        currentSurface = null
        cancelFrameCallback()
        renderHandler.post {
            if (rendererHandle != 0L) {
                SkityNative.setSurface(rendererHandle, null)
                SkityNative.onSurfaceDestroyed(rendererHandle)
            }
        }
    }

    fun updateScene(scene: DemoScene) {
        this.scene = scene
        renderHandler.post {
            if (rendererHandle != 0L) {
                SkityNative.setScene(rendererHandle, scene.ordinal)
            }
        }
        requestFrame()
    }

    fun updateSize(width: Int, height: Int) {
        currentWidth = width
        currentHeight = height
        renderHandler.post {
            if (rendererHandle != 0L) {
                SkityNative.onSurfaceChanged(rendererHandle, width, height)
            }
        }
        requestFrame()
    }

    fun onResumeRendering() {
        resumed = true
        requestFrame()
    }

    fun onPauseRendering() {
        resumed = false
        cancelFrameCallback()
    }

    fun getOverlayDetails(): String = SkityNative.getRendererOverlay(rendererHandle)

    fun setValidationRequested(enabled: Boolean) {
        validationRequested = enabled
        renderHandler.post {
            recreateRendererIfNeeded()
        }
    }

    fun setPresentMode(mode: VulkanPresentMode) {
        presentMode = mode
        renderHandler.post {
            recreateRendererIfNeeded()
        }
    }

    fun setMinImageCount(count: Int) {
        minImageCount = count
        renderHandler.post {
            recreateRendererIfNeeded()
        }
    }

    fun setMsaaSampleCount(sampleCount: Int) {
        msaaSampleCount = sampleCount
        renderHandler.post {
            if (rendererHandle != 0L) {
                SkityNative.setMsaaSampleCount(rendererHandle, sampleCount)
            }
        }
    }

    private fun ensureRenderer() {
        if (rendererHandle == 0L) {
            rendererHandle = SkityNative.createRenderer(
                backend = BackendType.VULKAN,
                enableVulkanValidation = validationRequested,
                vulkanPresentMode = presentMode,
                vulkanMinImageCount = minImageCount
            )
            SkityNative.setMsaaSampleCount(rendererHandle, msaaSampleCount)
        }
    }

    private fun recreateRendererIfNeeded() {
        if (rendererHandle == 0L) {
            return
        }

        val currentSurfaceReady = surfaceReady
        val currentScene = scene
        val surface = currentSurface
        val width = currentWidth
        val height = currentHeight

        if (currentSurfaceReady) {
            SkityNative.setSurface(rendererHandle, null)
            SkityNative.onSurfaceDestroyed(rendererHandle)
        }
        SkityNative.destroyRenderer(rendererHandle)
        rendererHandle = 0L
        ensureRenderer()
        if (currentSurfaceReady && surface != null) {
            SkityNative.setSurface(rendererHandle, surface)
            SkityNative.setScene(rendererHandle, currentScene.ordinal)
            SkityNative.onSurfaceCreated(rendererHandle)
            if (width > 0 && height > 0) {
                SkityNative.onSurfaceChanged(rendererHandle, width, height)
            }
            requestFrame()
        }
    }

    private fun requestFrame() {
        if (!resumed || !surfaceReady || frameCallbackScheduled) {
            return
        }
        frameCallbackScheduled = true
        mainHandler.post {
            Choreographer.getInstance().postFrameCallback(frameCallback)
        }
    }

    private fun cancelFrameCallback() {
        if (!frameCallbackScheduled) {
            return
        }
        frameCallbackScheduled = false
        mainHandler.post {
            Choreographer.getInstance().removeFrameCallback(frameCallback)
        }
    }
}
