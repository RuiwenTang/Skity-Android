package com.skity.android;

import android.content.Context;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;

import com.skity.graphic.Renderer;

import javax.microedition.khronos.egl.EGL;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

public class SkityDemoView extends GLSurfaceView implements GLSurfaceView.EGLConfigChooser {
    private MRenderer mRenderer;

    public SkityDemoView(Context context) {
        super(context);

        setEGLConfigChooser(this);
        setEGLContextClientVersion(3);

        mRenderer = new MRenderer(this, generateRender());

        setRenderer(mRenderer);
    }

    public void onDestroy() {
        mRenderer.onDestroy();
    }

    protected com.skity.graphic.Renderer generateRender() {
        return new com.skity.graphic.Renderer();
    }

    @Override
    public EGLConfig chooseConfig(EGL10 egl10, EGLDisplay eglDisplay) {
        int[] attrs = {
                EGL10.EGL_RED_SIZE, 8,
                EGL10.EGL_GREEN_SIZE, 8,
                EGL10.EGL_BLUE_SIZE, 8,
                EGL10.EGL_ALPHA_SIZE, 8,
                EGL10.EGL_DEPTH_SIZE, 0,
                EGL10.EGL_STENCIL_SIZE, 8,
                EGL10.EGL_SAMPLE_BUFFERS, 1,
                EGL10.EGL_SAMPLES, 4,
                EGL10.EGL_NONE,
        };

        EGLConfig[] configs = new EGLConfig[1];
        int[] numConfigs = new int[1];

        egl10.eglChooseConfig(eglDisplay, attrs, configs, 1, numConfigs);
        if (numConfigs[0] == 0) {
            return null;
        }

        return configs[0];
    }


    private static class MRenderer implements GLSurfaceView.Renderer {
        private final GLSurfaceView mView;
        private final com.skity.graphic.Renderer mRender;

        private MRenderer(GLSurfaceView view, com.skity.graphic.Renderer nativeRender) {
            this.mView = view;
            mRender = nativeRender;
        }


        @Override
        public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
            mRender.init(mView.getWidth()
                    , mView.getHeight()
                    , (int) mView.getContext().getResources().getDisplayMetrics().density
                    , mView.getContext());
        }

        @Override
        public void onSurfaceChanged(GL10 gl10, int i, int i1) {

        }

        @Override
        public void onDrawFrame(GL10 gl10) {
            mRender.draw();
        }

        public void onDestroy() {
            mRender.destroy();
        }
    }

    ;
}
