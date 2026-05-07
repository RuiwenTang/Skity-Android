package org.lynxsdk.lynx.skity.dev

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet

class SkityRenderSurfaceView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null
) : GLSurfaceView(context, attrs) {
    private val rendererDelegate = NativeRendererDelegate()

    init {
        setEGLContextClientVersion(3)
        preserveEGLContextOnPause = true
        setRenderer(rendererDelegate)
        renderMode = RENDERMODE_CONTINUOUSLY
    }

    fun setScene(scene: DemoScene) {
        rendererDelegate.setScene(scene)
    }

    fun getOverlayDetails(): String = rendererDelegate.getOverlayDetails()

    fun release() {
        rendererDelegate.prepareForRelease()
        queueEvent {
            rendererDelegate.releaseOnGlThread()
        }
    }

    private class NativeRendererDelegate : Renderer {
        private var rendererHandle: Long = 0L
        private var pendingReleaseHandle: Long = 0L
        @Volatile
        private var scene: DemoScene = DemoScene.SHAPES

        fun setScene(scene: DemoScene) {
            this.scene = scene
            if (rendererHandle != 0L) {
                SkityNative.setScene(rendererHandle, scene.ordinal)
            }
        }

        fun getOverlayDetails(): String = SkityNative.getRendererOverlay(rendererHandle)

        fun prepareForRelease() {
            pendingReleaseHandle = rendererHandle
            rendererHandle = 0L
        }

        fun releaseOnGlThread() {
            val handle = pendingReleaseHandle
            if (handle != 0L) {
                SkityNative.onSurfaceDestroyed(handle)
                SkityNative.destroyRenderer(handle)
                pendingReleaseHandle = 0L
            }
        }

        override fun onSurfaceCreated(
            gl: javax.microedition.khronos.opengles.GL10?,
            config: javax.microedition.khronos.egl.EGLConfig?
        ) {
            if (rendererHandle == 0L) {
                rendererHandle = SkityNative.createRenderer(BackendType.GLES)
            }
            SkityNative.setScene(rendererHandle, scene.ordinal)
            SkityNative.onSurfaceCreated(rendererHandle)
        }

        override fun onSurfaceChanged(
            gl: javax.microedition.khronos.opengles.GL10?,
            width: Int,
            height: Int
        ) {
            SkityNative.onSurfaceChanged(rendererHandle, width, height)
        }

        override fun onDrawFrame(gl: javax.microedition.khronos.opengles.GL10?) {
            SkityNative.drawFrame(rendererHandle)
        }
    }
}
