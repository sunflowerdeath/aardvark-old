#include "paint_cache.hpp"

namespace aardvark {

enum class PaintCacheMechanism { Record, Rasterize, Off };

void PaintCache::restart(bool is_changed) {
    this->is_changed = is_changed;
    if (is_changed) records.clear();
    current_index = 0;
}

void PaintCache::paint(SkCanvas* real_canvas,
                       std::function<void(SkCanvas*)> painter) {
    if (is_changed) {
        // Record commands as picture
        auto info = real_canvas->imageInfo();
        SkPictureRecorder recorder;
        SkCanvas* picture_canvas =
            recorder.beginRecording({0, 0, static_cast<float>(info.width()),
                                     static_cast<float>(info.height())});
        painter(picture_canvas);
        sk_sp<SkPicture> picture = recorder.finishRecordingAsPicture();
        records.push_back(picture);
    }
    // Replay commands from picture
    real_canvas->drawPicture(records[current_index]);
    current_index++;
}

}  // namespace aardvark
