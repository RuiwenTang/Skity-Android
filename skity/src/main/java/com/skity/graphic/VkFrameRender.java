package com.skity.graphic;

import android.content.Context;
import android.view.Surface;

public class VkFrameRender extends VkRenderer {
    @Override
    protected long createNativeHandle(int width, int height, int density, Surface surface) {
        return nativeInit(width, height, density, surface);
    }

    @Override
    protected void onInit(Context context) {

    }


    private native long nativeInit(int width, int height, int density, Surface surface);
}
