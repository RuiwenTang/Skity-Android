package com.skity.android;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    private Button mOpenGLExample;
    private Button mOpenGLSVGExample;
    private Button mOpenGLFrameExample;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main2);

        mOpenGLExample = findViewById(R.id.gl_example);
        mOpenGLSVGExample = findViewById(R.id.gl_svg_example);
        mOpenGLFrameExample = findViewById(R.id.gl_frame_example);

        mOpenGLExample.setOnClickListener(this);
        mOpenGLSVGExample.setOnClickListener(this);
        mOpenGLFrameExample.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        Intent intent = new Intent();
        if (view == mOpenGLExample) {
            intent.setClass(this, GLRenderActivity.class);
        } else if (view == mOpenGLSVGExample) {
            intent.setClass(this, GLSVGRenderActivity.class);
        } else if (view == mOpenGLFrameExample) {
            intent.setClass(this, GLFrameActivity.class);
        }

        this.startActivity(intent);
    }
}