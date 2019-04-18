package com.aardvark;

import java.util.Map;
import java.util.HashMap;

import org.json.JSONObject;

import android.app.Activity;
import android.os.Bundle;

public class AardvarkActivity extends Activity {
    private AardvarkView view;
    private long nativeAppPtr;
    private Map<String, Object> channels = new HashMap<String, Object>();

    @Override
    protected void onCreate(Bundle bundle) {
        super.onCreate(bundle);
        view = new AardvarkView(getApplicationContext(), this);
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
        view.queueEvent(new Runnable() {
            public void run() {
                channel.sendMessage(message);
            }
        });
    }

    public void onBeforeNativeAppCreate() {
        NativeWrapper.initJni();
        registerChannel("system", new MessageChannel(JsonCodec.instance));
    }

    public void onNativeAppCreate(long nativeAppPtr) {
        this.nativeAppPtr = nativeAppPtr;
    }

    @Override
    protected void onPause() {
        super.onPause();
        view.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        view.onResume();
    }
}