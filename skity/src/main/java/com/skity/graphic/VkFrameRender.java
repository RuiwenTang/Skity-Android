package com.skity.graphic;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.view.Surface;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class VkFrameRender extends VkRenderer {

    private List<Bitmap> images = new ArrayList<>();

    @Override
    protected long createNativeHandle(int width, int height, int density, Surface surface) {
        return nativeInit(width, height, density, surface);
    }

    @Override
    protected void onInit(Context context) {
        AssetManager am = context.getAssets();

        nativeInitTypeface(nativeHandle, am);

        for (int i = 0; i < 12; i++) {
            String imageName = String.format("images/image%d.jpg", i + 1);
            try {
                Bitmap bitmap = BitmapFactory.decodeStream(am.open(imageName));
                images.add(bitmap);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        nativeInitImages(nativeHandle, images);
    }


    private native long nativeInit(int width, int height, int density, Surface surface);

    private native void nativeInitTypeface(long handle, AssetManager am);

    private native void nativeInitImages(long handle, List<Bitmap> images);
}
