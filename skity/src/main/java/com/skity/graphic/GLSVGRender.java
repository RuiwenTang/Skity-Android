package com.skity.graphic;

import android.content.Context;
import android.content.res.AssetManager;

public class GLSVGRender extends Renderer {

    @Override
    public void init(int width, int height, int density, Context context) {
        nativeHandle = nativeInitSVG(width, height, density, context);
        nativeLoadSVG(nativeHandle, context.getAssets());
    }

    private native long nativeInitSVG(int width, int height, int density, Context context);

    private native void nativeLoadSVG(long handler, AssetManager assetManager);
}
