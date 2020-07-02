package com.aardvark;

import java.util.Map;
import java.util.HashMap;

import org.json.JSONObject;
import org.json.JSONException;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.view.Choreographer;

/*
class TimerController {
    private Handler handler = new Handler();
    Map<Integer, Runnable> timers = new HashMap<Integer, Runnable>();
    
    public int setTimeout(final int id, final Runnable runnable, int timeout) {
        this.timers.put(id, runnable);
        Runnable wrapper = new Runnable() {
            public void run() {
                if (timers.containsKey(id)) {
                    timers.remove(id);
                    runnable.run();
                }
            }
        };
        this.handler.postDelayed(wrapper, timeout);
        return id;
    }
    
    public void clearTimeout(int id) {
        if (timers.containsKey(id)) {
            handler.removeCallbacks(timers.get(id));
            timers.remove(id);
        }
    }
}

class NativeTimerController {
    private final MessageChannel<JSONObject> channel;
    private TimerController controller = new TimerController();
    
    public NativeTimerController(MessageChannel<JSONObject> achannel) {
        this.channel = achannel;
        MessageHandler<JSONObject> handler = new MessageHandler<JSONObject>() {
            public void handle(final JSONObject message) {
                try {
                    String type = message.getString("type");
                    if (type == "setTimeout") {
                        int timeout = message.getInt("timeout");
                        int id = message.getInt("id");
                        controller.setTimeout(id, new Runnable() {
                            public void run() {
                                channel.sendMessage(message);
                            }
                        }, timeout);
                    } else if (type == "clearTimeout") {
                        int id = message.getInt("id");
                        controller.clearTimeout(id);
                    }
                } catch (JSONException e) { return; }
            }
        };
        channel.setMessageHandler(handler);
    }
}
*/

public class AardvarkActivity extends Activity {
    // private AardvarkView view;
    private AardvarkSurfaceView view;
    private long nativeHostPtr;
    private Map<String, Object> channels = new HashMap<String, Object>();
    private Choreographer choreographer = Choreographer.getInstance();
    private boolean isRendering = false;
    //private NativeTimerController timerController;

    @Override
    protected void onCreate(Bundle bundle) {
        super.onCreate(bundle);
        view = new AardvarkSurfaceView(getApplicationContext(), this);
        setContentView(view);
    }

    public <T> void registerChannel(String name, MessageChannel<T> channel) {
        channels.put(name, channel);
    }

    public <T> MessageChannel<T> getChannel(String name) {
        return (MessageChannel<T>) channels.get(name);
    }

    public <T> void sendMessage(String name, final T message) {
        final MessageChannel<T> channel = getChannel(name);
        channel.sendMessage(message);
        /*
        view.queueEvent(new Runnable() {
            public void run() {
                channel.sendMessage(message);
            }
        });
        */
    }

    public void onBeforeNativeAppCreate() {
        NativeWrapper.initJni();
        
        MessageChannel<JSONObject> systemChannel = new MessageChannel(JsonCodec.instance);
        registerChannel("system", systemChannel);
        
        //MessageChannel<JSONObject> timerChannel = new MessageChannel(JsonCodec.instance);
        //registerChannel("timers", timerChannel);
        //timerController = new NativeTimerController(timerChannel);
    }

    public void onNativeAppCreate(long nativeHostPtr) {
        this.nativeHostPtr = nativeHostPtr;
        startRendering();
    }

    private void startRendering() {
        if (!isRendering) {
            isRendering = true;
            renderLoop();
        }
    }

    private void stopRendering() {
        isRendering = false;
    }

    private void renderLoop() {
        if (!isRendering) return;
        NativeWrapper.hostUpdate(nativeHostPtr);
        choreographer.postFrameCallback(new Choreographer.FrameCallback() {
            public void doFrame(long frameTimeNanos) {
                renderLoop();
            }
        });
    }

    protected void onPause() {
        super.onPause();
        //view.onPause();
    }

    protected void onResume() {
        super.onResume();
        //view.onResume();
    }
}
