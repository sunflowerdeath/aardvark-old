package com.aardvark;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

class AardvarkView extends GLSurfaceView {
    private static String TAG = "AardvarkView";

    public AardvarkView(Context context, AardvarkActivity activity) {
        super(context);
        this.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        setEGLContextClientVersion(2);
        setEGLConfigChooser(new ConfigChooser());
        setRenderer(new Renderer(activity));
    }

    // Chooser that selects RGBA888 to match code creating Skia surfaces on the native side
    private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser {
        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
            int EGL_OPENGL_ES2_BIT = 4;
            int[] configAttribs =
                    {
                            EGL10.EGL_RED_SIZE, 8,
                            EGL10.EGL_GREEN_SIZE, 8,
                            EGL10.EGL_BLUE_SIZE, 8,
                            EGL10.EGL_ALPHA_SIZE, 8,
                            EGL10.EGL_STENCIL_SIZE, 8,
                            EGL10.EGL_DEPTH_SIZE, 0,
                            EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                            EGL10.EGL_NONE
                    };
            int[] numConfigs = new int[1];
            EGLConfig[] configs = new EGLConfig[1];
            egl.eglChooseConfig(display, configAttribs, configs, 1, numConfigs);
            if (numConfigs[0] <= 0) {
                throw new IllegalArgumentException("No configs match config spec");
            }
            return configs[0];
        }
    }

    private static class Renderer implements GLSurfaceView.Renderer {
        private long appPtr;
        private AardvarkActivity activity;

        Renderer(AardvarkActivity activity) {
            this.activity = activity;
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            appPtr = NativeWrapper.appCreate(activity, width, height);
            activity.onNativeAppCreate(appPtr);
        }

        public void onDrawFrame(GL10 gl) {
            //gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);
            NativeWrapper.appUpdate(appPtr);
            //gl.glFlush();
        }
    }
}