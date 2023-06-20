#include <vector>
#include <string>
#include <bits/stdc++.h> 

#include "raylib.h"

using namespace std;



static int ExplosionEffect = 1;

static int TrailEffect = 2;

static int RainEffect = 3;


#define MaxParticles 50



class ParticleSystem{
    public:
        Vector2 ParticleArray[MaxParticles];
        Vector2 ParticleVelocity[MaxParticles];
        int ParticleSize[MaxParticles];
        int Colour[MaxParticles];

        int ParticleCounter = 0;

        void DeleteEffect(){
            for (int i = 0 ; i <= MaxParticles - 1; i ++){
                ParticleArray[i].x = 0;
                ParticleArray[i].y = 0;
                ParticleVelocity[i].x = 0; 
                ParticleVelocity[i].y = 0;
                ParticleSize[i] = 0;
                ParticleCounter = 0;
            }
        }

        void TriggerEffect(Vector2 Start , int EffectType){
            if (EffectType == ExplosionEffect){
                for (int i = 0 ; i <= MaxParticles - 1; i ++){
                    ParticleArray[i].x = Start.x + GetRandomValue(-20, 20);
                    ParticleArray[i].y = Start.y + GetRandomValue(-20, 20);
                    ParticleVelocity[i].x = GetRandomValue(-20, 20);
                    ParticleVelocity[i].y = GetRandomValue(-20, 20);

                    ParticleSize[i] = GetRandomValue(0, 10);
                    Colour[i] = GetRandomValue(0, 4);

                }
            }
            if (EffectType == TrailEffect){

                ParticleArray[ParticleCounter].x = Start.x + GetRandomValue(1, 1);
                ParticleArray[ParticleCounter].y = Start.y + GetRandomValue(-20, 1);
                ParticleVelocity[ParticleCounter].x = GetRandomValue(-10, -15);
                ParticleVelocity[ParticleCounter].y = GetRandomValue(-4, 1);

                ParticleSize[ParticleCounter] = GetRandomValue(0, 10);
                Colour[ParticleCounter] = GetRandomValue(0, 4);

                ParticleCounter ++;
                

                if (ParticleCounter == MaxParticles){
                    DeleteEffect();
                }
            }
            if (EffectType == RainEffect){

                ParticleArray[ParticleCounter].x = Start.x + GetRandomValue(1, GetScreenWidth());
                ParticleArray[ParticleCounter].y = Start.y;
                ParticleVelocity[ParticleCounter].x = 0;
                ParticleVelocity[ParticleCounter].y = GetRandomValue(1, 20);

                ParticleSize[ParticleCounter] = GetRandomValue(0, 10);
                Colour[ParticleCounter] = 2;

                ParticleCounter ++;
                

                if (ParticleCounter == MaxParticles){
                    DeleteEffect();
                }
            }
        }

        void Update(int XOff , int YOff){
            for (int i = 0 ; i <= MaxParticles - 1; i ++){


                switch (Colour[i])
                {
                    case 0:
                        //DrawCircleLines(ParticleArray[i].x - XOff , ParticleArray[i].y - YOff , ParticleSize[i] , RED); 
                        DrawPixel(ParticleArray[i].x - XOff , ParticleArray[i].y - YOff , RED);
                        break;
                    case 1:
                        //DrawCircleLines(ParticleArray[i].x - XOff , ParticleArray[i].y - YOff , ParticleSize[i] , ORANGE); 
                        DrawPixel(ParticleArray[i].x - XOff , ParticleArray[i].y - YOff , ORANGE);
                        break;
                    case 2:
                        DrawPixel(ParticleArray[i].x - XOff , ParticleArray[i].y - YOff , BLUE);
                    
                        break;
                    case 3:
                        //DrawCircleLines(ParticleArray[i].x - XOff , ParticleArray[i].y - YOff , ParticleSize[i] , YELLOW); 
                        DrawPixel(ParticleArray[i].x - XOff , ParticleArray[i].y - YOff , YELLOW);
                        break;
                    case 5:
                        //DrawCircleLines(ParticleArray[i].x - XOff , ParticleArray[i].y - YOff , ParticleSize[i] , YELLOW); 
                        DrawPixel(ParticleArray[i].x - XOff , ParticleArray[i].y - YOff , BLUE);
                        break;

                    default:
                        //DrawCircleLines(ParticleArray[i].x - XOff , ParticleArray[i].y - YOff , ParticleSize[i] , WHITE); 
                        DrawPixel(ParticleArray[i].x - XOff , ParticleArray[i].y - YOff , RED);
                        break;
                }


                //if (ParticleArray[i].x <= 0)DeleteEffect();
                

                ParticleArray[i].x += ParticleVelocity[i].x;
                ParticleArray[i].y += ParticleVelocity[i].y;

                ParticleArray[i].x += GetMouseDelta().x;
                ParticleArray[i].y += GetMouseDelta().y;

            }
        }
};
