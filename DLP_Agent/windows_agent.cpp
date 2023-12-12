// agent.cpp

#include <iostream>
#include <Windows.h>
#include <mosquitto.h>
#include <string>
#include <ctime>
#include <chrono>
#include <thread>

#define MQTT_HOST "192.168.15.65" // Địa chỉ IP hoặc hostname của MQTT broker
#define MQTT_PORT 1883 // Port MQTT broker đang lắng nghe
#define MQTT_TOPIC "send_info" // Topic để gửi tin nhắn

struct DeviceInfo {
    int id;
    std::string name;
    std::string os;
    std::string time;
};

// Hàm lấy thông tin thiết bị
DeviceInfo getDeviceInfo() {
    DeviceInfo device;
    device.id = 1; // Số thứ tự ID

    // Lấy tên thiết bị trên hệ điều hành Windows
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computerName);
    GetComputerNameA(computerName, &size);
    device.name = computerName;

    // Hệ điều hành của thiết bị
    // Hệ điều hành của thiết bị (có thể lấy thông tin từ hệ thống)
    #ifdef _WIN32
        device.os = "Windows";
    #elif __APPLE__
        device.os = "MacOS";
    #elif __linux__
        device.os = "Linux";
    #else
        device.os = "Other";
    #endif

    // Thời gian hiện tại
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
                DeviceInfo device = getDeviceInfo(); // Lấy thông tin thiết bị

                // Tạo thông điệp JSON từ thông tin thiết bị
                std::string message = "{\"id\":" + std::to_string(device.id) +
                                      ",\"name\":\"" + device.name +
                                      "\",\"os\":\"" + device.os +
                                      "\",\"time\":\"" + device.time + "\"}";

                mosquitto_publish(mosq, NULL, MQTT_TOPIC, message.size(), message.c_str(), 0, false);

                // Chờ 5 giây trước khi gửi tin nhắn tiếp theo
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
