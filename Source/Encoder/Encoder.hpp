#include <iostream>
#include <string>

using namespace std;

static bool GameDataEncoded = false;

wstring MainKey = L"9876543210zyxwvutsrqponmlkjihgfedcbaZYXWVUTSRQPONMLKJIHGFEDCBA";

wstring Encode(wstring Data, wstring Key);
