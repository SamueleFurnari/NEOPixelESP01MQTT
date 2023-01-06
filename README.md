# Overview

This project contains the implementation of an IoT gizmo to control a NEOPixel Ring. The code was developed and tested for the ESP-01 1M board (which mounts the ESP8266 chip with one mega byte of flash memory).

The code offers:
- A complete WiFi connection manager, implemented using [WiFiManager](https://tzapu.com/esp8266-wifi-connection-manager-library-arduino-ide/). The library is used to simplify the management of the connection to the WiFi network (thus avoiding sculpting the network credentials in the code) and eventually to configure  the connection to an MQTT server.
- The ability to interact with an MQTT server, implemented with the [PubSubClient](https://www.arduino.cc/reference/en/libraries/pubsubclient/) library.
- An HTTP Server that allow user to interact with the board using a couple of HTTP urls. This is implemented using the [ESP8266WebServer](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer).
- Support for OTA Updates implemented using the [ArduinoOTA](https://github.com/JAndrassy/ArduinoOTA) library.
- Support for hostname resolution, implemented using the [Multicast DNS](https://github.com/arduino/esp8266/tree/master/libraries/ESP8266mDNS). This allows you to "reach" the board without having to know the IP assigned by your router to the board.

# Setting up environment.

The project is developed using [Visual Studio Code](https://code.visualstudio.com/) and [PlatformIO](https://platformio.org/). See the official documentation to learn how to install and configure PlatformIO on VSCode.

# Project structure

The project is composed by 8 files that contains:
- The definition and implementation of the class used for animation management (_Animator.h_, _Animator.cpp_ and _hsv.h_). It is important and necessary to underline that the code that implements the animations is a re-adaptation of two other open projects:
  - The Flame animation was taken from the Lon Koenig [NEOPixel Jack o Lantern](https://learn.adafruit.com/neopixel-jack-o-lantern) project.
  - The others animations was taken from the Róbert Ulbricht [NeoPixel Ring HSV rotating circles](https://www.arduinoslovakia.eu/blog/2018/4/neopixel-ring-hsv-rotujuce-kruhy?lang=en) project.

- The definition and implementation of the class used to store and load additional configuration from the simulated EEPROM (_ConfigStorage.h_ and _ConfigStorage.cpp_)

- The definition and implementation of main code (_main.h_ and _main.cpp_).

- The _Configuration.h_, that contains all the static configuration definition. You can edit this file to easly change the project configuration. You can find more information following.

# After first flash flow

After first flash, when you apply power to the board, the WiFiManager configure the board as an Access Point. If you explore the WiFi networks, you should see a protected network that has name starting with _RingController__.

Connect to this network using the passkey _1234567890_. When the connection is completed, the operating system automatically open the capitivare portal that allow you to configure the connection to the network and eventually configure the MQTT connection.

Click on Save button to save the configuration. The WiFiManager now connects the board to your WiFi network.

If the connection fails, the WiFiManager shows again the Access Point.

# Change the configuration

The code is reach of comments that explain all configuration and algorithms, here I report the main configuration parameters related to security. To edit the configuration, open the Configuration.h file.

- _password_, is the Access Point password.
- _authValue_, is the value that the client must specify as Authorization header in the HTTP request.

# HTTP Shortcuts project

In the _HTTP Shortcuts_ folder you can find a json file that was exported from _HTTP Shortcuts_ Android app. You can download the app from the _Play store_ and than import this file to easily control the board from a smartphone connected to the same network of the device.

As an alternative, in the next paragraph you can find the _curl_ for all supported functions and, in another following paragraph, the topic and messages that can be used to interact via MQTT.

# HTTP curl

To witch on and off the ring, use this curl:

```console
curl -XGET -H 'Authorization: jfjfrr99e3' 'http://ring_controller.local:82/onOff?v={{OnOff}}'
```

Replace {{OnOff}} with 0 to switch off the ring; with 1 to switch on the ring.

To reset the configuration, use this curl:

```console
curl -XGET -H 'Authorization: jfjfrr99e3' 'http://ring_controller.local:82/reset'
```

To change animation, use this curl:

```console
curl -XGET -H 'Authorization: jfjfrr99e3' 'http://Ring_Controller.local:82/setAnimation?v={{Animation}}'
```

Replace {{Animation}} according to the _Value_ column in the following table to change animation.

| Animation                  | Value |
|----------------------------|-------|
| Flame                      | A0    |
| Rotating rainbow           | A1    |
| Rotating red               | A2    |
| Rotating with change color | A3    |


# MQTT

##TODO