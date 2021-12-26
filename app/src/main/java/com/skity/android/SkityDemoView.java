package com.skity.android;

import android.content.Context;
import android.opengl.GLSurfaceView;

import com.skity.graphic.Renderer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class SkityDemoView extends GLSurfaceView {
    private MRenderer mRenderer;

    public SkityDemoView(Context context) {
        super(context);

        setEGLConfigChooser(8, 8, 8, 8, 0, 8);
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
