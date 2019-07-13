#ifndef DIRECTIONAL_ANIMATION_H
#define DIRECTIONAL_ANIMATION_H

#include "BaseAnimation.h"
class DirectionalAnimation : public BaseAnimation {
protected:
  void FrameStep();
  void CropSprite();
  void ReadIn(std::stringstream &stream);
};

#endif // !DIRECTIONAL_ANIMATION_H
