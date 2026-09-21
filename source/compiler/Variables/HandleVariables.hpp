#include <iostream>
#if defined(_WIN32)
    #include <windows.h>
    #include <shobjidl.h> 
#endif
#include <SDL3/SDL.h>
#include <json/json.h>
#include <fstream>

extern std::vector<const char*> VarNameArray;
extern std::vector<const char*> VarDefaultArray;

void VarBuiltIn_Init();
void VarBuiltIn_Write();
