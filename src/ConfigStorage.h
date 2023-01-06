#ifndef CONFIG_STORAGE_H
#define CONFIG_STORAGE_H

#include <EEPROM.h>

#define EEPROM_SALT 0120221
typedef struct
{
    int salt = EEPROM_SALT;
    bool mqttEnabled = false;
    char mqttServer[16] = "192.168.1.1";
    char mqttUser[13] = "";
    char mqttPassword[10] = "";
} Settings;

class ConfigStorage
{
private:
    Settings settings;

public:
    ConfigStorage();
    ~ConfigStorage();
    void saveSettings();
    Settings loadSettings();
    void resetSettings();
};

#endif