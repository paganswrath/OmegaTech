#include "ParasiteScriptData.hpp"


#define RedText "\033[4;31m"
#define GreenText "\033[1;32m"
#define BlueText "\033[1;34m"
#define FadeColorText "\033[1;37m"


auto ParasiteScriptLoadFile(const char* Path){
    std::ifstream Input(Path);
    std::stringstream sstr;

    while(Input >> sstr.rdbuf());

    return sstr.str();
}

void ParasiteScriptTFlagWipe(){
    for (int i = 0 ; i <= MaxTFlag - 1; i++){
        ToggleFlags[i].Value = 0;
    }
}

void ParasiteScriptInit(){ 
    ParasiteScriptCoreData.LesserFlag = false;
    ParasiteScriptCoreData.GreaterFlag = false;
    ParasiteScriptCoreData.EqualFlag = false;
    ParasiteScriptCoreData.NotEqualFlag = false;

    ParasiteScriptCoreData.CompareFlag = false;
    ParasiteScriptCoreData.ContinueFlag = true;
    ParasiteScriptCoreData.ErrorFlag = false;

    ParasiteScriptCoreData.ReturnLine = 0;
    ParasiteScriptCoreData.Line[0] = '\0';

    for (int i = 0; i <= ParasiteScriptCoreData.ProgramSize; i ++){
        ParasiteScriptCoreData.Line[i] = "";
    }
    
    ParasiteScriptCoreData.ProgramSize = 0;
    ParasiteScriptCoreData.TextSize = 20;


    for (int i = 0 ; i <= MaxVaribles - 1; i++){
        VaribleMemory[i].IValue = 0;
    }
}

auto SplitValue(string Data , int Place ){ 
    string Out;
    int PlaceCounter = 0; 

    int Start = 0;
    int End = 0;
    
    if (Place == 0){
        for (int i = 0 ; i <= Data.size() ; i ++){
            if (Data[i] == ' ' || i == Data.size()){
                End = i - 1;
                break;
            }
        }
    }
    else {
        Place = Place;
        PlaceCounter = 1;
        for (int i = 0 ; i <= Data.size() ; i ++){
            if (PlaceCounter == Place){
                Start = i + 1;
                for (int x = i+1 ; x <= Data.size(); x++){
                    if (Data[x] == ' ' || x == Data.size()){
                        End = x - 1;
                        break;
                    }
                }
            }
            if (Data[i] == ' '){
                PlaceCounter ++;
            }
        }
    }

    Out = ReadValue(Data , Start , End);

    return Out;
}


auto StringToInt(string Data){
    int Out = 0;
    int Place = 1;
    for (int i = Data.size() ; i >= 0; i--){
        
        switch (Data[i]){
            case '1':
                Out += 1 * Place;
                Place = Place * 10;
                break;
            case '2':
                Out += 2 * Place;
                Place = Place * 10;
                break;
            case '3':
                Out += 3 * Place;
                Place = Place * 10;
                break;
            case '4':
                Out += 4 * Place;
                Place = Place * 10;
                break;
            case '5':
                Out += 5 * Place;
                Place = Place * 10;
                break;
            case '6':
                Out += 6 * Place;
                Place = Place * 10;
                break;
            case '7':
                Out += 7 * Place;
                Place = Place * 10;
                break;
            case '8':
                Out += 8 * Place;
                Place = Place * 10;
                break;
            case '9':
                Out += 9 * Place;
                Place = Place * 10;
                break;
            case '0':
                Place = Place * 10;
                break;
            case '-':
                Out = Out * -1;
                break;
        }
        
    }
    
    return Out;
}

auto StringToChar(string Data){
    char Out[] = ""; 
    for (int i = 0 ; i <= Data.size(); i++){
        switch (Data[i]){
            case '1':
                memcpy(Out , "1" , 1);
                break;
        }
    }
    
    return Out;
}

auto ScanVaribleMemory(string VaribleName){
    bool Found = false;
    for (int x = 0 ; x <= VaribleCounter; x++){
        if (VaribleMemory[x].Name == VaribleName){
            Found = true;
        }
    }
    return Found;
}

