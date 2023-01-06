#include "ConfigStorage.h"

ConfigStorage::ConfigStorage() {}
ConfigStorage::~ConfigStorage() {}
void ConfigStorage::saveSettings()
{
    EEPROM.begin(512);
    EEPROM.put(0, this->settings);
    EEPROM.commit();
    EEPROM.end();
}

Settings ConfigStorage::loadSettings()
{
    EEPROM.begin(512);
    EEPROM.get(0, this->settings);
    EEPROM.end();

    if (this->settings.salt != EEPROM_SALT)
    {
        Settings settings;
        this->settings = settings;
    }

    return this->settings;
    
}
void ConfigStorage::resetSettings()
{
    Settings settings;
    this->settings = settings;
}
