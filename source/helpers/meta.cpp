#include <iostream>
#if defined(_WIN32)
    #include <windows.h>
    #include <shobjidl.h> 
#endif
#include <SDL3/SDL.h>
#include "renderer.hpp"
#include <json/json.h>
#include <fstream>
#include "meta.hpp"
using namespace std;

//Parse a json file
Json::Value ParseJSON(const char* path){
    Json::Value result;

    FILE* fp = fopen(path, "rb");
    if (!fp) {
        perror("fopen failed");
        return result;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char* data = (char*)malloc(size + 1);
    fread(data, 1, size, fp);
    data[size] = 0;
    fclose(fp);

    Json::CharReaderBuilder builder;
    builder["allowTrailingCommas"] = true;
    std::string errs;
    std::istringstream stream(data);

    if (!Json::parseFromStream(builder, stream, &result, &errs)) {
        printf("JSON parse error: %s\n", errs.c_str());
    }

    free(data);
    return result;
}

const SDL_DialogFileFilter filters[] = {
    { "GameMaker Project",  "yyp" }
};

static std::string SelectedFile;

static void SDLCALL callback(void* userdata, const char* const* filelist, int filter)
{
    if (!filelist) {
        SDL_Log("An error occured: %s", SDL_GetError());
        SelectedFile.clear();
        return;
    } else if (!*filelist) {
        SDL_Log("The user did not select any file.");
        SDL_Log("Most likely, the dialog was canceled.");
        SelectedFile.clear();
        return;
    }

    if (filter < 0) {
        SDL_Log("The current platform does not support fetching "
                "the selected filter, or the user did not select"
                " any filter.");
        return;
    } else if (filter < SDL_arraysize(filters)) {
        SDL_Log("The filter selected by the user is '%s' (%s).",
                filters[filter].pattern, filters[filter].name);
        return;
    }

    SelectedFile = *filelist;
    printf("Selected file: %s\n", SelectedFile.c_str());
}

//Brings up the file picker (to-do, clean up and shrink i just stole this from a microsoft example lol)
const char* GetFileUI(/*COMDLG_FILTERSPEC rgSpec[], UINT filterCount*/){
    SelectedFile.clear();
    SDL_ShowOpenFileDialog(callback, NULL, window, filters, SDL_arraysize(filters), NULL, false);
    return SelectedFile.c_str();
}

//Show a error message
void ShowError(const char* Message){
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", Message, window);
}

void File_WriteEnd(const char* FilePath, const char* Message){
    std::ofstream out;
    out.open(FilePath, std::ios::app);
    std::string str = Message;
    out << str;
    out.close();
}

void File_WriteLine(const char* FilePath, int Line, const char* Message){
    std::ifstream in(FilePath);
    std::vector<std::string> lines;
    std::string str;

    //Push back until at the line
    while (std::getline(in, str))
        lines.push_back(str);

    in.close();

    std::ofstream out(FilePath);

    for (int i = 0; i < lines.size(); i++){
        if (i == Line)
            out << Message << "\n";

        out << lines[i] << "\n";
    }

    out.close();
}

const char* File_GetLocation(const char* path){
    static std::string location;
    
    location = (std::filesystem::path(initDir) / path).string();
    return location.c_str();
}

void File_MakeNew(const char* FilePath, ...){

    char txt[256];
    va_list args;
    va_start(args, FilePath);
    vsnprintf(txt, sizeof(txt), FilePath, args);
    va_end(args);

    std::filesystem::path path = std::filesystem::path(initDir) / txt;

    std::filesystem::create_directories(path);
    std::ofstream file(path);
}

void File_CreateDir(const char* FilePath){
    std::filesystem::path path = std::filesystem::path(initDir) / FilePath;

    std::filesystem::create_directories(path);
}

void File_RemoveAll(const char* FilePath){
    std::filesystem::path path = std::filesystem::path(initDir) / FilePath;

    std::filesystem::remove_all(path);
}

void File_CopyAll(const char* FilePath, const char* destination){
    std::filesystem::path path = std::filesystem::path(initDir) / destination;

    std::filesystem::create_directories(path);

    std::filesystem::copy(FilePath, destination, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
}

void File_WriteFirst(const char* FilePath, const char* Message){
    File_WriteLine(FilePath, 0, Message);
}

void File_AddToEndReplace(const char* FilePath, const char* Find, const char* Message){
    std::ifstream in(FilePath);
    std::vector<std::string> lines;
    std::string str;

    while (std::getline(in, str))
        lines.push_back(str);

    in.close();

    for (size_t i = 0; i < lines.size(); i++){
        if (lines[i].find(Find) != std::string::npos){
            lines[i] += Message;
            break;
        }
    }

    std::ofstream out(FilePath);
    for (auto& l : lines)
        out << l << "\n";
    out.close();
}

void File_ReplaceLine(const char* FilePath, const char* Find, const char* Message){
    std::ifstream in(FilePath);
    std::vector<std::string> lines;
    std::string line;

    while (std::getline(in, line)){
        if (line == Find){
            line = Message;
        }

        lines.push_back(line);
    }

    in.close();

    std::ofstream out(FilePath);

    for (const std::string& line : lines){
        out << line << "\n";
    }

    out.close();
}

char* File_ToChar(const char* path){
    FILE* file = fopen(path, "rb");

    if (!file)
        return "";

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* code = (char*)malloc(size + 1);

    if (!code){
        fclose(file);
        return "";
    }

    fread(code, 1, size, file);
    code[size] = '\0';

    fclose(file);

    return code;
}

const char* File_GetDir(const char* filepath){
    static char dir[1024];

    const char* slash = strrchr(filepath, '/');
    const char* backslash = strrchr(filepath, '\\');

    if (backslash > slash)
        slash = backslash;

    if (slash == nullptr)
        return "";

    int length = slash - filepath;

    snprintf(dir, sizeof(dir), "%.*s", length, filepath);

    return dir;
}