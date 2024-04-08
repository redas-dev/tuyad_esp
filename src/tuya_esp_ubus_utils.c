#include <stdlib.h>
#include <syslog.h>
#include <cjson/cJSON.h>

#include "tuya_esp_ubus_utils.h"
#include "tuya_esp_utils.h"

struct ubus_context* ctx;

int connect_to_ubus()
{
    ctx = ubus_connect(NULL);

    if (!ctx){
        ubus_free(ctx);
        syslog(LOG_ERR, "%s", "Failed to connect to ubus");
        return -1;
    }
    syslog(LOG_INFO, "%s", "Connected to ubus");

    return 0;
}

// Parse response from ubus
static void devices_callback(struct ubus_request *req, int type, struct blob_attr *msg) 
{
    char** deviceJSON = (char**)req->priv;
	struct blob_attr* tb[__DEVICES_MAX];

    // Get the device array
	blobmsg_parse(devices_policy, __DEVICES_MAX, tb, blob_data(msg), blob_len(msg));
    
    if (!tb[DEVICES_DATA]){
        syslog(LOG_ERR, "Couldn't parse device array");
        return;
    }

    // Create a response JSON that should be sent to tuya
    (*deviceJSON) = create_tuya_devices_response(tb);
}

// Invoke methods to get list of connected devices
int get_esp_devices(char** devices)
{
    int res = 0;
    uint32_t id = 0;

    // Get id of espcontroller ubus object
    res = ubus_lookup_id(ctx, "espcontroller", &id);
    if (res != 0){
        syslog(LOG_INFO, "%s", "Failed to get id of ESPController");
        return res;
    }

    // Invoke the 'devices' method on espcontroller
    res = ubus_invoke(ctx, id, "devices", NULL, devices_callback, devices, 3000);

    if (res != 0){
        syslog(LOG_INFO, "%s", "Couldn't get the devices service");
        return res;
    }

    return res;
}

// Execute on/off command on pin
static void action_callback(struct ubus_request *req, int type, struct blob_attr *msg) 
{
    char** resp = (char**)req->priv;

    char* response = blobmsg_get_string(blob_data(msg));

    (*resp) = construct_method_response(response);
}

// Invoke methods to turn specified pins on/off through ubus
int send_action_to_device(char* port, double pin, char* action, char** response)
{
    int res = 0;
    uint32_t id = 0;

    // Get id of espcontroller ubus object
    res = ubus_lookup_id(ctx, "espcontroller", &id);
    if (res != 0){
        syslog(LOG_INFO, "%s", "Failed to get id of ESPController");
        return res;
    }

    // Generate command input JSON
    struct blob_buf b = {};
    blob_buf_init(&b, 0);

    char* json = create_json_command(port, pin);
    blobmsg_add_json_from_string(&b, json);

    // Send on/off command to ubus with JSON parameters
    res = ubus_invoke(ctx, id, action, b.head, action_callback, response, 3000);

    blob_buf_free(&b);
    free(json);
    
    if (res != 0){
        syslog(LOG_INFO, "%s %s %s", "Couldn't get the", action, "service");
        return res;
    }
}

void disconnect_from_ubus()
{
    ubus_free(ctx);
    syslog(LOG_INFO, "%s", "Disconected from ubus");
}