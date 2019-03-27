package com.aardvark;

import java.nio.ByteBuffer;

// Сhannel for exchanging messages between platform and native side
public class BinaryChannel {
    static public interface MessageHandler {
        void handle(ByteBuffer message);
    }

    private long nativeChannelAddr;
    private MessageHandler handler;

    BinaryChannel() {
        nativeChannelAddr = NativeWrapper.channelCreate(this);
    }
    
    // Sends message to the native side
    public void sendMessage(ByteBuffer message) {
        NativeWrapper.channelHandleMessage(nativeChannelAddr, message);
    }
    
    public void setMessageHandler(MessageHandler handler) {
        this.handler = handler;
    }
    
    // Handles message received from the native side
    public void handleMessage(ByteBuffer data) {
        if (handler != null) handler.handle(data);
    }
}