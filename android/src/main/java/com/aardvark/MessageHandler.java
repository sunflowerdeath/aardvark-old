package com.aardvark;

public interface MessageHandler<T> {
    void handle(T message);
}
