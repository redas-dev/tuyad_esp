#include <cjson/cJSON.h>
#include "arg_parser.h"
#include "tuya_esp_ubus_utils.h"

#ifndef _JSON_UTILS_
#define _JSON_UTILS_
    extern cJSON* get_device_array(struct blob_attr* tb[]);
    extern char* get_device_array_json(cJSON* obj);
    extern void free_obj_array(cJSON* obj);
    char* create_json_command(char* port, double pin);
    extern char* create_tuya_devices_response(struct blob_attr* tb[]);
    const char* get_topic(const char* deviceID);
    void parse_input_params(double* pin, char** port, char** method, char* json);
    char* construct_method_response(char* deviceResp);
#endif