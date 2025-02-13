/**
 * structure for keeping track of current character animation
 * save variables for right draw
 * each character got this structure
 */
typedef struct AnimatorBaseData
{
    int speedMultiplier;
    int currentFrame;
} AnimatorBaseData;

typedef struct Animator3D
{
    AnimatorBaseData data;
    AnimatorDir* animation;
} AnimatorDir;