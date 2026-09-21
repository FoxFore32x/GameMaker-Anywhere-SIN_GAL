#include <iostream>
#if defined(_WIN32)
    #include <windows.h>
    #include <shobjidl.h> 
#endif
#include <json/json.h>
#include <SDL3/SDL.h>
#include <string>
#include <cstdlib>
#include <fstream>
#include <filesystem>

#define ARRAYSIZE(arg) SDL_arraysize(arg)

extern const char* initDir;

void ShowError(const char* Message);
const char* GetFileUI(/*COMDLG_FILTERSPEC rgSpec[], UINT filterCount*/);
Json::Value ParseJSON(const char* path);
const char* File_GetLocation(const char* path);
void File_MakeNew(const char* FilePath, ...);
void File_RemoveAll(const char* FilePath);
void File_CopyAll(const char* FilePath, const char* destination);
void File_CreateDir(const char* FilePath);
void File_WriteEnd(const char* FilePath, const char* Message);
void File_WriteLine(const char* FilePath, int Line, const char* Message);
void File_WriteFirst(const char* FilePath, const char* Message);
void File_ReplaceLine(const char* FilePath, const char* Find, const char* Message);
void File_AddToEndReplace(const char* FilePath, const char* Find, const char* Message);
char* File_ToChar(const char* path);
const char* File_GetDir(const char* filepath);