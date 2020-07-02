package com.aardvark;

import org.json.JSONObject;
import org.json.JSONException;

import android.content.Context;
import android.graphics.PixelFormat;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.view.Surface;
import android.view.MotionEvent;

class AardvarkSurfaceView extends SurfaceView {
    private static String TAG = "AardvarkSurfaceView";

    private AardvarkActivity activity;
    
    private final SurfaceHolder.Callback surfaceCallback =
        new SurfaceHolder.Callback() {
            public void surfaceCreated(SurfaceHolder holder) {
                // TODO
            }

            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                //surfaceChanged(width, height);
                activity.onBeforeNativeAppCreate();
                Surface surface = getHolder().getSurface();
                long hostPtr = NativeWrapper.hostCreate(
                        activity,
                        activity.<JSONObject>getChannel("system").binaryChannel,
                        surface);
                activity.onNativeAppCreate(hostPtr);
            }

            public void surfaceDestroyed(SurfaceHolder holder) {
                //surfaceDestroyed();
            }
    };

    public AardvarkSurfaceView(Context context, AardvarkActivity activity) {
        super(context);
        this.activity = activity;
        getHolder().setFormat(PixelFormat.TRANSLUCENT);
        getHolder().addCallback(surfaceCallback);
    }

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
