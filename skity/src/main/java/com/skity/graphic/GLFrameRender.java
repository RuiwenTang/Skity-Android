package com.skity.graphic;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class GLFrameRender extends Renderer {
    private List<Bitmap> images = new ArrayList<>();

    @Override
    public void init(int width, int height, int density, Context context) {
        nativeHandle = nativeInitFrame(width, height, density, context);
        AssetManager am = context.getAssets();


        nativeInitTypefaces(nativeHandle, am);
        initInternal(context);
    }


    private void initInternal(Context context) {
        images = new ArrayList<>(12);
        AssetManager am = context.getAssets();
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

    @Override
    public void destroy() {
        super.destroy();

        for (Bitmap bitmap : images) {
            bitmap.recycle();
        }
    }

    private native long nativeInitFrame(int width, int height, int density, Context context);

    private native void nativeInitImages(long nativeHandle, List<Bitmap> images);

    private native void nativeInitTypefaces(long nativeHandle, AssetManager assetManager);
}
