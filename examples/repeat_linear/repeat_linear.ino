#include <Pixels.h>

// the datastructure that represents a strand of pixels
Pixels p;

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