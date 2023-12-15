#include "../headers/main.h"
#include <stdio.h>
#include "../headers/mosquitto.h"

#define MQTT_HOST "192.168.15.65"
#define MQTT_PORT 1883
#define MQTT_TOPIC "agent_options"

void executeCommand(char option) {
    switch (option) {
        case 'c':
            blockScreen(1, nullptr);
            break;
        case 'w':
            blockWebsites();
            break;
        case 'i':
            blIPaddress();
            break;
        case 'p':
            blockApp();
            break;
        case 'u':
            blockUSB();
            break;
        case 's': {
            HINSTANCE hInstance = GetModuleHandle(NULL);
            HINSTANCE hPrevInstance = NULL;
            PSTR szCmdLine = NULL;
            int iCmdShow = SW_SHOW;
            int result = blockEdit(hInstance, hPrevInstance, szCmdLine, iCmdShow);
            break;
        }
        case 'e':
            printf("Exit.\n");
            break;
        default:
            printf("Invalid option received.\n");
            break;
    }
}

void message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message) {
    if (message->payloadlen) {
        char received_option = *((char *)message->payload);
        executeCommand(received_option);
    } else {
        printf("Empty message received\n");
    }
}

void receiveOptionFromServer() {
    mosquitto_lib_init();
    struct mosquitto *mosq = mosquitto_new(NULL, true, NULL);

    if (mosq) {
        mosquitto_message_callback_set(mosq, message_callback);
        printf("Connected to MQTT broker\n");
        if (mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60) == MOSQ_ERR_SUCCESS) {
            mosquitto_subscribe(mosq, NULL, MQTT_TOPIC, 0);
            int rc;
            do {
                rc = mosquitto_loop(mosq, -1, 1);
            } while (rc == MOSQ_ERR_SUCCESS);

            mosquitto_disconnect(mosq);
        } else {
            printf("Could not connect to MQTT broker\n");
        }

        mosquitto_destroy(mosq);
    } else {
        printf("Failed to create MQTT client\n");
    }

    mosquitto_lib_cleanup();
}

int main() {
    // Nhận và xử lý lựa chọn từ server
    receiveOptionFromServer();

    return 0;
}
