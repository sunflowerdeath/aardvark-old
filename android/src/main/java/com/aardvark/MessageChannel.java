package com.aardvark;

import java.nio.ByteBuffer;
import android.util.Log;

public class MessageChannel<T> {
    private MessageCodec<T> codec;
    private BinaryChannel binaryChannel;
    private MessageHandler<T> handler;
    // public long nativeChannelPtr;

    MessageChannel(MessageCodec<T> acodec) {
        codec = acodec;
        binaryChannel = new BinaryChannel();
        BinaryChannel.MessageHandler binaryHandler = new BinaryChannel.MessageHandler() {
            public void handle(ByteBuffer message) {
                if (handler == null) return;
                try {
                    handler.handle(codec.decode(message));
                } catch(RuntimeException e) {
                    Log.e("MessageChannel", "Failed to handle message", e);
                }
            }
        };
        binaryChannel.setMessageHandler(binaryHandler);
        // nativeChannelAddr = NativeWrapper.messageChannelCreate(this);
    }

    public void sendMessage(T message) {
        binaryChannel.sendMessage(codec.encode(message));
    }

    public void setMessageHandler(MessageHandler handler) {
        this.handler = handler;
    }
}