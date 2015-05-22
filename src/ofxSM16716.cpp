//  
// ofxSM16716.cpp
// library for controlling an LED grid
// grid is comprised of LED strips driven by the ofxSM16716 chipset
// code by Ross Parker http://limitcycle.net/about


#include "ofxSM16716.h"
#include "wiringPi.h"
  
// constructor
ofxSM16716::ofxSM16716() {

}


bool ofxSM16716::init(int num_pix){
  
  int setupResult = wiringPiSetup();

  if(setupResult > -1)
  {
    cout << "wiringPi Setup successful";
  } else {
    cout << "wiringPi setup failed! try running as sudo";
    return false;
  }

  
  pixels.resize(num_pix);

  pinMode(DATA_PIN, OUTPUT);
  digitalWrite(DATA_PIN, LOW);
  // setup Arduino for clock pin
  pinMode(CLOCK_PIN, OUTPUT);
  digitalWrite(CLOCK_PIN, LOW);
 
  // turn all pixels off 
  clear();

  if(setupResult > -1) return true;
}

// destructor
ofxSM16716::~ofxSM16716() {
  // empty
}

// set a pixel in the RGB array in the strip layout using a single 32-bit int for the color
// masks with 00FFFFFF since color is 24-bit value, thus this sets uppermost byte to zeros
void ofxSM16716::set_pixel(int index, uint32_t color) {
  pixels[index] = color & 0x00FFFFFF;
}

// set a pixel in the RBG array in the strip layout using separate ints for R, G, and B
// creates a 32-bit uint from RGB values, then calls set_pixel
// bytes are red, green, blue from left to right
// note: some implementations have this backwards
void ofxSM16716::set_pixel_rgb(int index, uint32_t r, uint32_t g, uint32_t b) {
  
  if(r > 255) r = 255;
  if(g > 255) g = 255;
  if(b > 255) b = 255;

  set_pixel(index, b | (g << 8) | (r << 16));
}



// toggle clock pin high then low; "pushes" each bit one bit down the strip
void ofxSM16716::toggle_clock() {
  digitalWrite(CLOCK_PIN, HIGH);
  digitalWrite(CLOCK_PIN, LOW);
}

// write a single pixel of given index in strip layout from RGB array to a given strip
void ofxSM16716::write_pixel(int i) {
  // mask has a 1 in bit 25, zero everywhere else
  const uint32_t MASK = ((uint32_t)(1) << 24);
  // puts the bit 1 in front of the RGB value
  // so p (in binary) looks like 1 + red (8 bits) + green (8 bits) + blue (8 bits)
  // basically, we are putting a bit 1 between each set of 24 RGB bits
  uint32_t p = pixels[i] | MASK;
  // send bits from most to least significant, we are sending 25 bits total
  int j = 25;
  while (j--) {
    // send the bit in the 25th position (from the least significant end)
    // write HIGH if bit is 1, LOW if bit is 0
    digitalWrite(DATA_PIN, (p & MASK) ? HIGH : LOW);
    // toggle clock to "push" each bit one down the strip
    toggle_clock();
    // shift p to the left by one so the next bit we want is in the 25th position
    p <<= 1;
  }
}

// // write pixels of given index in strip layout from RGB array to all strips
// // this has (theoretical) advantage that a single toggle_clock() pushes data down all strips at once
// void ofxSM16716::write_pixel_all_strips(uint8_t pos) {
//   // mask has a 1 in bit 25, zero everywhere else
//   const uint32_t MASK = ((uint32_t)(1) << 24);
//   // here p is an array indexed by strip
//   // first we initialize p for each strip, as above
//   uint32_t p[NUM_STRIPS];
//   for (int i = 0; i < NUM_STRIPS; i++) {
//     p[i] = pixels[i][pos] | MASK;
//   }
//   // then we send appropriate data to all strips as above
//   // toggling clock once we've sent a bit to all the strips
//   int j = 25;
//   while (j--) {
//     for (int i = 0; i < NUM_STRIPS; i++) {
//       digitalWrite(DATA_PIN + i, (p[i] & MASK) ? HIGH : LOW);
//       p[i] <<= 1;
//     }
//     toggle_clock();
//   }
// }

// write a single blank pixel to all strips
void ofxSM16716::write_blank_pixel() {
  // write a string of 25 zeros to all strips
  // again, as above, we are writing 25 bits, since there is one bit between each set of 24 RGB bits
  int j = 200;
  while (j--) {
    
    digitalWrite(DATA_PIN, 0);
    
    toggle_clock();
  }
}
  
// push all pixels from array to grid, one at a time; uses write_pixel
// it appears that we write 24 bits of data to the first LED using write_pixel, 
// plus one extra bit of padding; then the next 24 bits get sent to the second LED
// in the strip, etc.
// so although we are sending the bits for each pixel from most to least significant,
// we are writing the LEDs from the near end to the far end
void ofxSM16716::show() {
  // write a single zero to each strip
  
  digitalWrite(DATA_PIN, LOW);

  // toggle the clock 50 times
  for (int i = 0; i < 50; i++) {
    toggle_clock();
  }
  // cycle through the strips one at a time

  for (int i = 0; i < pixels.size(); ++i) {
    write_pixel(i);
  }
  
  // write a single blank pixel
  write_blank_pixel();
  delay(1);
}


// // push all pixels to grid doing strips concurrently
// // similar to show(), except we are using write_pixel_all_strips
// // so we do not also have to cycle through the strips
// void ofxSM16716::show_all_strips() {
  
//   digitalWrite(DATA_PIN, LOW);

//   for (int i = 0; i < 50; i++) {
//     toggle_clock();
//   }
//   for (int i = 0; i < pixels.size(); ++i) {
//     write_pixel_all_strips(i);
//   }
//   write_blank_pixel();
//   delay(1);
// }


    
// clears LED grid by setting all the pixels in the array to zero
// note: does not call show(), so does not actually display a blank grid
void ofxSM16716::clear() {
  
  for (int j = 0; j< pixels.size(); j++) {
    set_pixel_rgb(j, 0, 0, 0);
  }

}


