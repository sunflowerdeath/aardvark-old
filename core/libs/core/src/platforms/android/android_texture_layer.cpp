#include "android_texture_layer.hpp"

namespace aardvark {

static jclass surface_texture_class;
 = 
auto attach_to_gl_context_method =
    env->GetMethodID(surface_texture_class, "attachToGLContext", "(I)V");
auto detach_from_gl_context_method =
    env -> GetMethodID(surface_texture_class, "detachFromGLContext", "()V");
auto update_tex_image_method;
                                                

AndroidTextureLayer::init_env(env) {
    return env->GetMethodID(surface_texture_class, "updateTexImage", "()V");
}

AndroidTextureLayer::attach_to_gl_context() {
    surface_texture_class = env->FindClass("android/graphics/SurfaceTexture");
    env->CallVoidMethod(surface_texture, attach_to_gl_context_method,
                        texture_name);
    state = AndroidTextureState::Attached;
}

AndroidTexture::detach_from_gl_context() {
    env->CallVoidMethod(surface_texture, detach_from_gl_context_method);
    state = AndroidTextureState::Detached;
}

AndroidTexture::update_tex_image() {
    env->CallVoidMethod(surface_texture, update_tex_image_method);
}

AndroidTexture::update_image_snapshot() {
    GrGLTextureInfo texture_info;
    texture_info.fTarget = GL_TEXTURE_EXTERNAL_OES;
    texture_info.fID = texture_name;
    texture_info.fFormat = GL_RGBA8_OES;
    GrBackendTexture backend_texture(size.width, size.height, GrMipMapped::kNo,
                                     texture_info);
    image_snapshot = SkImage::MakeFromTexture(
        gr_context.get(), backend_texture, kTopLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);
}

AndroidTextureLayer::paint(SkCanvas* canvas, Position position) {
    if (state == AndroidTextureState::Uninitialized) {
        attach_to_gl_context();
    }
    if (state == AndroidTextureState::Detached) return;
    if (has_new_frame) {
        update_tex_image();
        has_new_frame = false
    }
    update_image_snapshot();
    canvas.drawImage(image_snapshot, position.left, position.top);
}

} // namespace aardvark {
