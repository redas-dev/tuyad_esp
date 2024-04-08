#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <signal.h>

#include <tuya_error_code.h>
#include <tuya_log.h>

#include "arg_parser.h"
#include "tuya_esp_utils.h"
#include "tuya_cloud.h"
#include "make_daemon.h"
#include "tuya_esp_entry.h"
#include "tuya_esp_ubus_utils.h"

#include <syslog.h>

tuya_mqtt_context_t client_instance;

volatile sig_atomic_t signal_sent = 0;

// Handle sigint and sigquit signals
void sig_handler(int signum)
{
    signal_sent = 1;
}

int entry(int argc, char** argv)
{
    openlog("tuyad_esp", LOG_PID, LOG_DAEMON);

    struct sigaction sa = { .sa_handler = sig_handler };
    sigaction(SIGINT, &sa, 0);
    sigaction(SIGQUIT, &sa, 0);
    sigaction(SIGHUP, &sa, 0);
    sigaction(SIGABRT, &sa, 0);
    sigaction(SIGTERM, &sa, 0);

    int ret = OPRT_OK;

    // Get arguments from console
    struct arguments args = parse_args(argc, argv);

    // Make program daemon
    if(args.isDaemon){
        make_daemon(0);
        fclose(stdout);
        fclose(stderr);
    }

    tuya_mqtt_context_t* client = &client_instance;
    ret = connect_to_tuya(client, args);

    if(ret != OPRT_OK){
        disconnect_from_tuya(client);
        closelog();
        return EXIT_FAILURE;
    }

    ret = connect_to_ubus();

    if (ret != 0){
        disconnect_from_ubus();
        closelog();
        return EXIT_FAILURE;
    }

    // Wait for messages to come
    while (!signal_sent) {
        tuya_mqtt_loop(client);
    }

    // Disconnect and free memory
    syslog(LOG_INFO, "%s", "Starting cleanup");

    disconnect_from_tuya(client);
    disconnect_from_ubus();

    syslog(LOG_INFO, "%s", "Cleanup ended");
    closelog();

    return 0;
}