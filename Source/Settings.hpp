#include "raygui/raygui.h"
#include  "raygui/dark.h"
#include <cstddef>

static bool ShowSettings = false;
static bool Debug = false;
static bool HeadBob = true;
static bool PixelShader = true;
static bool ParticlesEnabled = true;
static bool FPSEnabled = false;
static float ResolutionScale = 1.0f;

static RenderTexture2D Target;


void ToggleSettings(){
    if (ShowSettings){
        ShowSettings = false;
        HideCursor(); 
        DisableCursor();  
    }
    else {
        ShowSettings = true;
        ShowCursor();
        EnableCursor();
    }
}

void UpdateSettings(){
    if (ShowSettings){
        GuiWindowBox((Rectangle){ 20, 20, 300, 400 }, "OmegaTech Developer Settings");
        if (GuiButton((Rectangle){ 40, 50, 80, 40 }, "Toggle Debug")){
            if (Debug){
                Debug = false;
            }
            else {
                Debug = true;
            }
        }
        if (GuiButton((Rectangle){ 40 + 90, 50, 80, 40 }, "Toggle H.B.")){
            if (HeadBob){
                HeadBob = false;
            }
            else {
                HeadBob = true;
            }
        }
        if (GuiButton((Rectangle){ 40 + 90 * 2, 50, 80, 40 }, "Toggle P.S.")){
            if (PixelShader){
                PixelShader = false;
            }
            else {
                PixelShader = true;
            }
        }

        if (GuiButton((Rectangle){ 40, 50 + 50, 80, 40 }, "Particles")){
            if (ParticlesEnabled){
                ParticlesEnabled = false;
            }
            else {
                ParticlesEnabled = true;
            }
        }

        if (GuiButton((Rectangle){ 40 + 90, 50 + 50, 80, 40 }, "Show FPS")){
            if (FPSEnabled){
                FPSEnabled = false;
            }
            else {
                FPSEnabled = true;
            }
        } 

        if (GuiButton((Rectangle){ 40 + 90 * 2, 50 + 50, 80, 40 }, "VSync .T")){
            if (IsWindowState(FLAG_VSYNC_HINT)){
                ClearWindowState(FLAG_VSYNC_HINT);
            }
            else {
                SetConfigFlags(FLAG_VSYNC_HINT);
            }
        } 
        GuiGroupBox((Rectangle){ 40, 150, 260, 110 }, "Resolution Scale");
        ResolutionScale = GuiSlider((Rectangle){ 90, 160, 120, 40 }, "Internal" , TextFormat("%ipx: %ipx" , int(1280 * ResolutionScale) , int(720* ResolutionScale)), ResolutionScale, 0.0f, 3.0f);
        
        if (GuiButton((Rectangle){ 90, 210, 120, 20}, "Set")){
            UnloadRenderTexture(Target);
            Target = LoadRenderTexture(int(1280 * ResolutionScale) , int(720* ResolutionScale));
        } 

        GuiGroupBox((Rectangle){ 40, 150 + 120, 265, 60 }, "Window Size");

        if (GuiButton((Rectangle){ 50, 150 + 135, 45, 30}, "480p")){
            SetWindowSize(640,480);
        }

        if (GuiButton((Rectangle){ 50 + 50, 150 + 135, 45, 30}, "720p")){
            SetWindowSize(1280,720);
        }

        if (GuiButton((Rectangle){ 50 + 50 * 2, 150 + 135, 45, 30}, "1080p")){
            SetWindowSize(1980 , 1080);
        }

        if (GuiButton((Rectangle){ 50 + 50 * 3, 150 + 135, 45, 30}, "1440p")){
            SetWindowSize(2560,1440);
        }

        if (GuiButton((Rectangle){ 50 + 50 * 4, 150 + 135, 45, 30}, "4k")){
            SetWindowSize(3840,2160);
        }
    }
}

bool MenuSettings = false;

bool Spinner003EditMode = false;
int Spinner003Value = 0;
bool Spinner004EditMode = false;
int Spinner004Value = 0;

bool VSYNCToggle = false;
bool MXAAToggle = false;

bool MuteToggle = false;
float AudioSlider = 100.0f;

void ShowMenuSetiings(){
    Rectangle LayoutRecs[15] = {
        (Rectangle){ 72, 85, 120, 24 },
        (Rectangle){ 62, 115, 391, 419 },
        (Rectangle){ 71, 145, 374, 200 },
        (Rectangle){ 94, 186, 120, 24 },
        (Rectangle){ 172, 237, 120, 24 },
        (Rectangle){ 171, 278, 120, 24 },
        (Rectangle){ 311, 258, 120, 24 },
        (Rectangle){ 325, 176, 88, 24 },
        (Rectangle){ 325, 214, 88, 24 },
        (Rectangle){ 336, 399, 88, 24 },
        (Rectangle){ 182, 407, 120, 16 },
        (Rectangle){ 72, 361, 374, 85 },
        (Rectangle){ 73, 452, 120, 24 },
        (Rectangle){ 74, 486, 146, 26 },
        (Rectangle){ 268, 475, 140, 28 },
    };
    GuiWindowBox(LayoutRecs[1], "Settings");
    GuiLabel(LayoutRecs[0], "Omega Tech Settings");
    GuiPanel(LayoutRecs[2], "Window");
    if (GuiButton(LayoutRecs[3], "Toggle Fullscreen")) ToggleFullscreen(); 
    VSYNCToggle = GuiToggle(LayoutRecs[7], "VSync", VSYNCToggle);
    MXAAToggle = GuiToggle(LayoutRecs[8], "MXAA x4", MXAAToggle);

    GuiPanel(LayoutRecs[11], "Audio");
    MuteToggle = GuiToggle(LayoutRecs[9], "Mute Audio", MuteToggle);
    AudioSlider = GuiSlider(LayoutRecs[10], "Audio Volume", NULL, AudioSlider, 0, 100);

    GuiLabel(LayoutRecs[12], "OMEGATECH ENGINE ");
    GuiLabel(LayoutRecs[13], "PixelPhobicGames 2023");
    GuiLabel(LayoutRecs[14], "@EC, JF , ZT , NC , LC");

    if (VSYNCToggle){
        SetConfigFlags(FLAG_VSYNC_HINT);
    }
    else {
        ClearWindowState(FLAG_VSYNC_HINT);
    }

    if (MXAAToggle){
        SetConfigFlags(FLAG_MSAA_4X_HINT);
    }
    else {
        ClearWindowState(FLAG_MSAA_4X_HINT);
    }
    SetMasterVolume(AudioSlider);
    if (MuteToggle){
        SetMasterVolume(0);
    }
    
}