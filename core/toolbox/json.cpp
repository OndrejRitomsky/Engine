#define JSON_IMPLEMENTATION 
#define JSON_ERROR_MESSAGE
#define JSON_INPUT_STRING_IS_STORAGE
//#define JSON_ALLOC(context, size) <alloc function call> // malloc(size) by default
//#define JSON_FREE(context, data)  <free function call>   // free(data) by default
#include "json.h"



// @TODO Might move this json, when only editor will use it