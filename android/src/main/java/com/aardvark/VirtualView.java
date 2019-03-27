package com.aardvark;

import android.content.Context;
import android.view.View;
import android.graphics.SurfaceTexture;
import android.view.Surface;
import android.view.Display;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.app.Presentation;

class VirtualView {
    private Context context;
    private SurfaceTexture surfaceTexture;
    private Surface surface;
    private VirtualDisplay virtualDisplay;
    private View view;

    VirtualView(Context context, View view, int width, int height) {
        this.view = view;
        surfaceTexture = new SurfaceTexture(0);
        surfaceTexture.detachFromGLContext();
        surfaceTexture.setDefaultBufferSize(width, height);
        Surface surface = new Surface(surfaceTexture);
        DisplayManager displayManager =
                (DisplayManager) context.getSystemService(Context.DISPLAY_SERVICE);
        int densityDpi = context.getResources().getDisplayMetrics().densityDpi;
        virtualDisplay = displayManager.createVirtualDisplay(
                "aardvark-vd",
                width,
                height,
                densityDpi,
                surface,
                0
        );
        Presentation presentation = new Presentation(context, virtualDisplay.getDisplay());
        presentation.setContentView(view);
        presentation.show();
    }

    void resize(int width, int height) {
        int densityDpi = context.getResources().getDisplayMetrics().densityDpi;
        virtualDisplay.resize(width, height, densityDpi);
    }

    SurfaceTexture getSurfaceTexture() {
        return surfaceTexture;
    }
}