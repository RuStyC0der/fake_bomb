////////////////////////////////////////////////////////////////////////////////
// LED line
#include <Adafruit_NeoPixel.h>
#define PIN   26
#define NUMPIXELS 17

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void led_strip_setup() {
		pixels.begin();
		pixels.clear();
}

void led_strip_color(int r, int g, int b) {
		for (int i = 0; i < NUMPIXELS; i++) {
				pixels.setPixelColor(i, pixels.Color(r, g, b));
		}
		pixels.show();
}

void led_strip_Brightness(int n) {
	pixels.setBrightness(n);
	pixels.show();
}
