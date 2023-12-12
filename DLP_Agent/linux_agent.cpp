#include <iostream>
#include <unistd.h>
#include <mosquitto.h>
#include <string>
#include <ctime>
#include <chrono>
#include <thread>

#define MQTT_HOST "192.168.15.65"
#define MQTT_PORT 1883
#define MQTT_TOPIC "send_info"

struct DeviceInfo {
    int id;
    std::string name;
    std::string os;
    std::string time;
};

DeviceInfo getDeviceInfo() {
    DeviceInfo device;
    device.id = 1;

    char computerName[256];
    gethostname(computerName, sizeof(computerName));
    device.name = computerName;

    device.os = "Linux"; // Đặt cứng OS là Linux do chương trình chạy trên Linux

    std::time_t currentTime = std::time(nullptr);
    char timeBuffer[80];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S - %d/%m/%Y", std::localtime(&currentTime));
    device.time = timeBuffer;

    return device;
}

int main() {
    mosquitto_lib_init();
    struct mosquitto *mosq = mosquitto_new(NULL, true, NULL);

    if (mosq) {
        if (mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60) == MOSQ_ERR_SUCCESS) {
            while (true) {
                DeviceInfo device = getDeviceInfo();

                std::string message = "{\"id\":" + std::to_string(device.id) +
                                      ",\"name\":\"" + device.name +
                                      "\",\"os\":\"" + device.os +
                                      "\",\"time\":\"" + device.time + "\"}";

                mosquitto_publish(mosq, NULL, MQTT_TOPIC, message.size(), message.c_str(), 0, false);

                std::this_thread::sleep_for(std::chrono::seconds(5));
            }

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
