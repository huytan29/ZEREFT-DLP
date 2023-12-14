// server.cpp

#include <iostream>
#include <mosquitto.h>

#define MQTT_HOST "192.168.15.65" // Địa chỉ IP hoặc hostname của MQTT broker
#define MQTT_PORT 1883 // Port MQTT broker đang lắng nghe
#define MQTT_TOPIC "send_info" // Topic để nhận tin nhắn

void message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message) {
    if(message->payloadlen){
        std::string received_message(static_cast<const char*>(message->payload), message->payloadlen);
        std::cout << "Received message: " << received_message << std::endl;
        // Xử lý thông điệp nhận được ở đây
    } else {
        std::cout << "Empty message received" << std::endl;
    }
}

int main() {
    mosquitto_lib_init();
    struct mosquitto *mosq = mosquitto_new(NULL, true, NULL);

    if (mosq) {
        mosquitto_message_callback_set(mosq, message_callback);

        if (mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60) == MOSQ_ERR_SUCCESS) {
            mosquitto_subscribe(mosq, NULL, MQTT_TOPIC, 0);

            int rc;
            do {
                rc = mosquitto_loop(mosq, -1, 1);
            } while (rc == MOSQ_ERR_SUCCESS);

            mosquitto_disconnect(mosq);
        } else {
            std::cerr << "Could not connect to MQTT broker" << std::endl;
        }

        mosquitto_destroy(mosq);
    } else {
        std::cerr << "Failed to create MQTT client" << std::endl;
    }

    mosquitto_lib_cleanup();
    return 0;
}
