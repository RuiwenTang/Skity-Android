package com.skity.graphic;

import android.content.Context;
import android.content.res.AssetManager;
import android.view.Surface;

public abstract class VkRenderer {
    protected long nativeHandle = 0;

    static {
        System.loadLibrary("skity_android");
    }

    public void init(int width, int height, int density, Context context, Surface surface) {
        nativeHandle = createNativeHandle(width, height, density, surface);
        nativeLoadDefaultAssets(nativeHandle, context.getAssets());

        onInit(context);
    }

    public void draw() {
        nativeDraw(nativeHandle);
    }

    public void destroy() {
        if (nativeHandle == 0) {
            return;
        }
        nativeDestroy(nativeHandle);
        nativeHandle = 0;
    }

    protected abstract long createNativeHandle(int width, int height, int density, Surface surface);

    protected abstract void onInit(Context context);

    private native void nativeLoadDefaultAssets(long handler, AssetManager assetManager);

    private native void nativeDraw(long handler);

    private native void nativeDestroy(long handler);
}
