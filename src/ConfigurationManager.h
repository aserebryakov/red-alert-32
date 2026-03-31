#ifndef RED_ALERT_32_CONFIGURATIONMANAGER_H
#define RED_ALERT_32_CONFIGURATIONMANAGER_H

#include <Arduino.h>
#include <optional>

struct Configuration {
    std::optional<String> ssid;
    std::optional<String> password;
    std::optional<String> cityName;
};

class ConfigurationManager {
public:
    ConfigurationManager(const size_t eepromSize = 2048);
    
    void begin();

    bool writeConfiguration(const Configuration &config) const;
    Configuration readConfiguration() const;
    bool reset() const;

private:
    static constexpr size_t MAX_FIELD_LENGTH = 100; // unicode characters
    static constexpr size_t BYTES_PER_FIELD = 4 * MAX_FIELD_LENGTH; // 4 bytes per unicode char for UTF-8

    static constexpr size_t OFFSET_SSID = 0;
    static constexpr size_t OFFSET_PASSWORD = OFFSET_SSID + 1 + BYTES_PER_FIELD;
    static constexpr size_t OFFSET_CITY = OFFSET_PASSWORD + 1 + BYTES_PER_FIELD;

    bool writeField(size_t offset, const std::optional<String> &field) const;
    std::optional<String> readField(size_t offset) const;

    size_t eepromSize;
};

#endif //RED_ALERT_32_CONFIGURATIONMANAGER_H
