#include "BaseAnimation.h"
#include "SpriteSheet.h"

BaseAnimation::BaseAnimation() {}

BaseAnimation::~BaseAnimation() {}

void BaseAnimation::Play() { playing_ = true; }

void BaseAnimation::Pause() { playing_ = false; }

void BaseAnimation::Stop()
{
    playing_ = false;
    Reset();
}

void BaseAnimation::Reset()
{
    current_frame_ = frame_start_;
    elapsed_time_ = 0.0f;
    CropSprite();
}

void BaseAnimation::Update(const float& time_delta)
{
    if (playing_) {
        elapsed_time_ += time_delta;
        if (elapsed_time_ >= frame_time_) {
            FrameStep();
            CropSprite();
            elapsed_time_ = 0;
        }
    }
}

void BaseAnimation::SetSpriteSheet(SpriteSheet* sheet)
{
    sprite_sheet_ = sheet;
}

void BaseAnimation::SetFrame(const Frame frame)
{
    if ((frame >= frame_start_ && frame <= frame_end_) || (frame >= frame_end_ && frame <= frame_start_)) {
        current_frame_ = frame;
    }
}

void BaseAnimation::SetStartFrame(Frame frame_start)
{
    frame_start_ = frame_start;
}

void BaseAnimation::SetEndFrame(Frame frame_end) { frame_end_ = frame_end; }

void BaseAnimation::SetFrameRow(Frame frame_row) { frame_row_ = frame_row; }

void BaseAnimation::SetActionStart(Frame frame_action_start)
{
    frame_action_start_ = frame_action_start;
}

void BaseAnimation::SetActionEnd(Frame frame_action_end)
{
    frame_action_end_ = frame_action_end;
}

void BaseAnimation::SetFrameTime(float frame_time) { frame_time_ = frame_time; }

void BaseAnimation::SetLooping(bool loop) { looping_ = loop; }

void BaseAnimation::SetId(const std::string& animation_id)
{
    animation_id_ = animation_id;
}

const SpriteSheet* BaseAnimation::GetSpriteSheet() const
{
    return sprite_sheet_;
}

const Frame BaseAnimation::GetCurrentFrame() const { return current_frame_; }

const Frame BaseAnimation::GetStartFrame() const { return frame_start_; }

const Frame BaseAnimation::GetEndFrame() const { return frame_end_; }

const Frame BaseAnimation::GetFrameRow() const { return frame_row_; }

const std::optional<Frame> BaseAnimation::GetActionStart() const
{
    return frame_action_start_;
}

const std::optional<Frame> BaseAnimation::GetActionEnd() const
{
    return frame_action_end_;
}

const float BaseAnimation::GetFrameTime() const { return frame_time_; }

const float BaseAnimation::GetElapsedTime() const { return elapsed_time_; }

const bool BaseAnimation::IsLooping() const { return looping_; }

const bool BaseAnimation::IsPlaying() const { return playing_; }

const bool BaseAnimation::IsInAction() const
{
    if (!frame_action_start_ || !frame_action_end_) {
        return true;
    }
    return (current_frame_ >= *frame_action_start_ && current_frame_ <= *frame_action_end_);
}

const std::string BaseAnimation::GetName() const { return animation_id_; }
