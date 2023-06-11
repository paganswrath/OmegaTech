#include "raylib.h"
#include "Settings.hpp"
#include "Player.hpp"
#include "Editor.hpp"
#include "Video.hpp"
#include "ParticleDemon/ParticleDemon.hpp"
#include "Parasite/ParasiteScript.hpp"
#include "Encoder/Encoder.hpp"
#include "Entities.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <utility>

using namespace std;

#define MaxCachedModels 200


static wstring WorldData;
static wstring OtherWDLData;


static int R = 0;
static int G = 0;
static int B = 0;

int Direction = 1;
bool FadeDone = false;
Color FadeColor = (Color){R, G, B, 255};

void PlayFade()
{
    Direction = 1;
    R = 0;
    G = 0;
    B = 0;
    FadeDone = false;
}   



class GameModels
{
    public:
        Texture2D Skybox;
        
        Vector3 HeightMapPosition;

        // OBJ Models

        Model HeightMap;
        Texture2D HeightMapTexture;

        Model Model1;
        Texture2D Model1Texture;

        Model Model2;
        Texture2D Model2Texture;

        Model Model3;
        Texture2D Model3Texture;

        Model Model4;
        Texture2D Model4Texture;

        Model Model5;
        Texture2D Model5Texture;

        Model Model6;
        Texture2D Model6Texture;

        Model Model7;
        Texture2D Model7Texture;

        Model Model8;
        Texture2D Model8Texture;

        Model Model9;
        Texture2D Model9Texture;

        Model Model10;
        Texture2D Model10Texture;

        Model Model11;
        Texture2D Model11Texture;

        Model Model12;
        Texture2D Model12Texture;

        Model Model13;
        Texture2D Model13Texture;

        Model Model14;
        Texture2D Model14Texture;

        Model Model15;
        Texture2D Model15Texture;

        Model Model16;
        Texture2D Model16Texture;

        Model Model17;
        Texture2D Model17Texture;

        Model Model18;
        Texture2D Model18Texture;

        Model Model19;
        Texture2D Model19Texture;

        Model Model20;
        Texture2D Model20Texture;

        // GLTS Models

        // For Faster Rendering.

        Model FastModel1;
        Texture2D FastModel1Texture;
        Model FastModel2;
        Texture2D FastModel2Texture;
        Model FastModel3;
        Texture2D FastModel3Texture;
        Model FastModel4;
        Texture2D FastModel4Texture;
        Model FastModel5;
        Texture2D FastModel5Texture;
};

static GameModels WDLModels;

class GameData{
    public:
        float X;
        float Y;
        float Z;
        float R;
        float S;
        int ModelId;
        bool Collision;

        void Init(){
            X = 0;
            Y = 0;
            Z = 0;
            R = 0;
            S = 0;
            ModelId = 0;
            Collision = false;
        }

};

static int CachedModelCounter = 0;
static GameData CachedModels[MaxCachedModels];

class CollisionData{
    public:
        float X;
        float Y;
        float Z;
        float W;
        float H;
        float L;

        void Init(){
            X = 0;
            Y = 0;
            Z = 0;
            W = 0;
            H = 0;
            L = 0;
        }

};

static int CachedCollisionCounter = 0;
static CollisionData CachedCollision[MaxCachedModels];

class GameSounds
{
    public:
        Sound CollisionSound;
        Sound WalkingSound;
        Music BackgroundMusic;
        Sound UIClick;
        Sound ChasingSound;
        Sound Death;

        Music NESound1;
        Music NESound2;
        Music NESound3;

        bool MusicFound = false;
};

static GameSounds OmegaTechSoundData;
