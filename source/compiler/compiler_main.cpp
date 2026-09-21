#include <iostream>
#include <iostream>
#if defined(_WIN32)
    #include <windows.h>
    #include <shobjidl.h> 
#endif
#include <sys/stat.h>
#include "../helpers/renderer.hpp"
#include "../helpers/meta.hpp"
#include "compiler_main.hpp"
#include "Variables/HandleVariables.hpp"
#include <json/json.h>

//Assets
#include "Asset/RoomCompiler.hpp"
#include "Asset/SpriteCompiler.hpp"
#include "Asset/ObjectCompiler.hpp"

using namespace std;
const char* ProjectYYP = "";
char RuntimePath[512];
const char* ExportMode = "3DSX";
int currentsprite_count = 0;
int currentobject_count = 0;

//Start the project compilation
void RunCompiler(){
    if (InitCompiler() == false)
        ShowError("FAILED TO INIT COMPILER!\nCHECK LOG FOR MORE INFO!");
        
	//Create the t3s texture list
	if (ExportMode == "3DSX" || ExportMode == "CIA"){
        /*std::string command = "powershell -Command \"New-Item -Path '" + std::string(initDir) + "GamemakerAnywhere/Runtime/gfx/sprites.t3s' -Force\"";

        std::filesystem::path path = std::filesystem::path(initDir) / "GamemakerAnywhere/Runtime/gfx/sprites.t3s";

        std::filesystem::create_directories(path.parent_path());
        std::ofstream file(path);

        system(command.c_str());*/

        File_MakeNew("GamemakerAnywhere/Runtime/gfx/sprites.t3s");
        std::string t3sFile = std::string(initDir) + "GamemakerAnywhere/Runtime/gfx/sprites.t3s";
        FILE* T3S = fopen(t3sFile.c_str(), "w");
		fprintf(T3S, "--atlas\n");
        fclose(T3S);
	}

    //Create the scf texture list
	if (ExportMode == "GAMECUBE" || ExportMode == "WII"){
        File_MakeNew("GamemakerAnywhere/Runtime/gfx/textures.scf");
        /*std::string command = "powershell -Command \"New-Item -Path '" + std::string(initDir) + "GamemakerAnywhere/Runtime/gfx/textures.scf' -Force\"";
        system(command.c_str());*/
	}

    //Parse the yyp
    printf("Parsing YYP...\n");
    Json::Value yyp_json = ParseJSON(ProjectYYP);
    printf("Parsed YYP...\n");

    //Print the project name
    printf("Project Name: %s\n", yyp_json["name"].asCString());

    //The asset compile loop!!
    CompileAssets(yyp_json);
}

//Setup compiler (copy runtime, rest vars, etc)
bool InitCompiler(){
    //SETUP
    SDL_snprintf(RuntimePath, sizeof(RuntimePath), "%sRuntime", SDL_GetBasePath());
    struct stat sb;

    //Delete the old build if it exists
    if (stat(File_GetLocation("GamemakerAnywhere"), &sb) == 0){
        printf("Deleting old build...\n");
        File_RemoveAll("GamemakerAnywhere");
        
        while (stat(File_GetLocation("GamemakerAnywhere"), &sb) == 0){
            printf("Folder still not deleted...");
        }
    }

    //Copy the runtime folder
    printf("\nCopying runtime folder...\n");

    printf("RuntimePath: %s\n", RuntimePath);
    printf("Destination: %s\n", File_GetLocation("GamemakerAnywhere"));
    printf("Before File_CopyAll\n");
    fflush(stdout);

    File_CopyAll(RuntimePath, "GamemakerAnywhere");

    printf("After File_CopyAll\n");
    fflush(stdout);

    //Create other folders
    File_CreateDir("GamemakerAnywhere/Runtime/source/rooms");
    File_CreateDir("GamemakerAnywhere/Runtime/source/objects");
    File_CreateDir("GamemakerAnywhere/Runtime/source/sprites");
    File_CreateDir("GamemakerAnywhere/Runtime/output");
    File_CreateDir("GamemakerAnywhere/Runtime/gfx");

    //Rest vars
    currentsprite_count = 0;

    //Get the yyp
    //COMDLG_FILTERSPEC filters[] = {{ L"GameMaker Project", L"*.yyp" }};
    ProjectYYP = GetFileUI(/*filters, ARRAYSIZE(filters)*/);
    printf("Project path: %s\n", ProjectYYP);   

    //GMS vars
    VarBuiltIn_Init();
    VarBuiltIn_Write();

    if (stat(File_GetLocation("GamemakerAnywhere"), &sb) == 0)
        return true;
    else
        return false;
}

void scr_compilescripts(Json::Value yyfile){
    printf("Compiling Script...\n");
}
void scr_compilesounds(Json::Value yyfile){
    printf("Compiling Sound...\n");
}
void scr_compilefonts(Json::Value yyfile){
    printf("Compiling Font...\n");
}

void CompileAssets(Json::Value yyp_json){
    std::string yypDir = std::string(ProjectYYP);
    yypDir = yypDir.substr(0, yypDir.find_last_of("/\\"));

    for (int i = 0; i < yyp_json["resources"].size(); i++){
        //List Assets
        //printf(yyp_json["resources"][i]["id"]["name"].asCString());
        
        Json::Value _id = yyp_json["resources"][i]["id"];
        Json::Value yyfile = ParseJSON((yypDir + "/" + _id["path"].asString()).c_str());
        std::string type = yyfile["resourceType"].asString();

        if (type == "GMRoom")   scr_compilerooms(yyfile, i, yyp_json); //COMPILE ROOM
        if (type == "GMSprite") scr_compilesprites(yyfile);            //COMPILE SPRITE
        if (type == "GMObject") scr_compileobjects(yyfile, _id);            //COMPILE OBJECTS
        if (type == "GMScript") scr_compilescripts(yyfile);            //COMPILE SCRIPTS
        if (type == "GMSound")  scr_compilesounds(yyfile);             //COMPILE SOUNDS
        if (type == "GMFont")   scr_compilefonts(yyfile);              //COMPILE FONTS
    }

    //Finish off compile
    //Close sprite info brackets
    File_WriteLine(File_GetLocation("GamemakerAnywhere/Runtime/source/helpers/get_spriteinfo.cpp"), 11, "};"); //SPRITE WIDTH
	File_WriteLine(File_GetLocation("GamemakerAnywhere/Runtime/source/helpers/get_spriteinfo.cpp"), 12+1, "};"); //SPRITE HEIGHT

}

