#include "Data.hpp"

#include "raymath.h"
#include "rlights/rlights.h"

bool FloorCollision = true;
bool ObjectCollision = false;

void LoadSave();
void SaveGame();
void UpdateCustom();
void CacheWDL();
void PutLight(Vector3 Position);
void ClearLights();

class ParticleSystem;

class EngineData
{
public:
    int LevelIndex = 1;
    Camera MainCamera = {0};
    Shader PixelShader;
    Shader FogShader;
    Shader LineShader;
    Shader ToonShader;
    Shader SobelShader;
    Shader Lights;
    Light GameLights[MAX_LIGHTS];

    ParticleSystem RainParticles;
    Texture HomeScreen;
    ray_video_t HomeScreenVideo;
    Music HomeScreenMusic;

    ray_video_t Ending1Video;
    ray_video_t Ending2Video;
    Sound Ending2Music;

    bool FirstLoad = true;

    int Ticker = 0;
    int CameraSpeed = 1;
    int RenderRadius = 800;
    int Deaths;
    bool UseCachedRenderer = false;
    int BadPreformaceCounter = 0;
    bool SkyboxEnabled = false;
    int Ending = 0;
    int PanicCounter = 0;

    void InitCamera()
    {
        MainCamera.position = (Vector3){0.0f, 0.0f, 0.0f};
        MainCamera.target = (Vector3){0.0f, 10.0f, 0.0f};
        MainCamera.up = (Vector3){0.0f, 1.0f, 0.0f};      
        MainCamera.fovy = 60.0f;                        
        MainCamera.projection = CAMERA_PERSPECTIVE;
    }
};

static EngineData OmegaTechData;

void SpawnWDLProcess(const char *Path)
{
    wstring WData;

    if (GameDataEncoded)
    {
        WData = Encode(LoadFile(Path), MainKey);
    }
    else
    {
        WData = LoadFile(Path);
    }

    int WDLSize = 0;

    for (int i = 0; i <= WData.size(); i++)
    {
        if (WData[i] == L':')
        {
            WDLSize++;
        }
    }

    int EntityCounter = 0;

    for (int i = 0; i <= WDLSize; i++)
    {
        wstring Instruction = WSplitValue(WData, i);

        if (WReadValue(Instruction, 0, 5) == L"Walker")
        {
            OmegaEnemy[EntityCounter].X = ToFloat(WSplitValue(WData, i + 1));
            OmegaEnemy[EntityCounter].Y = ToFloat(WSplitValue(WData, i + 2));
            OmegaEnemy[EntityCounter].Z = ToFloat(WSplitValue(WData, i + 3));
            OmegaEnemy[EntityCounter].IsActive = true;
            OmegaEnemy[EntityCounter].Scream = false;
            EntityCounter++;
        }
        if (Instruction == L"Light")
        {
            PutLight({ToFloat(WSplitValue(WData, i + 1)) , ToFloat(WSplitValue(WData, i + 2)) , ToFloat(WSplitValue(WData, i + 3))});
        }
        i += 3;
    }
}

bool LoadFlag = false;

