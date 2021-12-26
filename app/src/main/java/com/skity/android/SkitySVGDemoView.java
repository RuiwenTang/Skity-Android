package com.skity.android;

import android.content.Context;

import com.skity.graphic.GLSVGRender;
import com.skity.graphic.Renderer;

public class SkitySVGDemoView extends SkityDemoView {
    public SkitySVGDemoView(Context context) {
        super(context);
    }


    @Override
    protected com.skity.graphic.Renderer generateRender() {
        return new GLSVGRender();
    }
}
