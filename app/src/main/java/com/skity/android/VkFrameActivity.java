package com.skity.android;

import androidx.appcompat.app.AppCompatActivity;

import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.view.Choreographer;

public class VkFrameActivity extends AppCompatActivity implements Choreographer.FrameCallback {

    VkFrameDemoView mView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mView = new VkFrameDemoView(this);
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