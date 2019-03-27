package com.aardvark;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

/*
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.webkit.WebViewClient;
 */

public class AardvarkActivity extends Activity {
    static {
        System.loadLibrary("aardvark-android");
    }

    private AardvarkView view;
    //private WebView webView;
    private BinaryChannel systemChannel;

    @Override
    protected void onCreate(Bundle bundle) {
        super.onCreate(bundle);

        NativeWrapper.initJni();
        systemChannel = new BinaryChannel();
        BinaryChannel.MessageHandler handler = new BinaryChannel.MessageHandler() {
            public void handle(ByteBuffer message) {
                try {
                    String str = new String(message.array(), "UTF-8");
                    Log.i("MESSAGE", str);
                } catch(UnsupportedEncodingException exc) {
                    Log.i("MESSAGE", "Unsupported encoding");
                }
            }
        };
        systemChannel.setMessageHandler(handler);

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

    public static native void init(long appPtr);

    public void onNativeAppCreate(long appPtr) {
        init(appPtr);
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