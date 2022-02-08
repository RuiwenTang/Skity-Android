package com.skity.android;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.Choreographer;

public class VkSVGActivity extends AppCompatActivity implements Choreographer.FrameCallback {

    VkSVGDemoView mView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mView = new VkSVGDemoView(this);
        setContentView(mView);
    }

    @Override
    protected void onResume() {
        super.onResume();
        Choreographer.getInstance().postFrameCallback(this);
    }

    @Override
    protected void onPause() {
        super.onPause();
        Choreographer.getInstance().removeFrameCallback(this);
    }

    @Override
    public void doFrame(long frameTimeNanos) {
        mView.draw();

        Choreographer.getInstance().postFrameCallback(this);
    }
}