#ifndef NEO_PIXEL_ANIMATIONS_H
#define NEO_PIXEL_ANIMATIONS_H

#include <Adafruit_NeoPixel.h>

/// @brief Supported animations.
enum Animation
{
    Flame,
    RotatingRainbow,
    RotatingRed,
    RotatingChangesColorHSV
};

/// @brief Manage the NEOPixel ring.
class Animator
{
private:

    // Flame animation (see https://learn.adafruit.com/neopixel-jack-o-lantern for more info).
    void InitFlames();
    void UpdateFlameColor(byte flame_num, int new_brightness);
    void CreateNewFlame(byte flame_num); 
    int GetStepSize();
    int GetMaxBrightness();
    void flame();

    // NeoPixel Ring HSV rotating circles (see https://www.arduinoslovakia.eu/blog/2018/4/neopixel-ring-hsv-rotujuce-kruhy?lang=en for more info).
    void rotatingRainbowRing();
    void rotatingRed();
    void rotatingChangesColorHSV();

    void showAnimation();

public:
    Animator();
    ~Animator();
    void begin();
    void loop();
    void turnOn();
    void turnOff();
    void setAnimation(Animation animation);
};

#endif