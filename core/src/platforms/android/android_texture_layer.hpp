#pragma once

#include "jni.h"
#include "layer.hpp"
#include "base_types.hpp"
#include "GrContext.h"
#include "SkImage.h"

namespace aardvark {

class enum AndroidTextureLayerState = { uninitialized, attached, detached };

class AndroidTextureLayer : public Layer {
  public:
    AndroidTextureLayer(sk_sp<GrContext> gr_context, jobject surface_texture)
        : gr_context(gr_context), surface_texture(surface_texture){};

  private:
    // Android SurfaceTexture object
    jobject surface_texture;

    AndroidTextureLayerState state = AndroidTextureLayerState::uninitialized;

    // OpenGL texture to which the SurfaceTexture is attached.
    int texture_name;

    bool has_new_frame = true;

    sk_sp<SkImage> image_snapshot;

    void attach_to_gl_context();

    void detach_from_gl_context();
 
    // Updates the OpenGl texture from the android SurfaceTexture.
    // Can be called only in `attached` state.
    void update_tex_image();

    void update_image_snapshot();
};

}  // namespace aardvark