int PullIntFromMemory(string VaribleName){
    bool Found = false;
    for (int x = 0 ; x <= VaribleCounter; x++){
        if (VaribleMemory[x].Name == VaribleName){
            Found = true;
            return VaribleMemory[x].IValue;
        }
    }
    if (!Found){
        cout << "Error Varible Not Declared: " << VaribleName << "\n";
        ParasiteScriptCoreData.ErrorFlag = true;
    }
}

string PullStrFromMemory(string VaribleName){
    for (int x = 0 ; x <= VaribleCounter; x++){
        if (VaribleMemory[x].Name == VaribleName){
            return VaribleMemory[x].Value;
        }
    }
}

void StoreIntToMemory(string VaribleName, int Value){
    if (ScanVaribleMemory(VaribleName)){
        for (int x = 0 ; x <= VaribleCounter; x++){
            if (VaribleMemory[x].Name == VaribleName){
                VaribleMemory[x].IValue = Value;
            }
        }
    }
}

void StoreStrtoMemory(string VaribleName, string Value){
    for (int x = 0 ; x <= VaribleCounter; x++){
        if (VaribleMemory[x].Name == VaribleName){
            VaribleMemory[x].Value = Value;
        }
    }
}

string RemoveSpaces(string Data)
{
    string OutData = "";

    for (int i = 0 ; i <= Data.size() ; i ++){
        if (Data[i] != ' '){
            OutData[i] = Data[i];
        }
    }

    return OutData;
}

int ScanSpaces(string Data){
    int Spaces = 0;

    for (int i = 0 ; i <= Data.size() ; i ++){
        if (Data[i] != ' '){
            Spaces ++;
        }
    }

    return Spaces;
}

bool IsNumber(const string& s)
{
    for (char const &ch : s) {
        if (isdigit(ch) == 0) 
            return false;
    }
    return true;
 }


void LoadScript(const char *ScriptPath){ // Loads Script
    istringstream ProgramData(ParasiteScriptLoadFile(ScriptPath));
    string CurrentLine;

    while (getline(ProgramData, CurrentLine)) {

        if (ReadValue(CurrentLine, 0 , 0) == ":"){            
            JumpPoints[JumpPointCounter].Name = ReadValue(CurrentLine, 1 , CurrentLine.size() - 1);
            JumpPoints[JumpPointCounter].LineNumber = ParasiteScriptCoreData.LineCounter;
            JumpPointCounter ++;
        }
        if (ReadValue(CurrentLine, 0 , 0) == " "){
            int SpaceCounter = 0;
            for (int i = 0 ; i <= CurrentLine.size() ; i ++){
                if (ReadValue(CurrentLine, i , i) == " ")SpaceCounter ++;
                else {
                    break;
                }
            }
            string Temp = ReadValue(CurrentLine, SpaceCounter , CurrentLine.size() - 1);
            CurrentLine = Temp;
        }
        ParasiteScriptCoreData.Line[ParasiteScriptCoreData.LineCounter] = CurrentLine;
        ParasiteScriptCoreData.LineCounter ++;
        

    } 
    ParasiteScriptCoreData.ProgramSize = ParasiteScriptCoreData.LineCounter;

    ParasiteScriptCoreData.LineCounter = 0;

} 

static int TimeDelay = 0;

