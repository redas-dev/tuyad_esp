#include <libubox/blobmsg_json.h>
#include <libubox/blobmsg.h>
#include <libubus.h>

#ifndef _UBUS_UTILS_
#define _UBUS_UTILS_
    enum {
        VENDORID,
        PRODUCTID,
        PORT,
        __DEVICE_MAX
    };

    static const struct blobmsg_policy device_policy[__DEVICE_MAX] = {
        [VENDORID]	  = { .name = "VendorID",  .type = BLOBMSG_TYPE_STRING },
        [PRODUCTID]	  = { .name = "ProductID", .type = BLOBMSG_TYPE_STRING },
        [PORT]	      = { .name = "Port",      .type = BLOBMSG_TYPE_STRING },
    };

    enum {
        DEVICES_DATA,
        __DEVICES_MAX,
    };

    static const struct blobmsg_policy devices_policy[__DEVICES_MAX] = {
        [DEVICES_DATA] = { .name = "devices", .type = BLOBMSG_TYPE_ARRAY },
    };
    
    int connect_to_ubus();
    int get_esp_devices(char** devices);
    int send_action_to_device(char* port, double pin, char* action, char** response);
    void disconnect_from_ubus();
#endif