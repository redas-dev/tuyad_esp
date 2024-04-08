#define _DEFAULT_SOURCE

#include <cjson/cJSON.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tuya_esp_utils.h"

// Create the response object for 'devices' method
extern char* create_tuya_devices_response(struct blob_attr* tb[])
{
    // Create response object
    cJSON* obj = cJSON_CreateObject();
    cJSON* data = cJSON_AddObjectToObject(obj, "data");
    cJSON_AddStringToObject(data, "actionCode", "devices");
    cJSON* outputParams = cJSON_AddObjectToObject(data, "outputParams");

    cJSON* deviceArray = get_device_array(tb);

    // Add array to output params
    cJSON_AddItemToObject(outputParams, "devices", deviceArray);

    char* resp = cJSON_PrintUnformatted(obj);

    // Clean up
    free_obj_array(deviceArray);
    cJSON_DeleteItemFromObject(outputParams, "devices");
    cJSON_DeleteItemFromObject(data, "outputParams");
    cJSON_DeleteItemFromObject(data, "actionCode");
    cJSON_DeleteItemFromObject(obj, "data");
    cJSON_Delete(obj);

    return resp;
}

// Get each device that is connected through USB and is an ESP controller
extern cJSON* get_device_array(struct blob_attr* tb[])
{
    struct blob_attr* attr;
    int len = blobmsg_data_len(tb[DEVICES_DATA]);
    
    // Initiate array
    cJSON* arr = cJSON_CreateArray();

    // Get all devices and construct their JSON
    __blob_for_each_attr(attr, blobmsg_data(tb[DEVICES_DATA]), len){
        struct blob_attr* tbb[__DEVICE_MAX];

        blobmsg_parse(device_policy, __DEVICE_MAX, tbb, blobmsg_data(attr), blobmsg_len(attr));

        cJSON* temp = cJSON_CreateObject();

        cJSON_AddStringToObject(temp, "ProductID", blobmsg_get_string(tbb[PRODUCTID]));
        cJSON_AddStringToObject(temp, "VendorID", blobmsg_get_string(tbb[VENDORID]));
        cJSON_AddStringToObject(temp, "Port", blobmsg_get_string(tbb[PORT]));

        cJSON_AddItemToArray(arr, temp);
    }

    return arr;
}

// Format JSON object into string
extern char* get_device_array_json(cJSON* obj)
{
    return  cJSON_PrintUnformatted(obj);
}

// Release memory allocated for the devices array
extern void free_obj_array(cJSON* arr)
{
    uint32_t arrSize = cJSON_GetArraySize(arr);

    for(int i = 0; i < arrSize; i++){
        // Get i'th element from array
        cJSON* temp = cJSON_DetachItemFromArray(arr, i);

        // Delete data from object
        cJSON_DeleteItemFromObject(temp, "Port");
        cJSON_DeleteItemFromObject(temp, "VendorID");
        cJSON_DeleteItemFromObject(temp, "ProductID");
        cJSON_Delete(temp);
    }
}

// Create command JSON for ubus
char* create_json_command(char* port, double pin)
{
    cJSON* obj = cJSON_CreateObject();

    cJSON_AddStringToObject(obj, "port", port);
    cJSON_AddNumberToObject(obj, "pin", pin);

    char* json = cJSON_Print(obj);

    
    cJSON_Delete(obj);

    return json;
}

// Format topic: tylink/${device_id}/thing/action/execute_response
const char* get_topic(const char* deviceID)
{
    static char topic[100];
    snprintf(topic, sizeof(topic), "tylink/%s/thing/action/execute_response", deviceID);
    return topic;
}

// Parse params from tuya to usable format
void parse_input_params(double* pin, char** port, char** method, char* json)
{
    cJSON* obj = cJSON_Parse(json);
    cJSON* params = cJSON_GetObjectItem(obj, "inputParams");

    cJSON* cPin = cJSON_GetObjectItem(params, "pin");
    (*pin) = cPin->valuedouble;

    cJSON* cMethod = cJSON_GetObjectItem(params, "method");
    (*method) = strdup(cMethod->valuestring);

    cJSON* cPort = cJSON_GetObjectItem(params, "port");
    (*port) = strdup(cPort->valuestring);

    cJSON_DeleteItemFromObject(params, "pin");
    cJSON_DeleteItemFromObject(params, "method");
    cJSON_DeleteItemFromObject(params, "port");
    cJSON_DeleteItemFromObject(obj, "inputParams");
    cJSON_DeleteItemFromObject(obj, "actionCode");
    cJSON_Delete(obj);
}

// Create a response object for ubus command message (on/off)
char* construct_method_response(char* deviceResp)
{   
    // Create response object
    cJSON* obj = cJSON_CreateObject();
    cJSON* data = cJSON_AddObjectToObject(obj, "data");
    cJSON_AddStringToObject(data, "actionCode", "devices");
    cJSON* outputParams = cJSON_AddObjectToObject(data, "outputParams");

    // Add the response from ubus to the object
    cJSON_AddStringToObject(outputParams, "msg", deviceResp);

    char* response = cJSON_PrintUnformatted(obj);

    // Cleanup
    cJSON_DeleteItemFromObject(outputParams, "msg");
    cJSON_DeleteItemFromObject(data, "outputParams");
    cJSON_DeleteItemFromObject(data, "actionCode");
    cJSON_DeleteItemFromObject(obj, "data");
    cJSON_Delete(obj);

    return response;
}