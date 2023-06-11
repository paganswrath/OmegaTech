#include "raylib.h"

#define EntityCount 10

class Enemys{
    public:
        bool IsActive = false;
        bool Scream = false;

        float Height = 10.0f;
        float Width = 2.0f;

        float X = 0.0f;
        float Y = 0.0f;
        float Z = 0.0f;

        float OldX = 0.0f;
        float OldY = 0.0f;
        float OldZ = 0.0f;

};

static Enemys OmegaEnemy[10];


class EnemyTexture{
    public:
        Texture2D Frame1;
        Sound Scream;
};

static EnemyTexture EnemyTextures;