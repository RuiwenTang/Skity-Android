package com.skity.graphic;

import android.content.Context;
import android.content.res.AssetManager;
import android.view.Surface;

public class VkSVGRenderer extends VkRenderer {
    @Override
    protected long createNativeHandle(int width, int height, int density, Surface surface) {
        return nativeCreateSVGRender(width, height, density, surface);
    }

    @Override
    protected void onInit(Context context) {
        nativeInitSVGDom(nativeHandle, context.getAssets());
    }

    private native long nativeCreateSVGRender(int width, int height, int density, Surface surface);

    private native void nativeInitSVGDom(long handler, AssetManager assetManager);
}
