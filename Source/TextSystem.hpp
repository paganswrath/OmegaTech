#include "Objects.hpp"

using namespace std;

auto ReadValue(string Data , int Start , int End){ // Reads Value from (Start to End) 
    string Out;
    for (int i = Start ; i <= End ; i ++){
        Out += Data[i]; 
    }
    return Out;
}


class TextSystem{
    public:
        bool Trigger = false;
        bool ReadLine = true;
        int LinePosition = 0;
        int FrameCounter = 0;

        string Lines = ""; 
        string CurrentLine = "";
        string SecondLine = "";
        bool SEnable = false;
        Texture2D Bar;
        Font BarFont;
        Sound TextNoise;

        int ScaleValue = 0;
        Color TextColor = WHITE;

        int Red = 0;
        int Green = 0;
        int Blue = 0;

        Color TextBarColor = (Color){Red, Green, Blue, 255};

        void Update(){
            ScaleValue = int(GetScreenWidth() / 1280);
            TextBarColor = {Red, Green, Blue, Red};

            if (Red != 0){
                DrawTextureEx(Bar , { 0, 720 / 2 - 100} , 0 , 2 * ScaleValue, TextBarColor);
            }

            if (Trigger){
                if (Red != 255){
                    Red ++;
                    Green ++;
                    Blue ++;
                }
                if (ReadLine){
                    for (int i = LinePosition ; i <= Lines.size(); i ++){
                        if (Lines[i] != '|'){
                            if (CurrentLine.size() != 51){
                                CurrentLine += Lines[i];
                            }
                            else {
                                SEnable = true;
                            }

                            if (SEnable){
                                SecondLine += Lines[i];
                            }

                            LinePosition ++;
                        }
                        else {
                            ReadLine = false;
                            break;
                        }
                        if (Lines[i] == '*'){
                            exit(0);
                        }
                        if (Lines[i] == ' '){
                            StopSound(TextNoise);
                        }
                        if (Lines[i] == '/'){
                            CurrentLine = "";
                            LinePosition = 0;
                            ReadLine = false;
                            Trigger = false;
                            break;
                        }
                    }
                }
                else {
                    DrawTextEx(BarFont, TextSubtext(TextFormat("%s" , CurrentLine.c_str() ), 0 , FrameCounter / 3 ) , { 190* ScaleValue, 720 / 2 - 100 + 80  } , 30 , 1, TextColor);

                    if (SEnable){
                        DrawTextEx(BarFont, TextSubtext(TextFormat("%s" , SecondLine.c_str() ), 0 , FrameCounter / 3) , { 190 * ScaleValue, 720 / 2 - 100 + 80  + 35 } , 30  , 1, TextColor);
                    }
                    if (FrameCounter != (CurrentLine.size() * 3) +  SecondLine.size() * 3){
                        FrameCounter ++;
                    }

                    if (FrameCounter % 14 == 1){
                        StopSound(TextNoise);
                        PlaySound(TextNoise);
                    }

                    if (OmegaInputController.TextButton){
                        Red = 255;
                        Green = 255;
                        Blue = 255;
                        ReadLine = true;
                        LinePosition+=1;
                        CurrentLine = "";
                        SEnable = false;
                        SecondLine = "";
                        FrameCounter = 0;
                    }
                }
            }
            else {
                if (Red != 0){
                    Red -=5;
                    Green -=5;
                    Blue -=5;
                }
            }
        }

        auto Write(string SayLines){
            Red = 0;
            Blue = 0;
            Green = 0;

            Lines = SayLines;
            CurrentLine = "";
            SecondLine = "";
            LinePosition = 0;
            ReadLine = true;
            Trigger = true;
            SEnable = false;
        }
};

static TextSystem OmegaTechTextSystem;