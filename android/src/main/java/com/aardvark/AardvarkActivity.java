package com.aardvark;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.util.Map;
import java.util.HashMap;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import org.json.JSONObject;
import org.json.JSONException;

/*
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.webkit.WebViewClient;
 */

class JsonChannel extends MessageChannel<JSONObject> {
    JsonChannel() {
        super(JsonCodec.instance);
    }
}

public class AardvarkActivity extends Activity {
    static {
        System.loadLibrary("aardvark-android");
    }

    private AardvarkView view;
    private long nativeAppPtr;
    private Map<String, Object> channels = new HashMap<String, Object>();
    //private WebView webView;

    @Override
    protected void onCreate(Bundle bundle) {
        super.onCreate(bundle);
        view = new AardvarkView(getApplicationContext(), this);
        setContentView(view);
        /*
        webView = new WebView(getApplicationContext());
        webView.getSettings().setJavaScriptEnabled(true);
        webView.setWebViewClient(new WebViewClient());
        webView.setWebChromeClient(new WebChromeClient());
        webView.loadUrl("http://google.com");
        auto viewId = virtualViewController(webView, 512, 512);
        VirtualDisplayHolder holder = new VirtualDisplayHolder(this, webView, 512, 512);
        */
        //view = new AardvarkView(getApplicationContext(), true, holder);
    }

    public static native void init(long appPtr, BinaryChannel platformChannel);

    public <T> void registerChannel(String name, MessageChannel<T> channel) {
        channels.put(name, channel);
    }

    public <T> MessageChannel<T> getChannel(String name) {
        return (MessageChannel<T>) channels.get(name);
    }

    public <T> void sendMessage(String name, T message) {
        getChannel(name).sendMessage(message);
    }

    public void onBeforeNativeAppCreate() {
        NativeWrapper.initJni();

        // create channel
        registerChannel("system", new JsonChannel());

        // send message to channel
        /*
        try {
            JSONObject message = new JSONObject("{a: 1}");
            sendMessage("system", message);
        } catch (JSONException e) {
        }
        */

        // handle message from channel
        MessageHandler<JSONObject> jsonHandler = new MessageHandler<JSONObject>() {
            public void handle(JSONObject message) {
                Log.i("JSON MESSAGE", message.toString());
            }
        };
        getChannel("system").setMessageHandler(jsonHandler);
    }

    public void onNativeAppCreate(long appPtr) {
        nativeAppPtr = appPtr;
        MessageChannel<JSONObject> channel = this.<JSONObject>getChannel("system");
        init(appPtr, channel.binaryChannel);
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