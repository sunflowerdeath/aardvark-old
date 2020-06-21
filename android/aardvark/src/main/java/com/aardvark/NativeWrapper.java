package com.aardvark;

import java.nio.ByteBuffer;

// Wrapper for the native library
public class NativeWrapper {
    static {
        System.loadLibrary("aardvark_js");
    }

    // Initializes JNI bindings, should be called before all other code
    public static native void initJni();

    // Creates native application id, returns address of the instance
    // Returns pointer to native object
    public static native long hostCreate(
        AardvarkActivity activity, BinaryChannel systemChannel, int width, int height
    );

    // Triggers update of the native application
    public static native void hostUpdate(long hostPtr);

    // Creates native channel connected to the channel on the platform side
    // Returns pointer to native object
    public static native long channelCreate(BinaryChannel channel);

    // Requests native channel to handle the message
    public static native void channelHandleMessage(long channelPtr, ByteBuffer data);
}