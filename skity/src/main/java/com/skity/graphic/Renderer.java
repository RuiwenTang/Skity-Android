package com.skity.graphic;

public class Renderer {
    private long nativeHandle = 0;

    static {
        System.loadLibrary("skity_android");
    }


    public void init(int width, int height, int density) {
        nativeHandle = nativeInit(width, height, density);
    }

    public void draw() {
        nativeDraw(nativeHandle);
    }

    public void destroy() {
        nativeDestroy(nativeHandle);
    }

    private native long nativeInit(int width, int height, int density);

    private native void nativeDraw(long handler);

    private native void nativeDestroy(long handler);
}

