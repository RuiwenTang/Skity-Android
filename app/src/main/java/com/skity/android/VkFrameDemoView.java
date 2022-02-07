package com.skity.android;

import android.content.Context;

import com.skity.graphic.VkFrameRender;
import com.skity.graphic.VkRenderer;

public class VkFrameDemoView extends SkityVkDemoView{

    public VkFrameDemoView(Context context) {
        super(context);
    }

    @Override
    protected VkRenderer generateRender() {
        return new VkFrameRender();
    }
}