auto LoadWorld()
{
    PlayFade();
    ClearLights();


    if (OmegaTechData.Deaths != 3)
    {

        OmegaTechData.PanicCounter = 0;

        OmegaPlayer.Health = 100;

        OmegaTechData.SkyboxEnabled = false;

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE1.mp3", OmegaTechData.LevelIndex)))
        {
            StopMusicStream(OmegaTechSoundData.NESound1);
            OmegaTechSoundData.NESound1 = LoadMusicStream(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE1.mp3", OmegaTechData.LevelIndex));
        }
        else {
            UnloadMusicStream(OmegaTechSoundData.NESound1);
        }
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE2.mp3", OmegaTechData.LevelIndex)))
        {
            StopMusicStream(OmegaTechSoundData.NESound2);
            OmegaTechSoundData.NESound2 = LoadMusicStream(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE2.mp3", OmegaTechData.LevelIndex));
        }
        else {
            UnloadMusicStream(OmegaTechSoundData.NESound2);
        }
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE3.mp3", OmegaTechData.LevelIndex)))
        {
            StopMusicStream(OmegaTechSoundData.NESound3);
            OmegaTechSoundData.NESound3 = LoadMusicStream(TextFormat("GameData/Worlds/World%i/NoiseEmitter/NE3.mp3", OmegaTechData.LevelIndex));
        }
        else {
            UnloadMusicStream(OmegaTechSoundData.NESound3);
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Skybox.png", OmegaTechData.LevelIndex)))
        {

            WDLModels.Skybox = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Skybox.png", OmegaTechData.LevelIndex));
            OmegaTechData.SkyboxEnabled = true;
        }

        for (int i = 0; i <= EntityCount - 1; i++)
        {
            OmegaEnemy[i].IsActive = false;
            OmegaEnemy[i].Scream = false;
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Scripts/Launch.ps", OmegaTechData.LevelIndex)))
        {
            ParasiteScriptInit();
            LoadScript(TextFormat("GameData/Worlds/World%i/Scripts/Launch.ps", OmegaTechData.LevelIndex));
            for (int x = 0; x <= ParasiteScriptCoreData.ProgramSize; x++)
            {
                CycleInstruction();
                ParasiteScriptCoreData.LineCounter++;
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Entities/Walker/Frame1.png", OmegaTechData.LevelIndex)))
        {
            EnemyTextures.Frame1 = LoadTexture(TextFormat("GameData/Worlds/World%i/Entities/Walker/Frame1.png", OmegaTechData.LevelIndex));
            EnemyTextures.Scream = LoadSound(TextFormat("GameData/Worlds/World%i/Entities/Walker/Scream.mp3", OmegaTechData.LevelIndex));
            SpawnWDLProcess(TextFormat("GameData/Worlds/World%i/Entities/Entities.wdl", OmegaTechData.LevelIndex));
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/HeightMap.png", OmegaTechData.LevelIndex)))
        {
            WDLModels.HeightMapTexture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/HeightMapTexture.png", OmegaTechData.LevelIndex));
            Image HeightMapImage = LoadImage(TextFormat("GameData/Worlds/World%i/Models/HeightMap.png", OmegaTechData.LevelIndex));
            Texture2D Texture = LoadTextureFromImage(HeightMapImage);
            int X = PullConfigValue(TextFormat("GameData/Worlds/World%i/Models/HeightMapConfig.conf", OmegaTechData.LevelIndex), 0);
            int Y = PullConfigValue(TextFormat("GameData/Worlds/World%i/Models/HeightMapConfig.conf", OmegaTechData.LevelIndex), 1);
            int Z = PullConfigValue(TextFormat("GameData/Worlds/World%i/Models/HeightMapConfig.conf", OmegaTechData.LevelIndex), 2);
            Mesh Mesh1 = GenMeshHeightmap(HeightMapImage, (Vector3){X, Y, Z});
            WDLModels.HeightMap = LoadModelFromMesh(Mesh1);
            WDLModels.HeightMap.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.HeightMapTexture;
            WDLModels.HeightMap.meshes[0] = Mesh1;
            WDLModels.HeightMap.materials[0].shader = OmegaTechData.Lights;
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model1.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model1 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model1.obj", OmegaTechData.LevelIndex));
            WDLModels.Model1Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model1Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model1.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model1Texture;
            WDLModels.Model1.materials[0].shader = OmegaTechData.Lights;
        }
        else {
            if (WDLModels.Model1.meshCount != 0){
                UnloadModel(WDLModels.Model1);
            }
            if (WDLModels.Model1Texture.id != 0){
                UnloadTexture(WDLModels.Model1Texture);
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model2.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model2 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model2.obj", OmegaTechData.LevelIndex));
            WDLModels.Model2Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model2Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model2.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model2Texture;
            WDLModels.Model2.materials[0].shader = OmegaTechData.Lights;
        }
        else {
            if (WDLModels.Model2.meshCount != 0){
                UnloadModel(WDLModels.Model2);
            }
            if (WDLModels.Model2Texture.id != 0){
                UnloadTexture(WDLModels.Model2Texture);
            }
        }
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model3.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model3 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model3.obj", OmegaTechData.LevelIndex));
            WDLModels.Model3Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model3Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model3.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model3Texture;
            WDLModels.Model3.materials[0].shader = OmegaTechData.Lights;
        }
        else {
            if (WDLModels.Model3.meshCount != 0){
                UnloadModel(WDLModels.Model13);
            }
            if (WDLModels.Model3Texture.id != 0){
                UnloadTexture(WDLModels.Model3Texture);
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model4.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model4 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model4.obj", OmegaTechData.LevelIndex));
            WDLModels.Model4Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model4Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model4.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model4Texture;
            WDLModels.Model4.materials[0].shader = OmegaTechData.Lights;
        }
        else {
            if (WDLModels.Model4.meshCount != 0){
                UnloadModel(WDLModels.Model4);
            }
            if (WDLModels.Model4Texture.id != 0){
                UnloadTexture(WDLModels.Model4Texture);
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model5.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model5 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model5.obj", OmegaTechData.LevelIndex));
            WDLModels.Model5Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model5Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model5.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model5Texture;
        }
        else {
            if (WDLModels.Model5.meshCount != 0){
                UnloadModel(WDLModels.Model5);
            }
            if (WDLModels.Model5Texture.id != 0){
                UnloadTexture(WDLModels.Model5Texture);
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model6.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model6 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model6.obj", OmegaTechData.LevelIndex));
            WDLModels.Model6Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model6Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model6.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model6Texture;
            WDLModels.Model6.materials[0].shader = OmegaTechData.Lights;
        }
        else {
            if (WDLModels.Model6.meshCount != 0){
                UnloadModel(WDLModels.Model6);
            }
            if (WDLModels.Model6Texture.id != 0){
                UnloadTexture(WDLModels.Model6Texture);
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model7.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model7 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model7.obj", OmegaTechData.LevelIndex));
            WDLModels.Model7Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model7Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model7.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model7Texture;
            WDLModels.Model7.materials[0].shader = OmegaTechData.Lights;

        }
        else {
            if (WDLModels.Model7.meshCount != 0){
                UnloadModel(WDLModels.Model7);
            }
            if (WDLModels.Model7Texture.id != 0){
                UnloadTexture(WDLModels.Model7Texture);
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model8.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model8 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model8.obj", OmegaTechData.LevelIndex));
            WDLModels.Model8Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model8Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model8.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model8Texture;
            WDLModels.Model8.materials[0].shader = OmegaTechData.Lights;

        }
        else {
            if (WDLModels.Model8.meshCount != 0){
                UnloadModel(WDLModels.Model8);
            }
            if (WDLModels.Model8Texture.id != 0){
                UnloadTexture(WDLModels.Model8Texture);
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model9.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model9 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model9.obj", OmegaTechData.LevelIndex));
            WDLModels.Model9Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model9Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model9.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model9Texture;
            WDLModels.Model9.materials[0].shader = OmegaTechData.Lights;

        }
        else {
            if (WDLModels.Model9.meshCount != 0){
                UnloadModel(WDLModels.Model9);
            }
            if (WDLModels.Model9Texture.id != 0){
                UnloadTexture(WDLModels.Model9Texture);
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model10.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model10 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model10.obj", OmegaTechData.LevelIndex));
            WDLModels.Model10Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model10Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model10.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model10Texture;
            WDLModels.Model10.materials[0].shader = OmegaTechData.Lights;

        }
        else {
            if (WDLModels.Model10.meshCount != 0){
                UnloadModel(WDLModels.Model10);
            }
            if (WDLModels.Model10Texture.id != 0){
                UnloadTexture(WDLModels.Model10Texture);
            }
        }
        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model11.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model11 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model11.obj", OmegaTechData.LevelIndex));
            WDLModels.Model11Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model11Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model11.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model11Texture;
            WDLModels.Model11.materials[0].shader = OmegaTechData.Lights;

        }
        else {
            if (WDLModels.Model11.meshCount != 0){
                UnloadModel(WDLModels.Model11);
            }
            if (WDLModels.Model11Texture.id != 0){
                UnloadTexture(WDLModels.Model11Texture);
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model12.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model12 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model12.obj", OmegaTechData.LevelIndex));
            WDLModels.Model12Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model12Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model12.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model12Texture;
            WDLModels.Model12.materials[0].shader = OmegaTechData.Lights;

        }
        else {
            if (WDLModels.Model12.meshCount != 0){
                UnloadModel(WDLModels.Model12);
            }
            if (WDLModels.Model12Texture.id != 0){
                UnloadTexture(WDLModels.Model12Texture);
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model13.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model13 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model13.obj", OmegaTechData.LevelIndex));
            WDLModels.Model13Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model13Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model13.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model13Texture;
            WDLModels.Model13.materials[0].shader = OmegaTechData.Lights;

        }
        else {
            if (WDLModels.Model13.meshCount != 0){
                UnloadModel(WDLModels.Model13);
            }
            if (WDLModels.Model13Texture.id != 0){
                UnloadTexture(WDLModels.Model13Texture);
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model14.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model14 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model14.obj", OmegaTechData.LevelIndex));
            WDLModels.Model14Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model14Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model14.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model14Texture;
            WDLModels.Model14.materials[0].shader = OmegaTechData.Lights;

        }
        else {
            if (WDLModels.Model14.meshCount != 0){
                UnloadModel(WDLModels.Model14);
            }
            if (WDLModels.Model14Texture.id != 0){
                UnloadTexture(WDLModels.Model14Texture);
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model15.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model15 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model15.obj", OmegaTechData.LevelIndex));
            WDLModels.Model15Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model15Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model15.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model15Texture;
            WDLModels.Model15.materials[0].shader = OmegaTechData.Lights;

        }
        else {
            if (WDLModels.Model15.meshCount != 0){
                UnloadModel(WDLModels.Model15);
            }
            if (WDLModels.Model15Texture.id != 0){
                UnloadTexture(WDLModels.Model15Texture);
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model16.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model16 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model16.obj", OmegaTechData.LevelIndex));
            WDLModels.Model16Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model16Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model16.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model16Texture;
            WDLModels.Model16.materials[0].shader = OmegaTechData.Lights;

        }
        else {
            if (WDLModels.Model16.meshCount != 0){
                UnloadModel(WDLModels.Model16);
            }
            if (WDLModels.Model16Texture.id != 0){
                UnloadTexture(WDLModels.Model16Texture);
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model17.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model17 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model17.obj", OmegaTechData.LevelIndex));
            WDLModels.Model17Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model17Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model17.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model17Texture;
            WDLModels.Model17.materials[0].shader = OmegaTechData.Lights;

        }
        else {
            if (WDLModels.Model17.meshCount != 0){
                UnloadModel(WDLModels.Model17);
            }
            if (WDLModels.Model17Texture.id != 0){
                UnloadTexture(WDLModels.Model17Texture);
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model18.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model18 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model18.obj", OmegaTechData.LevelIndex));
            WDLModels.Model18Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model18Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model18.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model18Texture;
            WDLModels.Model18.materials[0].shader = OmegaTechData.Lights;

        }
        else {
            if (WDLModels.Model18.meshCount != 0){
                UnloadModel(WDLModels.Model18);
            }
            if (WDLModels.Model18Texture.id != 0){
                UnloadTexture(WDLModels.Model18Texture);
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model19.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model19 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model19.obj", OmegaTechData.LevelIndex));
            WDLModels.Model19Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model19Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model19.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model19Texture;
            WDLModels.Model19.materials[0].shader = OmegaTechData.Lights;

        }
        else {
            if (WDLModels.Model19.meshCount != 0){
                UnloadModel(WDLModels.Model19);
            }
            if (WDLModels.Model19Texture.id != 0){
                UnloadTexture(WDLModels.Model19Texture);
            }
        }

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Models/Model20.obj", OmegaTechData.LevelIndex)))
        {
            WDLModels.Model20 = LoadModel(TextFormat("GameData/Worlds/World%i/Models/Model20.obj", OmegaTechData.LevelIndex));
            WDLModels.Model20Texture = LoadTexture(TextFormat("GameData/Worlds/World%i/Models/Model20Texture.png", OmegaTechData.LevelIndex));
            WDLModels.Model20.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.Model20Texture;
            WDLModels.Model20.materials[0].shader = OmegaTechData.Lights;

        }
        else {
            if (WDLModels.Model20.meshCount != 0){
                UnloadModel(WDLModels.Model20);
            }
            if (WDLModels.Model20Texture.id != 0){
                UnloadTexture(WDLModels.Model20Texture);
            }
        }

        if (GameDataEncoded)
        {
            WorldData = Encode(LoadFile(TextFormat("GameData/Worlds/World%i/World.wdl", OmegaTechData.LevelIndex)), MainKey);
        }
        else
        {
            WorldData = L"";
            WorldData = LoadFile(TextFormat("GameData/Worlds/World%i/World.wdl", OmegaTechData.LevelIndex));
            OtherWDLData = L"";
            CacheWDL();
        }

        if (OmegaTechSoundData.MusicFound)StopMusicStream(OmegaTechSoundData.BackgroundMusic);

        OmegaTechSoundData.MusicFound = false;

        if (IsPathFile(TextFormat("GameData/Worlds/World%i/Music/Main.mp3", OmegaTechData.LevelIndex)))
        {
            OmegaTechSoundData.BackgroundMusic = LoadMusicStream(TextFormat("GameData/Worlds/World%i/Music/Main.mp3", OmegaTechData.LevelIndex));
            OmegaTechSoundData.MusicFound = true;
            PlayMusicStream(OmegaTechSoundData.BackgroundMusic);
        }

        SaveGame();
    }
}

void LoadLaunchConfig()
{
    wstring Config = LoadFile("GameData/Launch.conf");

    wstring Resolution = WSplitValue(Config, 0);

    switch (Resolution[0])
    {
    case L'1':
        SetWindowSize(640, 480);
        break;
    case L'2':
        SetWindowSize(1280, 720);
        break;
    case L'3':
        SetWindowSize(1980, 1080);
        break;
    case L'4':
        SetWindowSize(2560, 1440);
        break;
    case L'5':
        SetWindowSize(3840, 2160);
        break;
    default:
        SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
        ToggleFullscreen();
        break;
    }
}
int LightCounter = 1;

void UpdateLightSources(){
    float cameraPos[3] = { OmegaTechData.MainCamera.position.x, OmegaTechData.MainCamera.position.y, OmegaTechData.MainCamera.position.z };
    OmegaTechData.GameLights[0].position = { OmegaTechData.MainCamera.position.x, OmegaTechData.MainCamera.position.y, OmegaTechData.MainCamera.position.z };
    OmegaTechData.GameLights[0].target = { OmegaTechData.MainCamera.target.x, OmegaTechData.MainCamera.target.y - 5, OmegaTechData.MainCamera.target.z };
    SetShaderValue(OmegaTechData.Lights, OmegaTechData.Lights.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
    for (int i = 0; i < MAX_LIGHTS - 1; i++) UpdateLightValues(OmegaTechData.Lights, OmegaTechData.GameLights[i]);
}

void ClearLights(){
    LightCounter = 1;
    OmegaTechData.GameLights[MAX_LIGHTS] = { 0 };
}

void PutLight(Vector3 Position){
    OmegaTechData.GameLights[LightCounter] = CreateLight(LIGHT_POINT, Position, Vector3Zero(), RED, OmegaTechData.Lights);
    LightCounter ++;
}

void DrawLights(){
    for (int i = 1; i < MAX_LIGHTS; i++)
    {
        if (OmegaTechData.GameLights[i].enabled) DrawSphereEx(OmegaTechData.GameLights[i].position, 0.2f, 8, 8, OmegaTechData.GameLights[i].color);
        else DrawSphereWires(OmegaTechData.GameLights[i].position, 0.2f, 8, 8, ColorAlpha(OmegaTechData.GameLights[i].color, 0.3f));
    }
}

void OmegaTechInit()
{
    LoadLaunchConfig();
    ParasiteScriptTFlagWipe();

    GuiLoadStyleDark();

    OmegaTechData.InitCamera();
    OmegaTechData.PixelShader = LoadShader(0, "GameData/Shaders/Pixel.fs");
    OmegaTechData.FogShader = LoadShader(0, "GameData/Shaders/Fog.fs");
    OmegaTechData.LineShader = LoadShader(0, "GameData/Shaders/Scanlines.fs");
    OmegaTechData.SobelShader = LoadShader(0, "GameData/Shaders/Sobel.fs");
    OmegaTechData.ToonShader = LoadShader(0, "GameData/Shaders/Toon.fs");
    OmegaTechData.Lights = LoadShader("GameData/Shaders/Lights/Lighting.vs","GameData/Shaders/Lights/Lighting.fs");
    OmegaTechData.GameLights[MAX_LIGHTS] = { 0 };


    OmegaTechData.Lights.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(OmegaTechData.Lights, "viewPos");
    int AmbientLoc = GetShaderLocation(OmegaTechData.Lights, "ambient");
    float ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    SetShaderValue(OmegaTechData.Lights, AmbientLoc, ambient, SHADER_UNIFORM_VEC4);


    OmegaTechData.HomeScreen = LoadTexture("GameData/Global/Title/Title.png");
    OmegaTechData.HomeScreenVideo = ray_video_open("GameData/Global/Title/Title.mpg");
    OmegaTechData.HomeScreenMusic = LoadMusicStream("GameData/Global/Title/Title.mp3");

    OmegaTechTextSystem.Bar = LoadTexture("GameData/Global/TextBar.png");
    OmegaTechTextSystem.BarFont = LoadFont("GameData/Global/Font.ttf");
    OmegaTechSoundData.CollisionSound = LoadSound("GameData/Global/Sounds/CollisionSound.mp3");
    OmegaTechSoundData.WalkingSound = LoadSound("GameData/Global/Sounds/WalkingSound.mp3");
    OmegaTechSoundData.ChasingSound = LoadSound("GameData/Global/Sounds/ChasingSound.mp3");
    OmegaTechSoundData.UIClick = LoadSound("GameData/Global/Title/Click.mp3");
    OmegaTechSoundData.Death = LoadSound("GameData/Global/Sounds/Hurt.mp3");

    OmegaTechTextSystem.TextNoise = LoadSound("GameData/Global/Sounds/TalkingNoise.mp3");

    if (IsPathFile("GameData/Global/FModels/FModel1.gltf"))
    {
        WDLModels.FastModel1 = LoadModel("GameData/Global/FModels/FModel1.gltf");
        WDLModels.FastModel1Texture = LoadTexture("GameData/Global/FModels/FModel1Texture.png");
        WDLModels.FastModel1.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.FastModel1Texture;
        WDLModels.FastModel1.materials[0].shader = OmegaTechData.Lights;
    }
    if (IsPathFile("GameData/Global/FModels/FModel2.gltf"))
    {
        WDLModels.FastModel2 = LoadModel("GameData/Global/FModels/FModel2.gltf");
        WDLModels.FastModel2Texture = LoadTexture("GameData/Global/FModels/FModel2Texture.png");
        WDLModels.FastModel2.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.FastModel2Texture;
        WDLModels.FastModel2.materials[0].shader = OmegaTechData.Lights;
    }
    if (IsPathFile("GameData/Global/FModels/FModel3.gltf"))
    {
        WDLModels.FastModel3 = LoadModel("GameData/Global/FModels/FModel3.gltf");
        WDLModels.FastModel3Texture = LoadTexture("GameData/Global/FModels/FModel3Texture.png");
        WDLModels.FastModel3.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.FastModel3Texture;
        WDLModels.FastModel3.materials[0].shader = OmegaTechData.Lights;
    }
    if (IsPathFile("GameData/Global/FModels/FModel4.gltf"))
    {
        WDLModels.FastModel4 = LoadModel("GameData/Global/FModels/FModel4.gltf");
        WDLModels.FastModel4Texture = LoadTexture("GameData/Global/FModels/FModel4Texture.png");
        WDLModels.FastModel4.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.FastModel4Texture;
        WDLModels.FastModel4.materials[0].shader = OmegaTechData.Lights;
    }
    if (IsPathFile("GameData/Global/FModels/FModel5.gltf"))
    {
        WDLModels.FastModel5 = LoadModel("GameData/Global/FModels/FModel5.gltf");
        WDLModels.FastModel5Texture = LoadTexture("GameData/Global/FModels/FModel5Texture.png");
        WDLModels.FastModel5.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = WDLModels.FastModel5Texture;
        WDLModels.FastModel5.materials[0].shader = OmegaTechData.Lights;
    }
    Target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    InitObjects();

    PlayMusicStream(OmegaTechData.HomeScreenMusic);
}

void PlayHomeScreen()
{
    Rectangle LayoutRecs[5] = {
        (Rectangle){583, 355, 120, 24},
        (Rectangle){583, 421, 120, 24},
        (Rectangle){583, 387, 120, 24},
        (Rectangle){583, 489, 120, 24},
        (Rectangle){583, 454, 120, 24},
    };

    while (true && !WindowShouldClose())
    {
        BeginTextureMode(Target);
        UpdateMusicStream(OmegaTechData.HomeScreenMusic);

        ClearBackground(BLACK);

        ray_video_update(&OmegaTechData.HomeScreenVideo, GetFrameTime());

        DrawTextureEx(OmegaTechData.HomeScreenVideo.texture, {0, 0}, 0, 5, WHITE);
        DrawTexture(OmegaTechData.HomeScreen, 0, 0, WHITE);

        if (OmegaInputController.InteractPressed)
        {
            PlaySound(OmegaTechSoundData.UIClick);
        }

        if (GuiButton(LayoutRecs[0], "Start New Game"))
        {
            UnloadRenderTexture(Target);
            Target = LoadRenderTexture(GetScreenWidth() / 4, GetScreenHeight() / 4);

            break;
        }

        if (GuiButton(LayoutRecs[1], "Load Save"))
        {
            UnloadRenderTexture(Target);
            Target = LoadRenderTexture(GetScreenWidth() / 4, GetScreenHeight() / 4);

            if (IsPathFile("GameData/Saves/TF.sav"))
            {
                LoadSave();
                LoadFlag = true;
            }
            break;
        }
        GuiLine(LayoutRecs[2], NULL);
        if (GuiButton(LayoutRecs[3], "Settings"))
        {
            if (MenuSettings)
            {
                MenuSettings = false;
            }
            else
            {
                MenuSettings = true;
            }
        }

        GuiLine(LayoutRecs[4], NULL);

        if (MenuSettings)
        {
            ShowMenuSetiings();
        }
        EndTextureMode();
        BeginDrawing();
        // BeginShaderMode(OmegaTechData.LineShader);
        DrawTexturePro(Target.texture, (Rectangle){0, 0, Target.texture.width, -Target.texture.height}, (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()}, (Vector2){0, 0}, 0.f, WHITE);
        // EndShaderMode();
        EndDrawing();

        if (IsKeyPressed(KEY_ESCAPE))exit(0);
    }
    StopMusicStream(OmegaTechData.HomeScreenMusic);
    OmegaTechData.Deaths = 1;
}

static int ScriptTimer = 0;
static float X, Y, Z, S, Rotation, W, H, L;
bool NextCollision = false;

void CacheWDL()
{
    wstring WData = WorldData;

    OtherWDLData = L"";

    CachedModelCounter = 0;
    CachedCollisionCounter = 0;

    bool NextCollision = false;

    for (int i = 0; i <= MaxCachedModels - 1; i++)
    {
        CachedModels[i].Init();
        CachedCollision[i].Init();
    }

    for (int i = 0; i <= GetWDLSize(WorldData, L""); i++)
    {

        if (CachedModelCounter == MaxCachedModels)
            break;

        wstring Instruction = WSplitValue(WData, i);

        if (WReadValue(Instruction, 0, 4) == L"Model" || WReadValue(Instruction, 0, 8) == L"HeightMap")
        {
            if (WReadValue(Instruction, 0, 8) != L"HeightMap")
            {
                CachedModels[CachedModelCounter].ModelId = int(ToFloat(WReadValue(Instruction, 5, 6)));
            }
            else
            {
                CachedModels[CachedModelCounter].ModelId = -1;
            }
            
            CachedModels[CachedModelCounter].X = ToFloat(WSplitValue(WData, i + 1));
            CachedModels[CachedModelCounter].Y = ToFloat(WSplitValue(WData, i + 2));
            CachedModels[CachedModelCounter].Z = ToFloat(WSplitValue(WData, i + 3));
            CachedModels[CachedModelCounter].S = ToFloat(WSplitValue(WData, i + 4));
            CachedModels[CachedModelCounter].R = ToFloat(WSplitValue(WData, i + 5));

            if (NextCollision)
            {
                CachedModels[CachedModelCounter].Collision = true;
                NextCollision = false;
            }

            CachedModelCounter++;
        }

        if (WReadValue(Instruction, 0, 8) == L"Collision")
        {
            CachedModels[CachedModelCounter].ModelId = -2;
            CachedModels[CachedModelCounter].X = ToFloat(WSplitValue(WData, i + 1));
            CachedModels[CachedModelCounter].Y = ToFloat(WSplitValue(WData, i + 2));
            CachedModels[CachedModelCounter].Z = ToFloat(WSplitValue(WData, i + 3));
            CachedModels[CachedModelCounter].S = ToFloat(WSplitValue(WData, i + 4));
            CachedModels[CachedModelCounter].R = ToFloat(WSplitValue(WData, i + 5));
        }

        if (WReadValue(Instruction, 0, 11) == L"AdvCollision")
        {
            //OtherWDLData += WSplitValue(WData, i) + L":" + WSplitValue(WData, i + 1) + L":" + WSplitValue(WData, i + 2) + L":" + WSplitValue(WData, i + 3) + L":" + WSplitValue(WData, i + 4) + L":" + WSplitValue(WData, i + 5) + L":" + WSplitValue(WData, i + 6) + L":" + WSplitValue(WData, i + 7) + L":" + WSplitValue(WData, i + 8) + L":";
            CachedCollision[CachedCollisionCounter].X =  ToFloat(WSplitValue(WData, i + 1));
            CachedCollision[CachedCollisionCounter].Y =  ToFloat(WSplitValue(WData, i + 2));
            CachedCollision[CachedCollisionCounter].Z =  ToFloat(WSplitValue(WData, i + 3));
            CachedCollision[CachedCollisionCounter].W =  ToFloat(WSplitValue(WData, i + 6));
            CachedCollision[CachedCollisionCounter].H =  ToFloat(WSplitValue(WData, i + 7));
            CachedCollision[CachedCollisionCounter].L =  ToFloat(WSplitValue(WData, i + 8));
            CachedCollisionCounter ++;
        }

        if (WReadValue(Instruction, 0, 5) == L"Object" || WReadValue(Instruction, 0, 5) == L"Script") // Dont Cache Dynamic Objs
        {
            OtherWDLData += WSplitValue(WData, i) + L":" + WSplitValue(WData, i + 1) + L":" + WSplitValue(WData, i + 2) + L":" + WSplitValue(WData, i + 3) + L":" + WSplitValue(WData, i + 4) + L":" + WSplitValue(WData, i + 5) + L":";
        }

        if (Instruction == L"C")
        {
            NextCollision = true;
        }
    }
}

void CWDLProcess()
{
    for (int i = 0; i <= CachedCollisionCounter; i++)
    {
        X = CachedCollision[i].X;
        Y = CachedCollision[i].Y;
        Z = CachedCollision[i].Z;
        W = CachedCollision[i].W;
        H = CachedCollision[i].H;
        L = CachedCollision[i].L;

        if (OmegaTechData.MainCamera.position.z - OmegaTechData.RenderRadius < Z && OmegaTechData.MainCamera.position.z + OmegaTechData.RenderRadius > Z || CachedModels[i].ModelId == -1)
        {
            if (OmegaTechData.MainCamera.position.x - OmegaTechData.RenderRadius < X && OmegaTechData.MainCamera.position.x + OmegaTechData.RenderRadius > X || CachedModels[i].ModelId == -1)
            {
                if (CheckCollisionBoxSphere((BoundingBox){(Vector3){X, Y, Z}, (Vector3){W, H, L}},{OmegaTechData.MainCamera.position.x + OmegaPlayer.Width / 2,OmegaTechData.MainCamera.position.y - OmegaPlayer.Height / 2,OmegaTechData.MainCamera.position.z - OmegaPlayer.Width / 2},1.0)){
                    ObjectCollision = true;
                    if (!IsSoundPlaying(OmegaTechSoundData.CollisionSound))
                    {
                        PlaySound(OmegaTechSoundData.CollisionSound);
                    }
                }
            }
        }
    }
    
    for (int i = 0; i <= CachedModelCounter; i++)
    {
        X = CachedModels[i].X;
        Y = CachedModels[i].Y;
        Z = CachedModels[i].Z;
        S = CachedModels[i].S;
        Rotation = CachedModels[i].R;

        if (OmegaTechData.MainCamera.position.z - OmegaTechData.RenderRadius < Z && OmegaTechData.MainCamera.position.z + OmegaTechData.RenderRadius > Z || CachedModels[i].ModelId == -1)
        {
            if (OmegaTechData.MainCamera.position.x - OmegaTechData.RenderRadius < X && OmegaTechData.MainCamera.position.x + OmegaTechData.RenderRadius > X || CachedModels[i].ModelId == -1)
            {

                switch (CachedModels[i].ModelId)
                {
                case -2:
                    if (CheckCollisionBoxes(OmegaPlayer.PlayerBounds, (BoundingBox){(Vector3){X, Y, Z}, (Vector3){X + S, Y + S, Z + S}}))
                    {
                        ObjectCollision = true;
                        if (!IsSoundPlaying(OmegaTechSoundData.CollisionSound))
                        {
                            PlaySound(OmegaTechSoundData.CollisionSound);
                        }
                    }
                    break;
                case -1:
                    DrawModelEx(WDLModels.HeightMap, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 1:
                    DrawModelEx(WDLModels.Model1, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 2:
                    DrawModelEx(WDLModels.Model2, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 3:
                    DrawModelEx(WDLModels.Model3, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 4:
                    DrawModelEx(WDLModels.Model4, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 5:
                    DrawModelEx(WDLModels.Model5, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 6:
                    DrawModelEx(WDLModels.Model6, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 7:
                    DrawModelEx(WDLModels.Model7, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 8:
                    DrawModelEx(WDLModels.Model8, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 9:
                    DrawModelEx(WDLModels.Model9, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 10:
                    DrawModelEx(WDLModels.Model10, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 11:
                    DrawModelEx(WDLModels.Model11, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 12:
                    DrawModelEx(WDLModels.Model12, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 13:
                    DrawModelEx(WDLModels.Model13, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 14:
                    DrawModelEx(WDLModels.Model14, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 15:
                    DrawModelEx(WDLModels.Model15, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 16:
                    DrawModelEx(WDLModels.Model16, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 17:
                    DrawModelEx(WDLModels.Model17, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 18:
                    DrawModelEx(WDLModels.Model18, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 19:
                    DrawModelEx(WDLModels.Model19, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 20:
                    DrawModelEx(WDLModels.Model20, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                default:
                    break;
                }
                if (CachedModels[i].Collision)
                {
                    BoundingBox ModelBox = {{(X - S), (Y - S), (Z - S)}, {(X + S), (Y + S), (Z + S)}};
                    if (CheckCollisionBoxes(OmegaPlayer.PlayerBounds, ModelBox))
                    {
                        ObjectCollision = true;
                    }
                }
            }
        }
    }
}

float GetDistance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distance = std::sqrt(dx * dx + dy * dy);
    return distance;
}

int FlipNumber(int num) {
    int i = 100;
    return i - num;
}


void WDLProcess()
{

    wstring WData = L"";
    int Size = 0;
    if (OmegaTechData.UseCachedRenderer)
    {
        WData = OtherWDLData + ExtraWDLInstructions;
        Size = GetWDLSize(OtherWDLData, ExtraWDLInstructions);
    }
    else
    {
        WData = WorldData + ExtraWDLInstructions;
        Size = GetWDLSize(WorldData, ExtraWDLInstructions);
    }

    bool Render = false;
    bool FoundPlatform = false;
    float PlatformHeight = 0.0f;

    for (int i = 0; i <= Size; i++)
    {
        wstring Instruction = WSplitValue(WData, i);

        if (Instruction == L"C")
        {
            NextCollision = true;
        }

        if (WReadValue(Instruction, 0, 4) == L"Model" || WReadValue(Instruction, 0, 1) == L"NE" || WReadValue(Instruction, 0, 6) == L"ClipBox" ||WReadValue(Instruction, 0, 5) == L"Object" || WReadValue(Instruction, 0, 5) == L"Script" || WReadValue(Instruction, 0, 8) == L"HeightMap" || WReadValue(Instruction, 0, 8) == L"Collision" || WReadValue(Instruction, 0, 11) == L"AdvCollision")
        {

            X = ToFloat(WSplitValue(WData, i + 1));
            Y = ToFloat(WSplitValue(WData, i + 2));
            Z = ToFloat(WSplitValue(WData, i + 3));
            S = ToFloat(WSplitValue(WData, i + 4));

            Rotation = ToFloat(WSplitValue(WData, i + 5));

            if (OmegaTechData.MainCamera.position.z - OmegaTechData.RenderRadius < Z && OmegaTechData.MainCamera.position.z + OmegaTechData.RenderRadius > Z)
            {
                if (OmegaTechData.MainCamera.position.x - OmegaTechData.RenderRadius < X && OmegaTechData.MainCamera.position.x + OmegaTechData.RenderRadius > X)
                {
                    Render = true;

                    if (Instruction == L"NE1"){
                        if (!IsMusicStreamPlaying(OmegaTechSoundData.NESound1))PlayMusicStream(OmegaTechSoundData.NESound1);
                    }
                    if (Instruction == L"NE2"){
                        if (!IsMusicStreamPlaying(OmegaTechSoundData.NESound2))PlayMusicStream(OmegaTechSoundData.NESound2);
                    }
                    if (Instruction == L"NE3"){
                        if (!IsMusicStreamPlaying(OmegaTechSoundData.NESound3))PlayMusicStream(OmegaTechSoundData.NESound3);
                    }

                }
            }
        }
        else {
            if (Instruction == L"NE1"){
                StopMusicStream(OmegaTechSoundData.NESound1);
            }
            if (Instruction == L"NE2"){
                StopMusicStream(OmegaTechSoundData.NESound2);
            }
            if (Instruction == L"NE3"){
                StopMusicStream(OmegaTechSoundData.NESound3);
            }

        }

        if (Render)
        {
            if (WReadValue(Instruction, 0, 4) == L"Model")
            {
                int Identifier = ToFloat(WReadValue(Instruction, 5, 6));
                switch (Identifier)
                {
                case 1:
                    DrawModelEx(WDLModels.Model1, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 2:
                    DrawModelEx(WDLModels.Model2, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 3:
                    DrawModelEx(WDLModels.Model3, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 4:
                    DrawModelEx(WDLModels.Model4, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 5:
                    DrawModelEx(WDLModels.Model5, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 6:
                    DrawModelEx(WDLModels.Model6, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 7:
                    DrawModelEx(WDLModels.Model7, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 8:
                    DrawModelEx(WDLModels.Model8, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 9:
                    DrawModelEx(WDLModels.Model9, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 10:
                    DrawModelEx(WDLModels.Model10, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 11:
                    DrawModelEx(WDLModels.Model11, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 12:
                    DrawModelEx(WDLModels.Model12, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 13:
                    DrawModelEx(WDLModels.Model13, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 14:
                    DrawModelEx(WDLModels.Model14, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 15:
                    DrawModelEx(WDLModels.Model15, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 16:
                    DrawModelEx(WDLModels.Model16, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 17:
                    DrawModelEx(WDLModels.Model17, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 18:
                    DrawModelEx(WDLModels.Model18, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 19:
                    DrawModelEx(WDLModels.Model19, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                case 20:
                    DrawModelEx(WDLModels.Model20, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
                    break;
                default:
                    break;
                }
            }

            if (NextCollision)
            {
                BoundingBox ModelBox = {{(X - S), (Y - S), (Z - S)}, {(X + S), (Y + S), (Z + S)}};
                if (CheckCollisionBoxes(OmegaPlayer.PlayerBounds, ModelBox))
                {
                    ObjectCollision = true;
                }
                NextCollision = false;
            }

            int AudioValue = 0;
                                        
            if (Instruction == L"NE1"){
                AudioValue = FlipNumber(GetDistance( X , Z, OmegaTechData.MainCamera.position.x , OmegaTechData.MainCamera.position.z));
                if (AudioValue > 0)SetMusicVolume(OmegaTechSoundData.NESound1 , float(AudioValue) / 100.0f);
            }
            if (Instruction == L"NE2"){
                AudioValue = FlipNumber(GetDistance( X , Z, OmegaTechData.MainCamera.position.x , OmegaTechData.MainCamera.position.z));
                if (AudioValue > 0)SetMusicVolume(OmegaTechSoundData.NESound2 ,  float(AudioValue) / 100.0f);
            }
            if (Instruction == L"NE3"){
                AudioValue = FlipNumber(GetDistance( X , Z, OmegaTechData.MainCamera.position.x , OmegaTechData.MainCamera.position.z));
                if (AudioValue > 0)SetMusicVolume(OmegaTechSoundData.NESound3 ,  float(AudioValue) / 100.0f);
            }

            

            if (Instruction == L"Object1")
                DrawModelEx(OmegaTechGameObjects.Object1, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            if (Instruction == L"Object2")
                DrawModelEx(OmegaTechGameObjects.Object2, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            if (Instruction == L"Object3")
                DrawModelEx(OmegaTechGameObjects.Object3, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            if (Instruction == L"Object4")
                DrawModelEx(OmegaTechGameObjects.Object4, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);
            if (Instruction == L"Object5")
                DrawModelEx(OmegaTechGameObjects.Object5, {X, Y, Z}, {0, Rotation, 0}, Rotation, {S, S, S}, FadeColor);

            if (Instruction == L"Collision")
            { // Collision
                if (CheckCollisionBoxes(OmegaPlayer.PlayerBounds, (BoundingBox){(Vector3){X, Y, Z}, (Vector3){X + S, Y + S, Z + S}}))
                {
                    ObjectCollision = true;
                }

                if (Debug)
                {
                    if (ObjectCollision)
                    {
                        DrawCubeWires({X, Y, Z}, S, S, S, GREEN);
                    }
                    else
                    {
                        DrawCubeWires({X, Y, Z}, S, S, S, RED);
                    }
                }
                if (ObjectCollision)
                {
                    if (!IsSoundPlaying(OmegaTechSoundData.CollisionSound))
                    {
                        PlaySound(OmegaTechSoundData.CollisionSound);
                    }
                }
            }

            if (WReadValue(Instruction, 0, 5) == L"Script")
            {
                if (CheckCollisionBoxes(OmegaPlayer.PlayerBounds, (BoundingBox){(Vector3){X, Y, Z}, (Vector3){X + S, Y + S, Z + S}}))
                {
                    ObjectCollision = true;
                    if (ScriptTimer == 0)
                    {
                        ParasiteScriptInit();
                        LoadScript(TextFormat("GameData/Worlds/World%i/Scripts/Script%i.ps", OmegaTechData.LevelIndex, int(ToFloat(WReadValue(Instruction, 6, Instruction.size() - 1)))));

                        for (int x = 0; x <= ParasiteScriptCoreData.ProgramSize; x++)
                        {
                            CycleInstruction();
                            ParasiteScriptCoreData.LineCounter++;
                        }

                        ScriptTimer = 180;
                    }
                }

                if (Debug)
                {
                    if (ObjectCollision)
                    {
                        DrawCubeWires({X, Y, Z}, S, S, S, GREEN);
                    }
                    else
                    {
                        DrawCubeWires({X, Y, Z}, S, S, S, YELLOW);
                    }
                }
            }
        }
        if (Instruction == L"ClipBox")
        { // Collision

                W = ToFloat(WSplitValue(WData, i + 6));
                H = ToFloat(WSplitValue(WData, i + 7));
                L = ToFloat(WSplitValue(WData, i + 8));

                if (CheckCollisionBoxSphere(
                        (BoundingBox){(Vector3){X, Y, Z}, (Vector3){W, H, L}},
                        {OmegaTechData.MainCamera.position.x + OmegaPlayer.Width / 2,
                         OmegaTechData.MainCamera.position.y - OmegaPlayer.Height / 2,
                         OmegaTechData.MainCamera.position.z - OmegaPlayer.Width / 2},
                        1.0)){
                            PlatformHeight = H;
                            FoundPlatform = true;
                        }

                if (Debug)DrawBoundingBox((BoundingBox){(Vector3){X, Y, Z}, (Vector3){W, H  - 5, L}}, PURPLE);
            

            i += 3;
        }
        if (Instruction == L"AdvCollision")
        { // Collision

            if (Render)
            {
                W = ToFloat(WSplitValue(WData, i + 6));
                H = ToFloat(WSplitValue(WData, i + 7));
                L = ToFloat(WSplitValue(WData, i + 8));

                if (CheckCollisionBoxSphere(
                        (BoundingBox){(Vector3){X, Y, Z}, (Vector3){W, H, L}},
                        {OmegaTechData.MainCamera.position.x + OmegaPlayer.Width / 2,
                         OmegaTechData.MainCamera.position.y - OmegaPlayer.Height / 2,
                         OmegaTechData.MainCamera.position.z - OmegaPlayer.Width / 2},
                        1.0))
                    ObjectCollision = true;

                if (Debug)
                {
                    if (ObjectCollision)
                    {
                        DrawBoundingBox((BoundingBox){(Vector3){X, Y, Z}, (Vector3){W, H, L}}, GREEN);
                    }
                    else
                    {
                        DrawBoundingBox((BoundingBox){(Vector3){X, Y, Z}, (Vector3){W, H, L}}, PURPLE);
                    }
                }

                if (ObjectCollision)
                {
                    if (!IsSoundPlaying(OmegaTechSoundData.CollisionSound))
                    {
                        PlaySound(OmegaTechSoundData.CollisionSound);
                    }
                }
            }

            i += 3;
        }

        if (Instruction == L"HeightMap")
        { // ?
            WDLModels.HeightMapPosition.x = X;
            WDLModels.HeightMapPosition.y = Y;
            WDLModels.HeightMapPosition.z = Z;
            DrawModel(WDLModels.HeightMap, {X, Y, Z}, S, FadeColor);
        }

        if (!NextCollision)
        {
            i += 5;
        }

        Render = false;
    }

    if (FoundPlatform ){
        OmegaTechData.MainCamera.position.y = PlatformHeight;
    }
    else {
        OmegaTechData.MainCamera.position.y = 10;
    }
}

void UpdateEntities()
{
    float PX = OmegaTechData.MainCamera.position.x;
    float PY = OmegaTechData.MainCamera.position.y;
    float PZ = OmegaTechData.MainCamera.position.z;

    float Speed = .8f;
    int AttackRadius = 100;

    for (int i = 0; i <= EntityCount - 1; i++)
    {
        if (OmegaEnemy[i].IsActive)
        {


            DrawBillboard(OmegaTechData.MainCamera, EnemyTextures.Frame1, {OmegaEnemy[i].X, OmegaEnemy[i].Y, OmegaEnemy[i].Z}, 10.0f, WHITE);
            

            if (PZ - AttackRadius < OmegaEnemy[i].Z && PZ + AttackRadius > OmegaEnemy[i].Z)
            {
                if (PX - AttackRadius < OmegaEnemy[i].X && PX + AttackRadius > OmegaEnemy[i].X)
                {
                    if (OmegaEnemy[i].X > PX)
                        OmegaEnemy[i].X -= Speed;
                    if (OmegaEnemy[i].X < PX)
                        OmegaEnemy[i].X += Speed;

                    if (OmegaEnemy[i].Z > PZ)
                        OmegaEnemy[i].Z -= Speed;
                    if (OmegaEnemy[i].Z < PZ)
                        OmegaEnemy[i].Z += Speed;

                    if (PZ - 1 < OmegaEnemy[i].Z && PZ + 1 > OmegaEnemy[i].Z)
                    {
                        if (PX - 1 < OmegaEnemy[i].X && PX + 1 > OmegaEnemy[i].X)
                        {
                            OmegaPlayer.Health = 0;
                        }
                    }

                    if (OmegaTechData.PanicCounter != 240){
                        OmegaTechData.PanicCounter += 2;
                    }

                    if (OmegaTechData.Ticker % 2 == 0)
                    {
                        if (!IsSoundPlaying(OmegaTechSoundData.ChasingSound))
                        {
                            PlaySound(OmegaTechSoundData.ChasingSound);
                        }
                    }
                }
            }
        }
    }
}

void UpdatePlayer()
{


    if (IsKeyDown(KEY_W) || GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) != 0 && !Debug)
    {
        if (HeadBob)
        {
            if (OmegaTechData.Ticker % 2 == 0)
            {
                OmegaTechData.MainCamera.target.y += OmegaPlayer.HeadBob;
                if (OmegaPlayer.HeadBobDirection == 1)
                {
                    if (OmegaPlayer.HeadBob != 1)
                    {
                        OmegaPlayer.HeadBob++;
                    }
                    else
                    {
                        OmegaPlayer.HeadBobDirection = 0;
                    }
                }
                else
                {
                    if (OmegaPlayer.HeadBob != -1)
                    {
                        OmegaPlayer.HeadBob--;
                    }
                    else
                    {
                        OmegaPlayer.HeadBobDirection = 1;
                    }
                }
            }
        }
        if (!IsSoundPlaying(OmegaTechSoundData.WalkingSound))
        {
            PlaySound(OmegaTechSoundData.WalkingSound);
        }
    }
    else
    {
        if (IsSoundPlaying(OmegaTechSoundData.WalkingSound))
        {
            StopSound(OmegaTechSoundData.WalkingSound);
        }
    }

    OmegaPlayer.PlayerBounds = (BoundingBox){(Vector3){OmegaTechData.MainCamera.position.x - OmegaPlayer.Width / 2,
                                                       OmegaTechData.MainCamera.position.y - OmegaPlayer.Height,
                                                       OmegaTechData.MainCamera.position.z - OmegaPlayer.Width / 2},
                                             (Vector3){OmegaTechData.MainCamera.position.x + OmegaPlayer.Width / 2,
                                                       OmegaTechData.MainCamera.position.y,
                                                       OmegaTechData.MainCamera.position.z + OmegaPlayer.Width / 2}};
}

void UpdateNoiseEmitters(){
    UpdateMusicStream(OmegaTechSoundData.NESound1);
    UpdateMusicStream(OmegaTechSoundData.NESound2);
    UpdateMusicStream(OmegaTechSoundData.NESound3);
}
/*
void UpdateEditor()
{
    if (IsKeyDown(KEY_LEFT_SHIFT))
    {
        OmegaTechEditor.DrawModel = true;

        if (IsKeyDown(KEY_ONE))
        {

            OmegaTechEditor.ModelID = 1;
        }
        if (IsKeyDown(KEY_TWO))
        {

            OmegaTechEditor.ModelID = 2;
        }
        if (IsKeyDown(KEY_THREE))
        {

            OmegaTechEditor.ModelID = 3;
        }
        if (IsKeyDown(KEY_FOUR))
        {

            OmegaTechEditor.ModelID = 4;
        }
        if (IsKeyDown(KEY_FIVE))
        {

            OmegaTechEditor.ModelID = 5;
        }
        if (IsKeyDown(KEY_SIX))
        {

            OmegaTechEditor.ModelID = 6;
        }
        if (IsKeyDown(KEY_SEVEN))
        {

            OmegaTechEditor.ModelID = 7;
        }
        if (IsKeyDown(KEY_EIGHT))
        {

            OmegaTechEditor.ModelID = 8;
        }
        if (IsKeyDown(KEY_NINE))
        {

            OmegaTechEditor.ModelID = 9;
        }
        if (IsKeyDown(KEY_MINUS))
        {

            OmegaTechEditor.ModelID = 10;
        }

        if (IsKeyDown(KEY_Q))
        {

            OmegaTechEditor.ModelID = 11;
        }
        if (IsKeyDown(KEY_W))
        {

            OmegaTechEditor.ModelID = 12;
        }
        if (IsKeyDown(KEY_E))
        {

            OmegaTechEditor.ModelID = 13;
        }
        if (IsKeyDown(KEY_R))
        {

            OmegaTechEditor.ModelID = 14;
        }
        if (IsKeyDown(KEY_T))
        {

            OmegaTechEditor.ModelID = 15;
        }
        if (IsKeyDown(KEY_Y))
        {

            OmegaTechEditor.ModelID = 16;
        }
        if (IsKeyDown(KEY_U))
        {

            OmegaTechEditor.ModelID = 17;
        }
        if (IsKeyDown(KEY_I))
        {

            OmegaTechEditor.ModelID = 18;
        }
        if (IsKeyDown(KEY_P))
        {

            OmegaTechEditor.ModelID = 19;
        }

        if (IsKeyDown(KEY_TAB))
        {

            OmegaTechEditor.ModelID = 100;
        }
        if (IsKeyDown(KEY_RIGHT_ALT))
        {
            OmegaTechEditor.ModelID = -3;
        }
        if (IsKeyDown(KEY_SPACE))
        {

            OmegaTechEditor.ModelID = -1;

            OmegaTechEditor.W = OmegaTechData.MainCamera.position.x + 5;
            OmegaTechEditor.H = OmegaTechData.MainCamera.position.y + 5;
            OmegaTechEditor.L = OmegaTechData.MainCamera.position.z + 5;
        }
        if (IsKeyDown(KEY_LEFT_ALT))
        {
            OmegaTechEditor.ModelID = -2;
            OmegaTechEditor.W = OmegaTechData.MainCamera.position.x + 5;
            OmegaTechEditor.H = OmegaTechData.MainCamera.position.y + 5;
            OmegaTechEditor.L = OmegaTechData.MainCamera.position.z + 5;
        }
        OmegaTechEditor.X = OmegaTechData.MainCamera.position.x;
        OmegaTechEditor.Y = OmegaTechData.MainCamera.position.y;
        OmegaTechEditor.Z = OmegaTechData.MainCamera.position.z;
        OmegaTechEditor.R = 1;
    }

    if (IsKeyDown(KEY_LEFT_CONTROL))
    {
        OmegaTechEditor.DrawModel = true;

        if (IsKeyDown(KEY_ONE))
        {

            OmegaTechEditor.ModelID = 101;
        }
        if (IsKeyDown(KEY_TWO))
        {

            OmegaTechEditor.ModelID = 102;
        }
        if (IsKeyDown(KEY_THREE))
        {

            OmegaTechEditor.ModelID = 103;
        }
        if (IsKeyDown(KEY_FOUR))
        {

            OmegaTechEditor.ModelID = 104;
        }
        if (IsKeyDown(KEY_FIVE))
        {

            OmegaTechEditor.ModelID = 105;
        }
        if (IsKeyDown(KEY_SIX))
        {

            OmegaTechEditor.ModelID = 106;
        }
        if (IsKeyDown(KEY_SEVEN))
        {

            OmegaTechEditor.ModelID = 107;
        }
        if (IsKeyDown(KEY_EIGHT))
        {

            OmegaTechEditor.ModelID = 108;
        }
        if (IsKeyDown(KEY_NINE))
        {

            OmegaTechEditor.ModelID = 109;
        }
        if (IsKeyDown(KEY_MINUS))
        {

            OmegaTechEditor.ModelID = 110;
        }
        OmegaTechEditor.X = OmegaTechData.MainCamera.position.x;
        OmegaTechEditor.Y = OmegaTechData.MainCamera.position.y;
        OmegaTechEditor.Z = OmegaTechData.MainCamera.position.z;
        OmegaTechEditor.R = 1;
    }

    if (OmegaTechEditor.DrawModel)
    {
        switch (OmegaTechEditor.ModelID)
        {
        case 1:
            DrawModelEx(WDLModels.Model1, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 2:
            DrawModelEx(WDLModels.Model2, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 3:
            DrawModelEx(WDLModels.Model3, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 4:
            DrawModelEx(WDLModels.Model4, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 5:
            DrawModelEx(WDLModels.Model5, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 6:
            DrawModelEx(WDLModels.Model6, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 7:
            DrawModelEx(WDLModels.Model7, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 8:
            DrawModelEx(WDLModels.Model8, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 9:
            DrawModelEx(WDLModels.Model9, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 10:
            DrawModelEx(WDLModels.Model10, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 11:
            DrawModelEx(WDLModels.Model11, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 12:
            DrawModelEx(WDLModels.Model12, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 13:
            DrawModelEx(WDLModels.Model13, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 14:
            DrawModelEx(WDLModels.Model14, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 15:
            DrawModelEx(WDLModels.Model15, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 16:
            DrawModelEx(WDLModels.Model16, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 17:
            DrawModelEx(WDLModels.Model17, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 18:
            DrawModelEx(WDLModels.Model18, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 19:
            DrawModelEx(WDLModels.Model19, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case 20:
            DrawModelEx(WDLModels.Model20, {OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, {0, OmegaTechEditor.R, 0}, OmegaTechEditor.R, {OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S}, FadeColor);
            break;
        case -1:
            DrawBoundingBox((BoundingBox){(Vector3){OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, (Vector3){OmegaTechEditor.W, OmegaTechEditor.H, OmegaTechEditor.L}}, ORANGE);
            DrawCubeWires({OmegaTechEditor.W, OmegaTechEditor.H, OmegaTechEditor.L}, OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S, PINK);
            break;
        case -2:
            DrawBoundingBox((BoundingBox){(Vector3){OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, (Vector3){OmegaTechEditor.W, OmegaTechEditor.H - 5, OmegaTechEditor.L}}, ORANGE);
            DrawCubeWires({OmegaTechEditor.W, OmegaTechEditor.H - 5, OmegaTechEditor.L}, OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S, PINK);
            break;
        }

        if (OmegaTechEditor.ModelID <= 100 || OmegaTechEditor.ModelID == -3)
            DrawCubeWires({OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S, PINK);
        else
        {
            DrawCubeWires({OmegaTechEditor.X, OmegaTechEditor.Y, OmegaTechEditor.Z}, OmegaTechEditor.S, OmegaTechEditor.S, OmegaTechEditor.S, ORANGE);
        }

        if (!IsMouseButtonDown(2))
        {
            if (IsKeyPressed(KEY_U))
            {
                OmegaTechEditor.X -= 5.0f;
            }
            if (IsKeyPressed(KEY_J))
            {
                OmegaTechEditor.X += 5.0f;
            }
            if (IsKeyPressed(KEY_H))
            {
                OmegaTechEditor.Z += 5.0f;
            }
            if (IsKeyPressed(KEY_K))
            {
                OmegaTechEditor.Z -= 5.0f;
            }
            if (IsKeyPressed(KEY_Y))
            {
                OmegaTechEditor.Y -= 5.0f;
            }
            if (IsKeyPressed(KEY_I))
            {
                OmegaTechEditor.Y += 5.0f;
            }
        }
        else
        {
            if (IsKeyPressed(KEY_U))
            {
                OmegaTechEditor.W -= 5.0f;
            }
            if (IsKeyPressed(KEY_J))
            {
                OmegaTechEditor.W +=  5.0f;
            }
            if (IsKeyPressed(KEY_H))
            {
                OmegaTechEditor.L +=  5.0f;
            }
            if (IsKeyPressed(KEY_K))
            {
                OmegaTechEditor.L -= 5.0f;
            }
            if (IsKeyPressed(KEY_Y))
            {
                OmegaTechEditor.H -= 5.0f;
            }
            if (IsKeyPressed(KEY_I))
            {
                OmegaTechEditor.H += 5.0f;
            }
        }

        if (IsKeyPressed(KEY_O))
        {
            OmegaTechEditor.R += 90.0f;
        }
        if (IsKeyPressed(KEY_L))
        {
            OmegaTechEditor.R -= 90.0f;
        }
        if (IsKeyDown(KEY_T))
        {
            OmegaTechEditor.S += .5f;
        }
        if (IsKeyDown(KEY_G))
        {
            OmegaTechEditor.S -= .5f;
        }

        if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(1))
        {
            wstring WDLCommand = L"";

            wstring Id = L"";

            if (OmegaTechEditor.ModelID > 0)
            {
                if (OmegaTechEditor.ModelID != 100)
                {
                    if (OmegaTechEditor.ModelID < 100)
                    {
                        Id = L"Model";
                        WDLCommand += Id + to_wstring(OmegaTechEditor.ModelID) + L":";
                    }
                    else
                    {
                        Id = L"Script";
                        WDLCommand += Id + to_wstring(OmegaTechEditor.ModelID - 100) + L":";
                    }
                }
                else
                {
                    Id = L"Collision";
                    WDLCommand += Id + L":";
                }

                WDLCommand += to_wstring(OmegaTechEditor.X) + L":" + to_wstring(OmegaTechEditor.Y) + L":" + to_wstring(OmegaTechEditor.Z) + L":" + to_wstring(OmegaTechEditor.S) + L":" + to_wstring(OmegaTechEditor.R) + L":";
            }
            else {
                if (OmegaTechEditor.ModelID == -1)
                {
                    WDLCommand += L"AdvCollision:";
                    WDLCommand += to_wstring(OmegaTechEditor.X) + L":" + to_wstring(OmegaTechEditor.Y) + L":" + to_wstring(OmegaTechEditor.Z) + L":" + to_wstring(OmegaTechEditor.S) + L":" + to_wstring(OmegaTechEditor.R) + L":" + to_wstring(OmegaTechEditor.W) + L":" + to_wstring(OmegaTechEditor.H) + L":" + to_wstring(OmegaTechEditor.L) + L":";
                }

                if (OmegaTechEditor.ModelID == -2)
                {
                    WDLCommand += L"ClipBox:";
                    WDLCommand += to_wstring(OmegaTechEditor.X) + L":" + to_wstring(OmegaTechEditor.Y) + L":" + to_wstring(OmegaTechEditor.Z) + L":" + to_wstring(OmegaTechEditor.S) + L":" + to_wstring(OmegaTechEditor.R) + L":" + to_wstring(OmegaTechEditor.W) + L":" + to_wstring(OmegaTechEditor.H) + L":" + to_wstring(OmegaTechEditor.L) + L":";
                }

                if (OmegaTechEditor.ModelID == -3){
                    WDLCommand += L"NE1:" + to_wstring(OmegaTechEditor.X) + L":" + to_wstring(OmegaTechEditor.Y) + L":" + to_wstring(OmegaTechEditor.Z) + L":" + to_wstring(OmegaTechEditor.S) + L":" + to_wstring(OmegaTechEditor.R) + L":";
                }
            }

            wcout << WDLCommand << "\n";

            WorldData += WDLCommand;

            OmegaTechEditor.DrawModel = false;
        }

        if (OmegaInputController.InteractDown)
        {
            OmegaTechEditor.X += GetMouseDelta().x / 8;
            OmegaTechEditor.Y += GetMouseDelta().y / 8;
            OmegaTechEditor.Z -= (GetMouseWheelMove() * 2);
        }
    }
    if (IsKeyPressed(KEY_RIGHT_SHIFT))
    {
        wofstream Outfile;
        Outfile.open(TextFormat("GameData/Worlds/World%i/World.wdl", OmegaTechData.LevelIndex));

        if (GameDataEncoded)
        {
            Outfile << Encode(WorldData, MainKey) << "\n";
        }
        else
        {
            Outfile << WorldData << "\n";
        }
    }
}
*/

void SaveGame()
{
    wstring TFlags = L"";

    for (int i = 0; i <= 99; i++)
    {
        if (ToggleFlags[i].Value == 1)
        {
            TFlags += L'1';
        }
        if (ToggleFlags[i].Value == 0)
        {
            TFlags += L'0';
        }
    }

    TFlags += L':';

    if (OmegaTechGameObjects.Object1Owned)TFlags += L'1';
    else {TFlags += L'0';}
    if (OmegaTechGameObjects.Object2Owned)TFlags += L'1';
    else {TFlags += L'0';}
    if (OmegaTechGameObjects.Object3Owned)TFlags += L'1';
    else {TFlags += L'0';}
    if (OmegaTechGameObjects.Object4Owned)TFlags += L'1';
    else {TFlags += L'0';}
    if (OmegaTechGameObjects.Object5Owned)TFlags += L'1';
    else {TFlags += L'0';}

    wofstream Outfile;
    Outfile.open("GameData/Saves/TF.sav");
    Outfile << TFlags;

    wstring Position = to_wstring(OmegaTechData.MainCamera.position.x) + L':' +
                       to_wstring(OmegaTechData.MainCamera.position.y) + L':' +
                       to_wstring(OmegaTechData.MainCamera.position.z) + L':' +
                       to_wstring(OmegaTechData.LevelIndex) + L':';

    wofstream Outfile1;
    Outfile1.open("GameData/Saves/POS.sav");
    Outfile1 << Position;

    wofstream Outfile2;
    Outfile2.open("GameData/Saves/Script.sav");
    Outfile2 << ExtraWDLInstructions;
}

void LoadSave()
{
    wstring TFlags = LoadFile("GameData/Saves/TF.sav");

    for (int i = 0; i <= 99; i++)
    {
        if (TFlags[i] == L'1')
        {
            ToggleFlags[i].Value = 1;
        }
        if (TFlags[i] == L'0')
        {
            ToggleFlags[i].Value = 0;
        }
    }

    if (TFlags[101] == L'1')OmegaTechGameObjects.Object1Owned = true;
    if (TFlags[102] == L'1')OmegaTechGameObjects.Object2Owned = true;
    if (TFlags[103] == L'1')OmegaTechGameObjects.Object3Owned = true;
    if (TFlags[104] == L'1')OmegaTechGameObjects.Object4Owned = true;
    if (TFlags[105] == L'1')OmegaTechGameObjects.Object5Owned = true;

    wstring Position = LoadFile("GameData/Saves/POS.sav");

    OmegaTechData.LevelIndex = int(ToFloat(WSplitValue(Position, 3)));

    SetCameraFlag = true;

    int X = ToFloat(WSplitValue(Position, 0));
    int Y = ToFloat(WSplitValue(Position, 1));
    int Z = ToFloat(WSplitValue(Position, 2));
    SetCameraPos = {float(X), float(Y), float(Z)};

    ExtraWDLInstructions = LoadFile("GameData/Saves/Script.sav");
}

void DrawWorld()
{
    BeginTextureMode(Target);
    ClearBackground(BLACK);
    if (OmegaTechData.SkyboxEnabled)
    {
        DrawTexture(WDLModels.Skybox, 0, 0, FadeColor);
    }
    BeginMode3D(OmegaTechData.MainCamera);

    BeginShaderMode(OmegaTechData.ToonShader);


    if (OmegaTechSoundData.MusicFound)
    {
        UpdateMusicStream(OmegaTechSoundData.BackgroundMusic);
    }

    UpdateNoiseEmitters();

    if (!OmegaTechData.UseCachedRenderer)
    {
        WDLProcess();
    }
    else
    {
        CWDLProcess();
        WDLProcess();
    }

    

    UpdatePlayer();

    UpdateCustom();

    EndShaderMode();

    if (Debug)
    {
        //UpdateEditor();
        DrawLights();
    }
    else
    {
        UpdateEntities();
    }
    if (ObjectCollision)
    {
        OmegaTechData.MainCamera.position.x = OmegaPlayer.OldX;
        OmegaTechData.MainCamera.position.y = OmegaPlayer.OldY;
        OmegaTechData.MainCamera.position.z = OmegaPlayer.OldZ;
        ObjectCollision = false;
    }

    EndMode3D();
    EndTextureMode();

    if (SetSceneFlag)
    {
        OmegaTechData.LevelIndex = SetSceneId;
        LoadWorld();
        SetSceneFlag = false;
    }

    if (SetCameraFlag)
    {
        OmegaTechData.MainCamera.position = SetCameraPos;
        SetCameraFlag = false;
    }

    if (ScriptTimer != 0)
    {
        ScriptTimer--;
    }

    if (OmegaTechData.Ticker != 60)
    {
        OmegaTechData.Ticker++;
    }
    else
    {
        OmegaTechData.Ticker = 0;
    }
}


void UpdateCustom()
{

}
