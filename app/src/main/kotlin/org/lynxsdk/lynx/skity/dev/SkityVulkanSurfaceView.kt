package org.lynxsdk.lynx.skity.dev

import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView

class SkityVulkanSurfaceView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null
) : SurfaceView(context, attrs), SurfaceHolder.Callback {
    private val session = SharedRendererRegistry.vulkanSession
    @Volatile
    private var scene: DemoScene = DemoScene.SHAPES

    init {
        holder.addCallback(this)
    }

    fun setScene(scene: DemoScene) {
        this.scene = scene
        session.updateScene(scene)
    }

    fun onResumeRendering() {
        session.onResumeRendering()
    }

    fun onPauseRendering() {
        session.onPauseRendering()
    }

    fun release() {
        session.detachSurface()
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        session.attachSurface(holder.surface, scene)
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        session.updateSize(width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        session.detachSurface()
    }
}
