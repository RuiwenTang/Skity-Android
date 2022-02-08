package com.skity.android;

import android.content.Context;

import com.skity.graphic.VkRenderer;
import com.skity.graphic.VkSVGRenderer;

public class VkSVGDemoView extends SkityVkDemoView {
    public VkSVGDemoView(Context context) {
        super(context);
    }

    @Override
    protected VkRenderer generateRender() {
        return new VkSVGRenderer();
    }
}
