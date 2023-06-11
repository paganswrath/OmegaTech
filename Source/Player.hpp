#include "raylib.h"

class Player{
    public:
        float Height = 10.0f;
        float Width = 2.0f;

        int HeadBob = 0;
        int HeadBobDirection = 1;

        int Health = 100;

        float OldX = 0.0f;
        float OldY = 0.0f;
        float OldZ = 0.0f;

        BoundingBox PlayerBounds;
};

static Player OmegaPlayer;