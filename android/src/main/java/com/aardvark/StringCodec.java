package com.aardvark;

import java.nio.ByteBuffer;
import java.io.UnsupportedEncodingException;

class StringCodec implements MessageCodec<String> {
    public ByteBuffer encode(String message) {
        try {
            return ByteBuffer.wrap(message.getBytes("UTF-8"));
        } catch(UnsupportedEncodingException e) {
            throw new IllegalArgumentException("Unsupported encoding", e);
        }
    }

    public String decode(ByteBuffer message) {
        try {
            byte[] bytes = new byte[message.remaining()];
            message.get(bytes);
            return new String(bytes, "UTF-8");
        } catch(UnsupportedEncodingException e) {
            throw new IllegalArgumentException("Unsupported encoding", e);
        }
    }

    public static StringCodec instance = new StringCodec();
};