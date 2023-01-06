#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// Board LED
#define LED 1

// MQTT topic, QOS, retain, will payload
#define MQTT_TOPIC "ringcontroller"               // Subscribed, here the board receive commands
#define MQTT_TOPIC_STATUS "ringcontroller/status" // Here the board publish the ring status (online/offline)
#define MQTT_TOPIC_STATE "ringcontroller/state"   // Here the board publish if ring is on or off
#define MQTT_QOS 1
#define MQTT_RETAIN true
#define MQTT_WILL_PAYLOAD "offline"

// Board hostname
#define HOSTNAME "Ring_Controller"

// AP name and password
String ssid = "RingController_";
const char *password = "1234567890";

// MQTT client id and max retry to connect
String clientId = "ringcontroller";
int retriesLeft = 3;

// Authorization variables
const String authName = "Authorization";
const String authValue = "jfjfrr99e3";

#endif