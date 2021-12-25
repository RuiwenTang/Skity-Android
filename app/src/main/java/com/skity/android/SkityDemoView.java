package com.skity.android;

import android.content.Context;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class SkityDemoView extends GLSurfaceView {
    public SkityDemoView(Context context) {
        super(context);

        setEGLConfigChooser(8, 8, 8, 8, 0, 8);
        setEGLContextClientVersion(3);

        setRenderer(new MRenderer(this));
    }


    private static class MRenderer implements GLSurfaceView.Renderer {
        private final GLSurfaceView mView;
        private com.skity.graphic.Renderer mRender = new com.skity.graphic.Renderer();

        private MRenderer(GLSurfaceView view) {
            this.mView = view;
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
    }

    ;
}
