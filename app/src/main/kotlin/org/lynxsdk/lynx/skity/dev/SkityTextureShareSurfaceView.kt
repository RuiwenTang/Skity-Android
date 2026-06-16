package org.lynxsdk.lynx.skity.dev

import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView

class SkityTextureShareSurfaceView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null
) : SurfaceView(context, attrs), SurfaceHolder.Callback {
    private val session = SharedRendererRegistry.textureShareSession

    init {
        holder.addCallback(this)
    }

    fun setMsaaSampleCount(sampleCount: Int) {
        session.setMsaaSampleCount(sampleCount)
    }

    fun onResumeRendering() {
        session.onResumeRendering()
    }

    fun onPauseRendering() {
        session.onPauseRendering()
    }

    fun getOverlayDetails(): String = session.getOverlayDetails()

    fun release() {
        session.detachSurface()
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        session.attachSurface(holder.surface, DemoScene.SHAPES)
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        session.updateSize(width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        session.detachSurface()
    }
}
