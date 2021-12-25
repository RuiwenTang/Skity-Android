package com.skity.graphic;

import android.content.Context;
import android.content.res.AssetManager;

public class Renderer {
    private long nativeHandle = 0;

    static {
        System.loadLibrary("skity_android");
    }


    public void init(int width, int height, int density, Context context) {
        nativeHandle = nativeInit(width, height, density);
        nativeLoadDefaultAssets(nativeHandle, context.getAssets());
    }

    public void draw() {
        nativeDraw(nativeHandle);
    }

    public void destroy() {
        nativeDestroy(nativeHandle);
    }

    private native long nativeInit(int width, int height, int density);

    private native void nativeLoadDefaultAssets(long handler, AssetManager assetManager);

    private native void nativeDraw(long handler);

    private native void nativeDestroy(long handler);
}

