#include <mqtt_client_interface.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>

#include "tuya_esp_ubus_utils.h"
#include "actions.h"
#include "tuya_esp_utils.h"

// Gets the list of connected devices through serial and returns the list to tuya
void execute_devices(const char* response_topic, tuya_mqtt_context_t* context)
{
    char* devicesJSON = NULL;
    get_esp_devices(&devicesJSON);

    if (devicesJSON == NULL){
      devicesJSON = "{\"data\":{\"actionCode\":\"method\", \"outputParams\": {\"msg\": \"Failed to get devices.\"}}}";
    }
    // Send response back to tuya
    mqtt_client_publish(context->mqtt_client, response_topic, (const uint8_t*)devicesJSON, strlen(devicesJSON), 0);

    free(devicesJSON);
}

// Turns specified pin of device on/off
void execute_on_off_method(tuya_mqtt_context_t* context, const char* response_topic, char* json)
{
    double pin = -1;
    char* port = NULL;
    char* action = NULL;
    char* response = NULL;

    parse_input_params(&pin, &port, &action, json);
    
    send_action_to_device(port, pin, action, &response);

    free(port);
    free(action);

    if (response == NULL){
      syslog(LOG_ERR, "Method response was NULL");
      return;
    }

    mqtt_client_publish(context->mqtt_client, response_topic, (const uint8_t*)response, strlen(response), 0);

    free(response);
}