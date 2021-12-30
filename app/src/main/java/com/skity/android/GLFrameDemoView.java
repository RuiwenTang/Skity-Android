package com.skity.android;

import android.content.Context;

import com.skity.graphic.GLFrameRender;
import com.skity.graphic.Renderer;

public class GLFrameDemoView extends SkityDemoView {
    public GLFrameDemoView(Context context) {
        super(context);
    }

    @Override
    protected com.skity.graphic.Renderer generateRender() {
        return new GLFrameRender();
    }
}
