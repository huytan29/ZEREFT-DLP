#include <iostream>
#include "../headers/mosquitto.h"
#include <string>
using namespace std;

#define MQTT_HOST "192.168.15.65"
#define MQTT_PORT 1883
#define MQTT_TOPIC "agent_options"

// Hàm gửi lựa chọn tới agent
void sendOptionToAgent(char option)
{
    struct mosquitto *mosq = NULL;
    mosquitto_lib_init();

    mosq = mosquitto_new(NULL, true, NULL);
    if (mosq)
    {
        if (mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60) == MOSQ_ERR_SUCCESS)
        {
            mosquitto_publish(mosq, NULL, MQTT_TOPIC, sizeof(char), &option, 0, false);
            printf("Sent option to agent: %c\n", option);
            mosquitto_disconnect(mosq);
        }
        else
        {
            cerr << "Failed to connect to MQTT broker." << endl;
        }
        mosquitto_destroy(mosq);
    }
    else
    {
        cerr << "Failed to create MQTT client." << endl;
    }

    mosquitto_lib_cleanup();
}

int main()
{
    char option;
    bool running = true;

    cout << "Enter your choice (c: blockScreen, w: blockWebsites, i: blIPaddress, p: blockApp, u: blockUSB, s: blockEdit, e: Exit): ";
    
    while (running)
    {
        cin >> option;

        if (option == 'e')
        {
            running = false;
        }
        else
        {
            sendOptionToAgent(option);
        }
    }

    return 0;
}
