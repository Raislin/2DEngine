#ifndef BASE_ANIMATION_H
#define BASE_ANIMATION_H

#include <optional>
#include <sstream>

class SpriteSheet;
using Frame = unsigned;
class BaseAnimation {
    friend class SpriteSheet;

public:
    BaseAnimation();
    virtual ~BaseAnimation();

    void Play();
    void Pause();
    void Stop();
    void Reset();
    virtual void Update(const float& time_delta);
    friend std::stringstream& operator>>(std::stringstream& stream,
        BaseAnimation& animation)
    {
        animation.ReadIn(stream);
        return stream;
    }

    void SetSpriteSheet(SpriteSheet* sheet);
    void SetFrame(const Frame frame);
    void SetStartFrame(Frame frame);
    void SetEndFrame(Frame frame);
    void SetFrameRow(Frame frame);
    void SetActionStart(Frame frame);
    void SetActionEnd(Frame frame);
    void SetFrameTime(float time);
    void SetLooping(bool loop);
    void SetId(const std::string& id);

    const SpriteSheet* GetSpriteSheet() const;
    const Frame GetCurrentFrame() const;
    const Frame GetStartFrame() const;
    const Frame GetEndFrame() const;
    const Frame GetFrameRow() const;
    const std::optional<Frame> GetActionStart() const;
    const std::optional<Frame> GetActionEnd() const;
    const float GetFrameTime() const;
    const float GetElapsedTime() const;
    const bool IsLooping() const;
    const bool IsPlaying() const;
    const bool IsInAction() const;
    const std::string GetName() const;

protected:
    virtual void FrameStep() = 0;
    virtual void CropSprite() = 0;
    virtual void ReadIn(std::stringstream& stream) = 0;
    Frame current_frame_ = 0;
    Frame frame_start_ = 0;
    Frame frame_end_ = 0;
    Frame frame_row_ = 0;
    std::optional<Frame>
        frame_action_start_; // Frame when a specific "action" begins
    std::optional<Frame> frame_action_end_; // Frame when a specific "action" ends
    float frame_time_ = 0.0f;
    float elapsed_time_ = 0.0f;
    bool looping_ = false;
    bool playing_ = false;
    std::string animation_id_;
    SpriteSheet* sprite_sheet_;
};

#endif // !BASE_ANIMATION_H
