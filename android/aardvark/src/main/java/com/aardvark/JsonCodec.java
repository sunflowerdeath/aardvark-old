package com.aardvark;

import java.nio.ByteBuffer;
import org.json.JSONObject;
import org.json.JSONException;

class JsonCodec implements MessageCodec<JSONObject> {
    public ByteBuffer encode(JSONObject message) {
        return StringCodec.instance.encode(message.toString());
    }

    public JSONObject decode(ByteBuffer message) {
        try {
            return new JSONObject(StringCodec.instance.decode(message));
        } catch (JSONException e) {
            throw new IllegalArgumentException("Invalid JSON", e);
        }
    }

    public static JsonCodec instance = new JsonCodec();
}