#include <Adafruit_NeoPixel.h>

struct Pixels {
  uint32_t startTime;
  int32_t progress;
  uint32_t time;
  uint16_t currentSceneLength;
  bool progressing;
  Adafruit_NeoPixel* strip;
  uint8_t* pixelsCopy;

  void start(uint16_t numPixels) {
    start(numPixels, 6, millis());
  }
  void start(uint16_t numPixels, uint8_t pin) {
    start(numPixels, pin, millis());
  }
  void start(uint16_t numPixels, uint8_t pin, uint32_t start) {
    startTime = start;
    strip = new Adafruit_NeoPixel(numPixels, pin, NEO_GRB + NEO_KHZ800);
    pixelsCopy = malloc(numPixels * 3);
    strip->begin();
    strip->clear();
    strip->show();
  }

  Pixels* run() {
    return run(millis());
  }
  Pixels* run(uint32_t timeRef) {
    time = timeRef;
    progress = time - startTime;
    progressing = true;

    return this;
  }

  Pixels* scene(uint32_t duration) {  // TODO : rewrite, it looks like shit
    if (progress > duration || progress < 0) {
      progressing = false;
    } else if (progress < duration) {
      progressing = true;

      float normalizedProgress = progress / (float)duration;

      if (normalizedProgress >= 0.90) {
        copyPixelsBuffer();
      }
    }

    progress -= duration;
    currentSceneLength = duration;

    return this;
  }

  Pixels* repeat() {
    if (progress > 0) {
      startTime = time;
    }

    return this;
  }

  Pixels* solid(uint32_t color) {
    return solid(color, currentSceneLength, 0);
  }
  Pixels* solid(uint32_t color, uint32_t duration) {
    return solid(color, duration, 0);
  }
  Pixels* solid(uint32_t color, uint32_t duration, uint16_t offset) {
    if (!progressing) {
      return this;
    }

    uint16_t localProgress = toLocal(progress);

    if (localProgress < offset || localProgress > offset + duration) {
      return this;
    }

    for (uint16_t i = 0; i < strip->numPixels(); i++) {
      strip->setPixelColor(i, color);
    }
    strip->show();

    return this;
  }

  Pixels* solidArc(uint32_t color, uint16_t start, uint16_t count) {
    return solidArc(color, start, count, currentSceneLength, 0);
  }
  Pixels* solidArc(uint32_t color, uint16_t start, uint16_t count, uint16_t duration) {
    return solidArc(color, start, count, duration, 0);
  }
  Pixels* solidArc(uint32_t color, uint16_t start, uint16_t count, uint16_t duration, uint16_t offset) {
    if (!progressing) {
      return this;
    }

    uint16_t localProgress = toLocal(progress);

    if (localProgress < offset || localProgress > offset + duration) {
      return this;
    }

    strip->fill(color, start, count);
    strip->show();

    return this;
  }

  Pixels* chaser(uint32_t color, uint16_t length) {
    return chaser(color, length, 1, currentSceneLength);
  }
  Pixels* chaser(uint32_t color, uint16_t length, float speed) {
    return chaser(color, length, speed, currentSceneLength);
  }
  Pixels* chaser(uint32_t color, uint16_t length, float speed, uint16_t duration) {
    if (!progressing) {
      return this;
    }

    uint16_t localProgress = toLocal(progress);
    uint16_t headPosition = (uint16_t)roundf(((float)localProgress * speed / duration) * strip->numPixels());

    for (uint16_t i = 0; i < length; i++) {
      strip->setPixelColor((headPosition + i) % strip->numPixels(), color);
    }
    strip->show();

    return this;
  }

  Pixels* linear(uint32_t targetColor) {
    return linear(targetColor, currentSceneLength);
  }
  Pixels* linear(uint32_t targetColor, uint32_t duration) {
    if (!progressing) {
      return this;
    }

    float localProgress = toLocal(progress) / (float)duration;
    for (uint16_t i = 0; i < strip->numPixels(); i++) {
      uint8_t originalG = *(pixelsCopy + (i * 3));
      uint8_t originalR = *(pixelsCopy + ((i * 3) + 1));
      uint8_t originalB = *(pixelsCopy + ((i * 3) + 2));
      uint8_t targetR = (targetColor & 0xff0000) >> 16;
      uint8_t targetG = (targetColor & 0x00ff00) >> 8;
      uint8_t targetB = targetColor & 0x0000ff;
      uint8_t currentR = originalR * (1 - localProgress) + targetR * localProgress;
      uint8_t currentG = originalG * (1 - localProgress) + targetG * localProgress;
      uint8_t currentB = originalB * (1 - localProgress) + targetB * localProgress;

      strip->setPixelColor(i, currentR, currentG, currentB);
    }
    strip->show();

    return this;
  }

  uint16_t toLocal(uint16_t progress) {
    return currentSceneLength - (uint16_t)(-progress);
  }

  void copyPixelsBuffer() {
    for (uint16_t i = 0; i < strip->numPixels() * 3; i++) {
      *(pixelsCopy + i) = *(strip->getPixels() + i);
    }
  }
};
