package org.lynxsdk.lynx.skity.dev

import android.os.Handler
import android.os.HandlerThread
import android.view.Surface

object SharedRendererRegistry {
    val vulkanSession: SharedVulkanRendererSession by lazy {
        SharedVulkanRendererSession()
    }
}

class SharedVulkanRendererSession {
    private val renderThread = HandlerThread("SkityVulkanRenderThread").apply { start() }
    private val renderHandler = Handler(renderThread.looper)

    @Volatile
    private var resumed = false

    @Volatile
    private var surfaceReady = false

    @Volatile
    private var scene: DemoScene = DemoScene.SHAPES

    private var rendererHandle: Long = 0L

    private val frameRunnable = object : Runnable {
        override fun run() {
            if (resumed && surfaceReady && rendererHandle != 0L) {
                SkityNative.drawFrame(rendererHandle)
                renderHandler.postDelayed(this, FRAME_DELAY_MS)
            }
        }
    }

    fun attachSurface(surface: Surface, scene: DemoScene) {
        this.scene = scene
        renderHandler.post {
            ensureRenderer()
            SkityNative.setSurface(rendererHandle, surface)
            SkityNative.setScene(rendererHandle, scene.ordinal)
            SkityNative.onSurfaceCreated(rendererHandle)
            surfaceReady = true
            requestFrame()
        }
    }

    fun detachSurface() {
        surfaceReady = false
        renderHandler.removeCallbacks(frameRunnable)
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
                requestFrame()
            }
        }
    }

    fun updateSize(width: Int, height: Int) {
        renderHandler.post {
            if (rendererHandle != 0L) {
                SkityNative.onSurfaceChanged(rendererHandle, width, height)
                requestFrame()
            }
        }
    }

    fun onResumeRendering() {
        resumed = true
        requestFrame()
    }

    fun onPauseRendering() {
        resumed = false
        renderHandler.removeCallbacks(frameRunnable)
    }

    private fun ensureRenderer() {
        if (rendererHandle == 0L) {
            rendererHandle = SkityNative.createRenderer(BackendType.VULKAN)
        }
    }

    private fun requestFrame() {
        renderHandler.removeCallbacks(frameRunnable)
        if (resumed && surfaceReady) {
            renderHandler.post(frameRunnable)
        }
    }

    private companion object {
        const val FRAME_DELAY_MS = 16L
    }
}
