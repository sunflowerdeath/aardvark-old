package com.aardvark;

import java.nio.ByteBuffer;

public interface MessageCodec<T> {
    ByteBuffer encode(T message);
    T decode(ByteBuffer message);
}