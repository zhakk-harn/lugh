# Lugh

This is a toy library to help writing timelines for ws2812b LEDs.

It rellies on the [Adafruit_Neopixel](https://github.com/adafruit/Adafruit_NeoPixel) library so make sure you have it installed.

## Example

The main idea is that you've got a timeline made of scenes which contains animations.

First you'll need to create an instance thet will represent your strip of LEDs.

```ino
Pixels p;
```

Then, in your `setup()` you'll need to initialize it using `Pixel.start()`

```ino
p.start(24, 6); // I've got 24 LEDs and it's on the pin 6
```

And finally in your `loop()` you'll have to start your timeline with `Pixel.run()` which is going to return the pointer to the Pixel struct, like most functions in this lib, allowing you to chain your timeline nicely.

```ino
p.run() 							// start of the timeline
	->scene(1000)				// first scene lasts 1000ms
	->solid(0x110000);	// we display a dim red on the whole strip
```

Your timeline is made up of scenes. No two scenes can run at the same time, so if we put it all together we could have something like this

```ino
#include <Lugh.h>

Pixels p; // the datastructure that represents a strand of pixels

void setup() {
  Serial.begin(9600);
  Serial.println();

  // I specify that I've got 24 pixels on D6
  p.start(24, 6);
}

void loop() {
  p.run()
    ->scene(100)        // my first scene lasts 100ms
    ->solid(0x001100)   // it displays the color green rgb(0, 17, 0) on the whole strip
    ->scene(1000)       // then the second scene lasts 1000ms
    ->linear(0x110000)  // during which we transition to red rgb(17,0,0) on the whole strip
    ->scene(1000)
    ->linear(0x000011)  // then to blue
    ->scene(1000)
    ->linear(0x001100)  // then to green
    ->repeat();         // then we loop the loop the timeline (can be omitted)
}
```

You could put multiple animations per scene, affecting diferent ranges of pixels on your strip, as of 0.1.0 it's only a proof of concept and not very usable.

<details>
	<summary>In case you are a c/c++ dev...</summary>

    I'm aware of the code quality of this project and I'd be very much interested in all the grivances that will be generated upon reading the code.

    Please keep in mind that I've tried to use only features from C as an exercise (even though I'm rellying on a c++ library).

</details>

## Reference

### Attributes

```ino
Adafruit_Neopixel* strip
```

The underlying Adafruit_Neopixel object we are taking advantage of to not recode everything. You've got access to it in case you want to use the gamma correction, the `colorHSV()` or any other thing you could find in their [reference](https://adafruit.github.io/Adafruit_NeoPixel/html/class_adafruit___neo_pixel.html).

You should only use it to read data or use the functions in the "Static Public Member Fnuctions" section of their reference like the one to create colors, any writing to the LEDs should be done through Lugh, so it has a chance to keep track.

```ino
// [...]

p.run()
	->scene(1000)
	->solid(p.strip->ColorHSV(0, 128, 128)); // a solid fill of the strip with some red at mid saturation and mid value
// [...]
```

### Functions

#### start

```ino
  void start(uint16_t numPixels[, uint8_t pin, uint32_t start])
```

The function to initialize the structure .

`numPixels` : the amount of LEDs on your strip.

`pin` : the pin used to connect the strip.

`start` : a starting time, which is only usefull if you are using an alternative timekeeping method. By default it will use `millis()`.
``

#### run

```ino
  Pixels* run([uint32_t timeRef])
```

The function responsible to rewind a bunch of things in order to make everything ready to have another iteration of the `loop()` function.

`timeRef` : the current time for this iteration. You only have to specify this if you are using an alternative timekeeping method. By default it will use `millis()`.

#### scene

```ino
Pixels* scene(uint32_t duration)
```

The function to create a scoped(in time) chunk of your timeline.

`duration` : the length of time the scene will be displayed for.

#### repeat

```ino
void repeat()
```

A function to place at the end of your timeline to make it loop back whenever it ends

#### solid

```ino
Pixels* solid(uint32_t color[, uint32_t duration, uint16_t offset])
```

Displays a solid color on all of your pixels.

`color` : the color to show

`duration` : the length of time during which to show this color

`offset` : the length of time between the start of the scene and the start of this "animation"

#### chaser

```ino
Pixels* chaser(uint32_t color, uint16_t length[, float speed, uint16_t duration])
```

An animation of a certain amount of pixels going from the start to the end of the strip, looping back to the start.

`color` : the color of the animation

`length` : the length of the chain of moving light

`speed` : the speed modifier, greater than 1 will make it go faster, lower than 1 will slow it down. Less than 0 won't make it go backward.

#### linear

```ino
Pixels* linear(uint32_t targetColor[, uint32_t duration])
```

An animation wihch linearly goes from the last color displayed on the previous scene to a solid color.

`targetColor` : the color of the strip once the transition is done.

`duration` : the amount of time it will take to go from the state of the last frame of the last scene to the `targetColor`. By default it's the duration of the scene.
