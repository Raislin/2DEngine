#include "DirectionalAnimation.h"
#include "SpriteSheet.h"

void DirectionalAnimation::FrameStep()
{
    if (frame_start_ < frame_end_) {
        ++current_frame_;
    } else {
        --current_frame_;
    }

    if ((frame_start_ < frame_end_ && current_frame_ >= frame_end_) || (frame_start_ > frame_end_ && current_frame_ <= frame_end_)) {
        if (looping_) {
            current_frame_ = frame_start_;
        } else {
            Pause();
        }
    }
}

void DirectionalAnimation::CropSprite()
{
    Frame animation_row_ = frame_row_ + (sprite_sheet_->GetDirection() == Direction::kRight ? 0U : 1U);
    sf::Rect<int> rect(
      sprite_sheet_->GetSpriteSize().x * current_frame_,
      sprite_sheet_->GetSpriteSize().y * animation_row_,
      sprite_sheet_->GetSpriteSize().x,
      sprite_sheet_->GetSpriteSize().y);

    sprite_sheet_->CropSprite(rect);
}

void DirectionalAnimation::ReadIn(std::stringstream& stream)
{
    stream >> frame_start_ >> frame_end_ >> frame_row_ >> frame_time_ >> *frame_action_start_ >> *frame_action_end_;
}
