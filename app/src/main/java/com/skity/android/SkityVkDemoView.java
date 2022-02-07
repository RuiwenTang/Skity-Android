package com.skity.android;

import android.content.Context;
import android.graphics.Rect;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

import com.skity.graphic.VkRenderer;

public abstract class SkityVkDemoView extends SurfaceView implements SurfaceHolder.Callback {

    private VkRenderer mRenderer;

    public SkityVkDemoView(Context context) {
        super(context);
        mRenderer = generateRender();

        getHolder().addCallback(this);
    }

    public void draw() {
        mRenderer.draw();
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        Surface surface = holder.getSurface();
        Rect rect = holder.getSurfaceFrame();
        mRenderer.destroy();
        mRenderer.init(rect.width(), rect.height(), (int) getContext().getResources().getDisplayMetrics().density, getContext(), surface);
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        mRenderer.destroy();
    }

    protected abstract VkRenderer generateRender();
}
