package com.skity.android;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

public class GLSVGRenderActivity extends AppCompatActivity {

    SkitySVGDemoView mView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mView = new SkitySVGDemoView(this);
        setContentView(mView);
    }

    @Override
    protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mView.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mView.onDestroy();
    }
}