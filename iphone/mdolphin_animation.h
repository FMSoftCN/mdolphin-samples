
#ifndef mdolphin_animation_h
#define mdolphin_animation_h

#define MYANIMATION_CTRL  "md_animation"
#define FRAME_TIMER   0x100

BOOL RegisterMyAnimation();
void UnregisterMyAnimation();

HWND CreateAnimationWindow(HWND hParent);
void SetAnimationCtxt(HWND hwnd, void* ctx);

#endif // mdolphin_animation_h
