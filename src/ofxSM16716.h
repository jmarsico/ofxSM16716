//
// ofxSM16716.h
// library for controlling an LED grid
// grid is comprised of LED strips driven by the ofxSM16716 chipset
// modified code by Ross Parker http://limitcycle.net/about
//

#ifndef ofxSM16716_h
#define ofxSM16716_h

// allow to access wiringPi functions
#include "wiringPi.h"
#include "ofMain.h"

// data pin for the first strip; each additional strip is one pin higher
// although you can theoretically daisy chain as many strips as you want,
// we found that the signal degraded when strips got longer than 50 LEDs.
// solution was to connect data pin for each strip of 50 to a separate 
// digial out pin on the Arduino; for other chipsets, this might not be necessary
#define DATA_PIN (12)

// clock pin; used for all strips
// clock pins from all strips are connected to this pin
#define CLOCK_PIN (14)

// these define the physical strips used to construct the grid
// we will refer to this as the "strip layout"
// the pixel array always corresponds the strip layout
// number of strips



// ofxSM16716 class
class ofxSM16716 {
  public:
    // constructor and destructor
    ofxSM16716();
    ~ofxSM16716();
    
    // this set of methods sets a pixel in the strip layout
    // set a pixel in the RGB array using a single 32-bit uint for the color; does not write to grid
    void set_pixel(uint8_t index, uint32_t color);

    bool init(int num_pix);
    
    // set a pixel in the RBG array using separate uints for R, G, and B; does not write to grid
    void set_pixel_rgb(uint8_t index, uint32_t r, uint32_t g, uint32_t b);
   
    // this set of methods writes pixels from the pixel array to the LED strips
    // toggle clock pin high then low; "pushes" each bit one bit down the strip
    void toggle_clock();
    
    // write a single pixel of given index in strip layout from RGB array to a given strip
    void write_pixel(uint8_t i);
    
    // // write pixels of given index in strip layout from RGB array to all strips
    // // this has (theoretical) advantage that a single toggle_clock() pushes data down all strips at once
    // void write_pixel_all_strips(uint8_t pos);
    
    // write a blank pixel to all strips
    void write_blank_pixel();
      
    // push all pixels from array to grid, one at a time; uses write_pixel
    void show();
    
    // push all pixels from array to grid doing strips concurrently; uses write_pixel_all_strips
    // void show_all_strips();
    
    // clear the entire LED grid; does not call show()
    void clear();
 
  private:
    // array containing the RGB grid
    // this represents the 
    // uint32_t pixels[LIGHT_COUNT];
    vector <uint32_t> pixels;

 };
 
 #endif

