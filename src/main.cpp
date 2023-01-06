#include "main.h"

// MQTT client configuration
WiFiClient espClient;
PubSubClient client(espClient);

// WiFi Manager
WiFiManager wifiManager;

// Web server to manage communication between client and NeoPixel.
ESP8266WebServer server(82);

// The animation manager.
Animator animator = Animator();

// Ring controller settings and it's manager
Settings ringControllerSettings;
ConfigStorage configStorage;

// WiFiManager variables, used to get MQTT configuration
WiFiManagerParameter customMqttEnabled("e", "MQTT Enabled", "1", 1, "type='checkbox' style='width: auto;'><label for='e'> MQTT Enabled</label");
WiFiManagerParameter customMqttServer("server", "MQTT Server", ringControllerSettings.mqttServer, 16, "pattern='\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}'");
WiFiManagerParameter customMqttUser("user", "MQTT User", ringControllerSettings.mqttUser, 13);
WiFiManagerParameter customMqttPassword("password", "MQTT Password", ringControllerSettings.mqttPassword, 10);

// Flag used to indicate that user clicked on SAVE and than we can save settings in non volatile memory.
bool shouldSaveConfig = false;

/**
 * Board setup function.
*/
void setup()
{
  // Configure the board LED as output and switch it off.
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  // Read settings
  ringControllerSettings = configStorage.loadSettings();

  // Init Animator.
  animator.begin();

  // Initialize the ssid by concatenating the first 4 characters of the mac address
  String macAddress = WiFi.macAddress();
  macAddress.replace(":", "");
  ssid.concat(macAddress.substring(0, 4));

  // Setup the WiFi manager.
  setupWiFiManager();

  // Setup the HTTP Server.
  setupServer();

  // Setup the MQTT connection.
  setupMqtt();

  // Setup the board.
  setupBoard();

  // If the client send a SAVE configuration command, we must save the custom settings
  // in EEPROM.
  if (shouldSaveConfig)
  {
    saveConfig();
  }

}

/**
 * Loop function.
*/
void loop()
{
  animator.loop();
  server.handleClient();
  if (ringControllerSettings.mqttEnabled && !client.connected())
  {
    reconnectMqtt();
  }
  client.loop();
  MDNS.update();
  ArduinoOTA.handle();
}

/**
 * Called by the WiFiManager when user click on Save button, it indicates
 * that the board must save the configuration.
*/
void saveConfigCallback()
{
  shouldSaveConfig = true;
}

/**
 * Utils function to convert a byte array to char array.
*/
void byteToChar(byte *bytes, char *chars, unsigned int count)
{
  for (unsigned int i = 0; i < count; i++)
    chars[i] = (char)bytes[i];
  chars[count] = (char)0;
}

/**
 * Utils function to blink an led 2 times.
*/
void blinkLed()
{
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
}

/**
 * Swith on/off the NEOPixel ring and publish the updated state to mqtt server (if enabled).
*/
void onOffNeoPixel(char message)
{
  if (message == '1')
  {
    digitalWrite(LED, LOW);
    animator.turnOn();
    client.publish(MQTT_TOPIC_STATE, "on");
  }
  else
  {
    digitalWrite(LED, HIGH);
    animator.turnOff();
    client.publish(MQTT_TOPIC_STATE, "off");
  }

  blinkLed();
}

/**
 * Update the animation runned by the NEOPixel ring.
*/
void setAnimation(char *message)
{
  if (message[0] != 'A')
  {
    return;
  }

  const Animation currentAnimation = static_cast<Animation>(message[1] - '0');
  animator.setAnimation(currentAnimation);

  blinkLed();
}

/**
 * Reset the configuration.
*/
void reset(char *message)
{
  if (message[0] == 'r' && message[1] == 's' && message[2] == 't')
  {
    wifiManager.resetSettings();
    configStorage.resetSettings();
    ESP.eraseConfig();
    ESP.reset();
  }
}

