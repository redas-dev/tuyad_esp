#include <tuyalink_core.h>

#ifndef _ACTIONS_
#define _ACTIONS_
    void execute_devices(const char* response_topic, tuya_mqtt_context_t* context);
    void execute_on_off_method(tuya_mqtt_context_t* context, const char* response_topic, char* json);
#endif