auto CycleInstruction(){
    if (TimeDelay == 0){
        if (ParasiteScriptCoreData.LineCounter != ParasiteScriptCoreData.ProgramSize){

            bool FoundInstruction = false;
            
            string Instruction = ParasiteScriptCoreData.Line[ParasiteScriptCoreData.LineCounter];

            if (ParasiteScriptCoreData.ErrorFlag){
                cout << "Error Command: -->" << ParasiteScriptCoreData.Line[ParasiteScriptCoreData.LineCounter - 1] << "<--\n";
                exit(0);
            }

            if (SplitValue(Instruction, 0 ) == "end"){
                FoundInstruction = true;
                ParasiteScriptCoreData.ContinueFlag = true;
            }

            if (ParasiteScriptCoreData.ContinueFlag && SplitValue(Instruction, 0) != "//"){
                for (int i = 0 ; i <= ScanSpaces(Instruction); i ++){
                    if (SplitValue(Instruction, i) == "+"){
                        string Part1 = "";
                        string Part2 = "";
                        bool Found = false;
                        int SpaceCounter = 0 ;
                        if (!IsNumber(SplitValue(Instruction, i-1))){
                            if (IsNumber(SplitValue(Instruction, i+1))){
                                int Value = StringToInt(SplitValue(Instruction, i+1));
                                int Value1 = PullIntFromMemory(SplitValue(Instruction, i-1));
                                StoreIntToMemory(SplitValue(Instruction, i-1) , Value1 + Value);
                            }
                            else{
                                int Value = PullIntFromMemory(SplitValue(Instruction, i+1));
                                StoreIntToMemory(SplitValue(Instruction, i-1) , PullIntFromMemory(SplitValue(Instruction, i-1)) + Value);
                            }

                            for (int i = 0 ; i <= Instruction.size(); i++){
                                if (Instruction[i] == '+'){
                                    Part1 = ReadValue(Instruction , 0 , i - 1);
                                    Found = true;
                                }
                                if (Found){
                                    if (Instruction[i] == ' '){
                                        SpaceCounter ++;
                                        if (SpaceCounter == 2){
                                            Part2 = ReadValue(Instruction , i + 1, Instruction.size() - 1);
                                            Part2 += " ";
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            int Value = StringToInt(SplitValue(Instruction, i-1)) + StringToInt(SplitValue(Instruction, i+1));

                            for (int i = 0 ; i <= Instruction.size(); i++){
                                if (Instruction[i] == '+'){
                                    Part1 = ReadValue(Instruction , 0 , i - 3);
                                    stringstream ss;
                                    ss << Value;
                                    Part1 += ss.str();
                                    Part1 += " ";
                                    Found = true;
                                }
                                if (Found){
                                    if (Instruction[i] == ' '){
                                        SpaceCounter ++;
                                        if (SpaceCounter == 2){
                                            Part2 = ReadValue(Instruction , i + 1, Instruction.size());
                                            Part2 += " ";
                                        }
                                    }
                                }
                            }
                        }

                    
                        Instruction = '\0';
                        Instruction = Part1 + Part2;
                        ParasiteScriptCoreData.Line[ParasiteScriptCoreData.LineCounter] = Instruction;
                         
                    }
                    if (SplitValue(Instruction, i) == "-"){
                        string Part1 = "";
                        string Part2 = "";
                        bool Found = false;
                        int SpaceCounter = 0 ;
                        if (!IsNumber(SplitValue(Instruction, i-1))){
                            if (IsNumber(SplitValue(Instruction, i+1))){
                                int Value = StringToInt(SplitValue(Instruction, i+1));
                                int Value1 = PullIntFromMemory(SplitValue(Instruction, i-1));
                                StoreIntToMemory(SplitValue(Instruction, i-1) , Value1 - Value);
                            }
                            else{
                                int Value = PullIntFromMemory(SplitValue(Instruction, i+1));
                                StoreIntToMemory(SplitValue(Instruction, i-1) , PullIntFromMemory(SplitValue(Instruction, i-1)) - Value);
                            }

                            for (int i = 0 ; i <= Instruction.size(); i++){
                                if (Instruction[i] == '-'){
                                    Part1 = ReadValue(Instruction , 0 , i - 1);
                                    Found = true;
                                }
                                if (Found){
                                    if (Instruction[i] == ' '){
                                        SpaceCounter ++;
                                        if (SpaceCounter == 2){
                                            Part2 = ReadValue(Instruction , i + 1, Instruction.size() - 1);
                                            Part2 += " ";
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            int Value = StringToInt(SplitValue(Instruction, i-1)) - StringToInt(SplitValue(Instruction, i+1));

                            for (int i = 0 ; i <= Instruction.size(); i++){
                                if (Instruction[i] == '-'){
                                    Part1 = ReadValue(Instruction , 0 , i - 3);
                                    stringstream ss;
                                    ss << Value;
                                    Part1 += ss.str();
                                    Part1 += " ";
                                    Found = true;
                                }
                                if (Found){
                                    if (Instruction[i] == ' '){
                                        SpaceCounter ++;
                                        if (SpaceCounter == 2){
                                            Part2 = ReadValue(Instruction , i + 1, Instruction.size());
                                            Part2 += " ";
                                        }
                                    }
                                }
                            }
                        }

                    
                        Instruction = '\0';
                        Instruction = Part1 + Part2;
                        ParasiteScriptCoreData.Line[ParasiteScriptCoreData.LineCounter] = Instruction;
                    }

                    if (SplitValue(Instruction, i) == "*"){
                        string Part1 = "";
                        string Part2 = "";
                        bool Found = false;
                        int SpaceCounter = 0 ;
                        if (!IsNumber(SplitValue(Instruction, i-1))){
                            if (IsNumber(SplitValue(Instruction, i+1))){
                                int Value = StringToInt(SplitValue(Instruction, i+1));
                                int Value1 = PullIntFromMemory(SplitValue(Instruction, i-1));
                                StoreIntToMemory(SplitValue(Instruction, i-1) , Value1 * Value);
                            }
                            else{
                                int Value = PullIntFromMemory(SplitValue(Instruction, i+1));
                                StoreIntToMemory(SplitValue(Instruction, i-1) , PullIntFromMemory(SplitValue(Instruction, i-1)) * Value);
                            }

                            for (int i = 0 ; i <= Instruction.size(); i++){
                                if (Instruction[i] == '*'){
                                    Part1 = ReadValue(Instruction , 0 , i - 1);
                                    Found = true;
                                }
                                if (Found){
                                    if (Instruction[i] == ' '){
                                        SpaceCounter ++;
                                        if (SpaceCounter == 2){
                                            Part2 = ReadValue(Instruction , i + 1, Instruction.size() - 1);
                                            Part2 += " ";
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            int Value = StringToInt(SplitValue(Instruction, i-1)) * StringToInt(SplitValue(Instruction, i+1));

                            for (int i = 0 ; i <= Instruction.size(); i++){
                                if (Instruction[i] == '*'){
                                    Part1 = ReadValue(Instruction , 0 , i - 3);
                                    stringstream ss;
                                    ss << Value;
                                    Part1 += ss.str();
                                    Part1 += " ";
                                    cout << Part1 << "\n";
                                    Found = true;
                                }
                                if (Found){
                                    if (Instruction[i] == ' '){
                                        SpaceCounter ++;
                                        if (SpaceCounter == 2){
                                            Part2 = ReadValue(Instruction , i + 1, Instruction.size());
                                            Part2 += " ";
                                        }
                                    }
                                }
                            }
                        }

                    
                        Instruction = '\0';
                        Instruction = Part1 + Part2;
                        ParasiteScriptCoreData.Line[ParasiteScriptCoreData.LineCounter] = Instruction;
                         
                    }

                    if (SplitValue(Instruction, i) == "/"){
                        string Part1 = "";
                        string Part2 = "";
                        bool Found = false;
                        int SpaceCounter = 0 ;
                        if (!IsNumber(SplitValue(Instruction, i-1))){
                            if (IsNumber(SplitValue(Instruction, i+1))){
                                int Value = StringToInt(SplitValue(Instruction, i+1));
                                int Value1 = PullIntFromMemory(SplitValue(Instruction, i-1));
                                StoreIntToMemory(SplitValue(Instruction, i-1) , Value1 / Value);
                            }
                            else{
                                int Value = PullIntFromMemory(SplitValue(Instruction, i+1));
                                StoreIntToMemory(SplitValue(Instruction, i-1) , PullIntFromMemory(SplitValue(Instruction, i-1)) / Value);
                            }

                            for (int i = 0 ; i <= Instruction.size(); i++){
                                if (Instruction[i] == '/'){
                                    Part1 = ReadValue(Instruction , 0 , i - 1);
                                    Found = true;
                                }
                                if (Found){
                                    if (Instruction[i] == ' '){
                                        SpaceCounter ++;
                                        if (SpaceCounter == 2){
                                            Part2 = ReadValue(Instruction , i + 1, Instruction.size() - 1);
                                            Part2 += " ";
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            int Value = StringToInt(SplitValue(Instruction, i-1)) / StringToInt(SplitValue(Instruction, i+1));

                            for (int i = 0 ; i <= Instruction.size(); i++){
                                if (Instruction[i] == '/'){
                                    Part1 = ReadValue(Instruction , 0 , i - 3);
                                    stringstream ss;
                                    ss << Value;
                                    Part1 += ss.str();
                                    Part1 += " ";
                                    cout << Part1 << "\n";
                                    Found = true;
                                }
                                if (Found){
                                    if (Instruction[i] == ' '){
                                        SpaceCounter ++;
                                        if (SpaceCounter == 2){
                                            Part2 = ReadValue(Instruction , i + 1, Instruction.size());
                                            Part2 += " ";
                                        }
                                    }
                                }
                            }
                        }

                    
                        Instruction = '\0';
                        Instruction = Part1 + Part2;
                        ParasiteScriptCoreData.Line[ParasiteScriptCoreData.LineCounter] = Instruction;
                         
                    }
                }

                if (SplitValue(Instruction, 1) == "="){
                    FoundInstruction = true;
                    if (IsNumber(SplitValue(Instruction, 2))){
                        StoreIntToMemory(SplitValue(Instruction, 0) , StringToInt(SplitValue(Instruction, 2)));
                        if (Debug)cout << "set: " << StringToInt(SplitValue(Instruction, 2)) << "\n";
                    }
                    else {
                        int Value = PullIntFromMemory(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , Value);
                        if (Debug)cout << "set: " << Value << "\n";
                    }
                }

                if (SplitValue(Instruction, 1) == "+="){
                    FoundInstruction = true;
                    if (IsNumber(SplitValue(Instruction, 2))){
                        int Value = StringToInt(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , PullIntFromMemory(SplitValue(Instruction, 0)) + Value);

                        if (Debug)cout << "add: " << PullIntFromMemory(SplitValue(Instruction, 0))+ Value << "\n";
                    }
                    else{
                        int Value = PullIntFromMemory(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , PullIntFromMemory(SplitValue(Instruction, 0)) + Value);
                        if (Debug)cout << "add: " << PullIntFromMemory(SplitValue(Instruction, 0))+ Value << "\n";
                    }
                }

                if (SplitValue(Instruction, 1) == "-="){
                    FoundInstruction = true;
                    if (IsNumber(SplitValue(Instruction, 2))){
                        int Value = StringToInt(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , PullIntFromMemory(SplitValue(Instruction, 0)) - Value);
                        if (Debug)cout << "sub: " << PullIntFromMemory(SplitValue(Instruction, 0)) - Value << "\n";
                    }
                    else{
                        int Value = PullIntFromMemory(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , PullIntFromMemory(SplitValue(Instruction, 0)) - Value);
                        if (Debug)cout << "sub: " << PullIntFromMemory(SplitValue(Instruction, 0)) - Value << "\n";
                    }
                }

                if (SplitValue(Instruction, 1) == "*="){
                    FoundInstruction = true;
                    if (IsNumber(SplitValue(Instruction, 2))){
                        int Value = StringToInt(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , PullIntFromMemory(SplitValue(Instruction, 0)) * Value);
                        if (Debug)cout << "mul: " << PullIntFromMemory(SplitValue(Instruction, 0)) * Value << "\n";
                    }
                    else{
                        int Value = PullIntFromMemory(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , PullIntFromMemory(SplitValue(Instruction, 0)) * Value);
                        if (Debug)cout << "mul: " << PullIntFromMemory(SplitValue(Instruction, 0)) * Value << "\n";
                    }
                }

                if (SplitValue(Instruction, 1) == "/="){
                    FoundInstruction = true;
                    if (IsNumber(SplitValue(Instruction, 2))){
                        int Value = StringToInt(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , PullIntFromMemory(SplitValue(Instruction, 0)) / Value);
                        if (Debug)cout << "div: " << PullIntFromMemory(SplitValue(Instruction, 0)) / Value << "\n";
                    }
                    else{
                        int Value = PullIntFromMemory(SplitValue(Instruction, 2));
                        StoreIntToMemory(SplitValue(Instruction, 0) , PullIntFromMemory(SplitValue(Instruction, 0)) / Value);
                        if (Debug)cout << "div: " << PullIntFromMemory(SplitValue(Instruction, 0)) / Value << "\n";
                    }
                }

                if (SplitValue(Instruction, 0 ) == "if"){
                    FoundInstruction = true;

                    ParasiteScriptCoreData.ContinueFlag = true;
                    ParasiteScriptCoreData.ElseFlag = false;

                    string Operator = SplitValue(Instruction, 2 );
                    int Value1 = 0;
                    int Value2 = 0;

                    if (IsNumber(SplitValue(Instruction, 1))){
                        Value1 = StringToInt(SplitValue(Instruction, 1 ));
                    }
                    else {
                        Value1 = PullIntFromMemory(SplitValue(Instruction, 1 ));
                    }

                    if (IsNumber(SplitValue(Instruction, 3))){
                        Value2 = StringToInt(SplitValue(Instruction, 3 ));
                    }
                    else {
                        Value2 = PullIntFromMemory(SplitValue(Instruction, 3 ));
                    }

                    if (Operator == "=="){
                        if (Value1 == Value2){
                            ParasiteScriptCoreData.ContinueFlag = true;
                        }
                        else {
                            ParasiteScriptCoreData.ContinueFlag = false;
                            ParasiteScriptCoreData.ElseFlag = true;
                        }
                    }

                    if (Operator == ">"){
                        if (Value1 > Value2){
                            ParasiteScriptCoreData.ContinueFlag = true;
                        }
                        else {
                            ParasiteScriptCoreData.ContinueFlag = false;
                            ParasiteScriptCoreData.ElseFlag = true;
                        }
                    }

                    if (Operator == "<"){
                        if (Value1 < Value2){
                            ParasiteScriptCoreData.ContinueFlag = true;
                        }
                        else {
                            ParasiteScriptCoreData.ContinueFlag = false;
                            ParasiteScriptCoreData.ElseFlag = true;
                        }
                    }

                    if (Operator == ">="){
                        if (Value1 >= Value2){
                            ParasiteScriptCoreData.ContinueFlag = true;
                        }
                        else {
                            ParasiteScriptCoreData.ContinueFlag = false;
                            ParasiteScriptCoreData.ElseFlag = true;
                        }
                    }

                    if (Operator == "<="){
                        if (Value1 >= Value2){
                            ParasiteScriptCoreData.ContinueFlag = true;
                        }
                        else {
                            ParasiteScriptCoreData.ContinueFlag = false;
                            ParasiteScriptCoreData.ElseFlag = true;
                        }
                    }

                    if (Operator == "!="){
                        if (Value1 != Value2){
                            ParasiteScriptCoreData.ContinueFlag = true;
                        }
                        else {
                            ParasiteScriptCoreData.ContinueFlag = false;
                            ParasiteScriptCoreData.ElseFlag = true;
                        }
                    }
                }

                if (SplitValue(Instruction, 0 ) == "else"){
                    FoundInstruction = true;
                    if (!ParasiteScriptCoreData.ElseFlag){
                        ParasiteScriptCoreData.ContinueFlag = false;
                    }
                }

                if (SplitValue(Instruction, 0 ) == "stop"){
                    FoundInstruction = true;
                    exit(0);
                }

                if (SplitValue(Instruction, 0 ) == "var" ){
                    FoundInstruction = true;
                    VaribleMemory[VaribleCounter].Name = SplitValue(Instruction, 1 );

                    string VaribleData = SplitValue(Instruction, 2 );
                    int VaribleType = 0;

                    if (Debug)cout << RedText << "VD: " << VaribleData << " ";

                    if (ReadValue(VaribleData , 0 , 0) == "'"){
                        if (Debug)cout << "String Type\n";
                        VaribleType = 1;
                    }
                    else {
                        if (Debug)cout << "Int Type\n";
                        VaribleType = 0;

                    }
                    
                    if (VaribleType == 1){
                        string StoredData = "";
                        for (int i = 0 ; i <= VaribleData.size() ; i ++){
                            if (ReadValue(VaribleData , i , i) != "'"){
                                StoredData += ReadValue(VaribleData , i , i);
                            }
                        }
                        if (Debug)cout << "Parsed String: " << StoredData << "\n"; 
                        VaribleMemory[VaribleCounter].Value = StoredData;
                        VaribleMemory[VaribleCounter].IValue = 0;
                    }
                    else {
                        VaribleMemory[VaribleCounter].IValue = StringToInt(VaribleData);
                        VaribleMemory[VaribleCounter].Value = " ";
                    }

                    VaribleCounter++;   

                }

                if (SplitValue(Instruction, 0 ) == "wtflag"){
                    ToggleFlags[StringToInt(SplitValue(Instruction, 1 ))].Value = StringToInt(SplitValue(Instruction, 2));
                    FoundInstruction = true;
                }

                if (SplitValue(Instruction, 0 ) == "rtflag"){
                    int Value = ToggleFlags[StringToInt(SplitValue(Instruction, 1 ))].Value;
                    StoreIntToMemory(SplitValue(Instruction, 2), Value);
                    FoundInstruction = true;
                }

                if (SplitValue(Instruction, 0 ) == "say"){
                    OmegaTechTextSystem.Write(ReadValue(Instruction , 4 , Instruction.size() - 1));
                    FoundInstruction = true;
                }
                if (SplitValue(Instruction, 0 ) == "addwdli"){
                    string WDLString = SplitValue(Instruction, 1 );
                    wstring ws(WDLString.begin(), WDLString.end());
                    ExtraWDLInstructions += ws;
                    FoundInstruction = true;
                }
                if (SplitValue(Instruction, 0 ) == "clrwdli"){
                    ExtraWDLInstructions = L"";
                    FoundInstruction = true;
                }

                if (SplitValue(Instruction, 0 ) == "ownobj"){
                    int ObjectId = StringToInt(SplitValue(Instruction, 1));

                    switch (ObjectId){
                        case 1:
                            OmegaTechGameObjects.Object1Owned = true;
                            break;
                        case 2:
                            OmegaTechGameObjects.Object2Owned = true;
                            break;
                        case 3:
                            OmegaTechGameObjects.Object3Owned = true;
                            break;
                        case 4:
                            OmegaTechGameObjects.Object3Owned = true;
                            break;
                        case 5:
                            OmegaTechGameObjects.Object3Owned = true;
                            break;
                    }

                    FoundInstruction = true;
                }
                

                if (SplitValue(Instruction, 0 ) == "setscene"){
                    SetSceneFlag = true;
                    SetSceneId = StringToInt(SplitValue(Instruction, 1 ));
                    FoundInstruction = true;
                }

                if (SplitValue(Instruction, 0 ) == "kill"){
                    ParasiteScriptCoreData.LineCounter = ParasiteScriptCoreData.ProgramSize;
                    FoundInstruction = true;
                }
                if (SplitValue(Instruction, 0 ) == "setcampos"){
                    SetCameraFlag = true;
                    int X = StringToInt(SplitValue(Instruction, 1 ));
                    int Y = StringToInt(SplitValue(Instruction, 2 ));
                    int Z = StringToInt(SplitValue(Instruction, 3 ));
                    SetCameraPos = {float(X),float(Y),float(Z)};

                    FoundInstruction = true;
                }
            }
        }
    }
}
