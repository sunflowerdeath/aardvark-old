package com.aardvark;

import org.json.JSONObject;
import org.json.JSONException;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

class AardvarkView extends GLSurfaceView {
    private static String TAG = "AardvarkView";

    private AardvarkActivity activity;

    public AardvarkView(Context context, AardvarkActivity activity) {
        super(context);
        this.activity = activity;
        this.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        setEGLContextClientVersion(2);
        setEGLConfigChooser(new ConfigChooser());
        setRenderer(new Renderer(activity, this));
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
        private AardvarkView view;

        Renderer(AardvarkActivity activity, AardvarkView view) {
            this.activity = activity;
            this.view = view;
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            activity.onBeforeNativeAppCreate();
            appPtr = NativeWrapper.appCreate(
                activity, activity.<JSONObject>getChannel("system").binaryChannel, width, height);
            activity.onNativeAppCreate(appPtr);
        }

        public void onDrawFrame(GL10 gl) {
            NativeWrapper.appUpdate(appPtr);
        }
    }

    @Override
    public boolean onTouchEvent(final MotionEvent event) {
        JSONObject message = new JSONObject();
        try {
            message.put("x", event.getX());
            message.put("y", event.getY());
            message.put("action", event.getAction());
        } catch (JSONException e) {
            return true;
        }
        activity.sendMessage("system", message);
        return true;
    }
}