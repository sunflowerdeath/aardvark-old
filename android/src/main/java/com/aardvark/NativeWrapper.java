package com.aardvark;

// import android.graphics.SurfaceTexture;
import java.nio.ByteBuffer;

// Wrapper for the native library
public class NativeWrapper {
    static {
        System.loadLibrary("aardvark-android");
    }

    // Initializes JNI bindings, should be called before all other code
    public static native void initJni();

    // Creates native application id, returns address of the instance
    // Returns pointer to native object
    public static native long appCreate(AardvarkActivity activity, int width, int height);

    // Updates state of the native application
    public static native void appUpdate(long app_ptr);

    // Creates native channel connected to the channel on the platform side
    // Returns pointer to native object
    public static native long channelCreate(BinaryChannel channel);

    // Requests native channel to handle the message
    public static native void channelHandleMessage(long channelPtr, ByteBuffer data);

    // Requests native channel to handle the message
    public static native long messageChannelCreate(MessageChannel channel);
}