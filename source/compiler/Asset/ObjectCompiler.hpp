#include <iostream>
#include <iostream>
#if defined(_WIN32)
    #include <windows.h>
    #include <shobjidl.h> 
#endif
#include <sys/stat.h>
#include <vector>
#include <string>
#include <fstream>
#include "../../helpers/renderer.hpp"
#include "../../helpers/meta.hpp"
#include "../compiler_main.hpp"
#include <json/json.h>
#include <cstring>

void scr_compileobjects(Json::Value yyfile, Json::Value _id);