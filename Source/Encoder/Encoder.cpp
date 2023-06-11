 #include <iostream>
#include <string>



using namespace std;

// ghidefjklmnovwstuxyzabcpqr0123456789

wstring Encode(wstring Data, wstring Key){
    wstring Out = L"";
    
    for (int i=0; i <= Data.size(); i ++){
        switch (Data[i]){
            case L'A':
                Out += Key[0];
                break;
            case L'B':
                Out += Key[1];
                break;
            case L'C':
                Out += Key[2];
                break;
            case L'D':
                Out += Key[3];
                break;
            case L'E':
                Out += Key[4];
                break;
            case L'F':
                Out += Key[5];
                break;
            case L'G':
                Out += Key[6];
                break;
            case L'H':
                Out += Key[7];
                break;
            case L'I':
                Out += Key[8];
                break;
            case L'J':
                Out += Key[9];
                break;
            case L'K':
                Out += Key[10];
                break;
            case L'L':
                Out += Key[11];
                break;
            case L'M':
                Out += Key[12];
                break;
            case L'N':
                Out += Key[13];
                break;
            case L'O':
                Out += Key[14];
                break;
            case L'P':
                Out += Key[15];
                break;
            case L'Q':
                Out += Key[16];
                break;
            case L'R':
                Out += Key[17];
                break;
            case L'S':
                Out += Key[18];
                break;
            case L'T':
                Out += Key[19];
                break;
            case L'U':
                Out += Key[20];
                break;
            case L'V':
                Out += Key[21];
                break;
            case L'W':
                Out += Key[22];
                break;
            case L'X':
                Out += Key[23];
                break;
            case L'Y':
                Out += Key[24];
                break;
            case L'Z':
                Out += Key[25];
                break;
            case L'a':
                Out += Key[0 + 26];
                break;
            case L'b':
                Out += Key[1 + 26];
                break;
            case L'c':
                Out += Key[2 + 26];
                break;
            case L'd':
                Out += Key[3 + 26];
                break;
            case L'e':
                Out += Key[4 + 26];
                break;
            case L'f':
                Out += Key[5 + 26];
                break;
            case L'g':
                Out += Key[6 + 26];
                break;
            case L'h':
                Out += Key[7 + 26];
                break;
            case L'i':
                Out += Key[8 + 26];
                break;
            case L'j':
                Out += Key[9 + 26];
                break;
            case L'k':
                Out += Key[10 + 26];
                break;
            case L'l':
                Out += Key[11 + 26];
                break;
            case L'm':
                Out += Key[12 + 26];
                break;
            case L'n':
                Out += Key[13 + 26];
                break;
            case L'o':
                Out += Key[14 + 26];
                break;
            case L'p':
                Out += Key[15 + 26];
                break;
            case L'q':
                Out += Key[16 + 26];
                break;
            case L'r':
                Out += Key[17 + 26];
                break;
            case L's':
                Out += Key[18 + 26];
                break;
            case L't':
                Out += Key[19 + 26];
                break;
            case L'u':
                Out += Key[20 + 26];
                break;
            case L'v':
                Out += Key[21 + 26];
                break;
            case L'w':
                Out += Key[22 + 26];
                break;
            case L'x':
                Out += Key[23 + 26];
                break;
            case L'y':
                Out += Key[24 + 26];
                break;
            case L'z':
                Out += Key[25 + 26];
                break;

            case L'0':
                Out += Key[52];
                break;
            case L'1':
                Out += Key[53];
                break;
            case L'2':
                Out += Key[54];
                break;
            case L'3':
                Out += Key[55];
                break;
            case L'4':
                Out += Key[56];
                break;
            case L'5':
                Out += Key[57];
                break;
            case L'6':
                Out += Key[58];
                break;
            case L'7':
                Out += Key[59];
                break;
            case L'8':
                Out += Key[60];
                break;
            case L'9':
                Out += Key[61];
                break;
                
            default:
                Out += Data[i];
                break;
        }
    }
    return Out;
}
