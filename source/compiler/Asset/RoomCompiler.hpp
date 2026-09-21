#include <iostream>
#include <iostream>
#if defined(_WIN32)
    #include <windows.h>
    #include <shobjidl.h> 
#endif
#include <sys/stat.h>
#include "../../helpers/renderer.hpp"
#include "../../helpers/meta.hpp"
#include "../compiler_main.hpp"
#include <json/json.h>

void scr_compilerooms(Json::Value yyfile, int i, Json::Value yyp_json);