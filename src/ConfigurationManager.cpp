#include "ConfigurationManager.h"
#include <EEPROM.h>

ConfigurationManager::ConfigurationManager(const size_t eepromSize) : eepromSize(eepromSize) {}

void ConfigurationManager::begin() {
    if (!EEPROM.begin(eepromSize)) {
        Serial.println("Failed to initialise EEPROM");
    }
}

bool ConfigurationManager::writeConfiguration(const Configuration &config) const {
    if (!writeField(OFFSET_SSID, config.ssid)) {
        return false;
    }
    if (!writeField(OFFSET_PASSWORD, config.password)) {
        return false;
    }
    if (!writeField(OFFSET_CITY, config.cityName)) {
        return false;
    }
    return EEPROM.commit();
}

Configuration ConfigurationManager::readConfiguration() const {
    return {
        readField(OFFSET_SSID),
        readField(OFFSET_PASSWORD),
        readField(OFFSET_CITY)
    };
}

bool ConfigurationManager::reset() const {
    if (!writeField(OFFSET_SSID, String(""))) {
        return false;
    }
    return EEPROM.commit();
}

bool ConfigurationManager::writeField(size_t offset, const std::optional<String> &field) const {
    Serial.print("Field ");
    Serial.print(offset);

    if (!field.has_value()) {
        Serial.println(" has no value and will not be changed");
        return true;
    }

    Serial.println(" has value: " + *field);
    EEPROM.write(offset, 1);
    const String value = *field;
    const size_t len = value.length();
    if (len > BYTES_PER_FIELD) {
        return false;
    }

    for (size_t i = 0; i < len; i++) {
        EEPROM.write(offset + 1 + i, value[i]);
    }

    // Write null terminator if there is space
    if (len < BYTES_PER_FIELD) {
        EEPROM.write(offset + 1 + len, '\0');
    }

    return true;
}

std::optional<String> ConfigurationManager::readField(size_t offset) const {
    uint8_t flag = EEPROM.read(offset);
    if (flag == 0) {
        return std::nullopt;
    }
    
    char buffer[BYTES_PER_FIELD + 1];
    for (size_t i = 0; i < BYTES_PER_FIELD; i++) {
        buffer[i] = EEPROM.read(offset + 1 + i);
        if (buffer[i] == '\0') {
            break;
        }
    }
    buffer[BYTES_PER_FIELD] = '\0';
    return String(buffer);
}
