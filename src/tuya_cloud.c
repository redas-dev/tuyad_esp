#include <cjson/cJSON.h>
#include <tuya_log.h>
#include <tuya_error_code.h>
#include <system_interface.h>
#include <mqtt_client_interface.h>
#include <syslog.h>
#include <stdlib.h>
#include "tuya_cacert.h"

#include "tuya_esp_utils.h"
#include "tuya_esp_ubus_utils.h"
#include "tuya_cloud.h"
#include "actions.h"

// Execute action sent from the cloud
extern void execute_method(tuya_mqtt_context_t* context, char* method, char* json)
{
  const char* response_topic = get_topic(context->config.device_id);

  // Execute devices method
  if (strcmp(method, "devices") == 0){
    execute_devices(response_topic, context);
  }
  // Execute ON/OFF methods
  else if (strcmp(method, "method") == 0){
    execute_on_off_method(context, response_topic, json);
  }
}

extern void on_connected(tuya_mqtt_context_t* context, void* user_data)
{
  syslog(LOG_INFO, "%s", "Connected to Tuya IOT");
}

extern void on_disconnect(tuya_mqtt_context_t* context, void* user_data)
{
  syslog(LOG_INFO, "%s", "Disconnected from Tuya IOT");
}

// Parse Tuya messages
extern void on_messages(tuya_mqtt_context_t* context, void* user_data, const tuyalink_message_t* msg)
{
  switch (msg->type) {
    case THING_TYPE_ACTION_EXECUTE:{
      // Get the name of the action that has to be executed
      cJSON* temp = cJSON_Parse(msg->data_string);
      char* method = cJSON_GetObjectItem(temp, "actionCode")->valuestring;

      execute_method(context, method, msg->data_string);

      cJSON_Delete(temp);

      break;
    }
    default:
      syslog(LOG_INFO, "Message received id:%s, type:%d, code:%d", msg->msgid, msg->type, msg->code);
    break;
  }
}

extern int connect_to_tuya(tuya_mqtt_context_t* client, struct arguments args)
{
  syslog(LOG_INFO, "%s", "Connecting to Tuya IOT");

  int ret = OPRT_OK;
  
  // Initialize SDK
  ret = tuya_mqtt_init(client, &(const tuya_mqtt_config_t) {
    .host = "m1.tuyacn.com",
    .port = 8883,
    .cacert = tuya_cacert_pem,
    .cacert_len = sizeof(tuya_cacert_pem),
    .device_id = args.deviceId,
    .device_secret = args.deviceSecret,
    .keepalive = 60,
    .timeout_ms = 2000,
    .on_connected = on_connected,
    .on_disconnect = on_disconnect,
    .on_messages = on_messages
  });

  if (ret != OPRT_OK){
    syslog(LOG_ERR, "%s", "Tuya IOT SDK initialization failed");
    return ret;
  }
  syslog(LOG_INFO, "%s", "Tuya IOT SDK initialization successful");

  // Connect to Tuya
  ret = tuya_mqtt_connect(client);
  if (ret != OPRT_OK){
    syslog(LOG_ERR, "%s", "Connection to Tuya IOT failed");
    return ret;
  }

  syslog(LOG_INFO, "%s", "Connection to Tuya IOT was successful");

  return ret;
}

// Disconnect from Tuya cloud and deinitialize the SDK
extern void disconnect_from_tuya(tuya_mqtt_context_t* client)
{
  tuya_mqtt_disconnect(client);
  tuya_mqtt_deinit(client);
}