/**
 * MQTT callback function.
*/
void callback(char *topic, byte *payload, unsigned int length)
{
  if (strcmp(topic, MQTT_TOPIC) != 0)
  {
    return;
  }

  char message[3];
  byteToChar(payload, message, length);

  if (length == 1)
  {
    onOffNeoPixel(message[0]);
  }
  if (length == 2)
  {
    setAnimation(message);
  }

  if (length == 3)
  {
    reset(message);
  }
}

/**
 * Save the configuration to non volatile memory.
*/
void saveConfig()
{
  const String mqttEnabledValue = customMqttEnabled.getValue();
  if (mqttEnabledValue.compareTo("1"))
  {
    ringControllerSettings.mqttEnabled = true;
  }
  
  strcpy(ringControllerSettings.mqttServer, customMqttServer.getValue());
  strcpy(ringControllerSettings.mqttUser, customMqttUser.getValue());
  strcpy(ringControllerSettings.mqttPassword, customMqttPassword.getValue());

  configStorage.saveSettings();

}

/**
 * WiFiManager setup. It adds 4 custom settings.
*/
void setupWiFiManager()
{
  wifiManager.setDebugOutput(false);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&customMqttEnabled);
  wifiManager.addParameter(&customMqttServer);
  wifiManager.addParameter(&customMqttUser);
  wifiManager.addParameter(&customMqttPassword);
  wifiManager.setConfigPortalTimeout(60);
  wifiManager.autoConnect(ssid.c_str(), password);

}

/**
 * Check if client that sent a request is authorized.
*/
bool isAuthrorized()
{
  if (!server.hasHeader(authName) || !server.header(authName).equals(authValue))
  {
    return false;
  }

  return true;
}

/**
 * Setup the HTTP server.
*/
void setupServer()
{
  // React to unmanaged urls
  server.onNotFound([]()
                    { server.send(404, "text/plain", "Uri not found " + server.uri()); });
  server.on("/reset", []()
            {
              if (isAuthrorized())
              {
                reset(const_cast<char *>("rst"));
              }

              server.send(200, "text/plain"); });

  server.on("/onOff", []()
            {
              if (isAuthrorized())
              {
                String arg = server.arg(0);
                if (arg.length() == 1)
                {
                  onOffNeoPixel(arg.c_str()[0]);
                }
              }
              server.send(200, "text/plain", server.arg(0)); });

  server.on("/setAnimation", []()
            {
              if (isAuthrorized())
              {
                setAnimation((char *)server.arg(0).c_str());
              }
              server.send(200, "text/plain", server.arg(0)); });

  const char *headerkeys[] = {authName.c_str()};
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char *);
  server.collectHeaders(headerkeys, headerkeyssize);
  server.begin();
}

/**
 * Setup the MQTT server.
*/
void setupMqtt()
{
  clientId.concat(ESP.getChipId());
  client.setServer(ringControllerSettings.mqttServer, 1883);
  client.setCallback(callback);
}

/**
 * If enabled, reconnect the board to the MQTT server.
*/
void reconnectMqtt()
{
  while (!client.connected() && retriesLeft-- > 0)
  {
    if (client.connect(
            clientId.c_str(),
            ringControllerSettings.mqttUser,
            ringControllerSettings.mqttPassword,
            MQTT_TOPIC_STATUS,
            MQTT_QOS,
            MQTT_RETAIN,
            MQTT_WILL_PAYLOAD))
    {
      client.publish(MQTT_TOPIC_STATUS, "online", true);
      client.subscribe(MQTT_TOPIC);
    }
    else
    {
      delay(1000);
    }
  }
}

/**
 * Setup board's common settings.
*/
void setupBoard()
{
  
  // Set board hostname
  WiFi.hostname(HOSTNAME);

  // Begin MDNS service to resolve hostname
  MDNS.begin(HOSTNAME, WiFi.localIP());
  MDNS.addService(HOSTNAME, "tcp", 8080);

  // Setup OTA
  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.begin();

}