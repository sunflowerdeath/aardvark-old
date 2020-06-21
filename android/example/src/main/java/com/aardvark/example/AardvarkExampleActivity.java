package com.aardvark.example;

import java.util.Map;
import java.util.HashMap;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import org.json.JSONObject;
import org.json.JSONException;

import com.aardvark.AardvarkActivity;
import com.aardvark.MessageHandler;

/*
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.webkit.WebViewClient;
 */

public class AardvarkExampleActivity extends AardvarkActivity {
    static {
        System.loadLibrary("aardvark-example");
    }

    public static native void init(long appPtr);
    
    public void onBeforeNativeAppCreate() {
        super.onBeforeNativeAppCreate();

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

    public void onNativeAppCreate(long nativeHostPtr) {
        super.onNativeAppCreate(nativeHostPtr);
        init(nativeHostPtr);
    }
}