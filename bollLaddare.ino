/*
   Fil: bollLaddare
   Namn: Svante Bengtsson
   Datum: 2023-11-09
   Beskrivning: roterar en stepper motor ett visst antal varv utifrån hur många ostbollar man vill ladda in i ett magasin. man kan ändra antal bollar med en knapp och antalet visas på en seven segment display. hur långt man har kommit i laddningsprocessen visas också på en neopixel ring
*/

//libraries for neopixel ring
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

//neopixel led pin
#define LED_PIN    2

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 24

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// for 8 digit seven segment display
#include "LedControl.h"
LedControl lc=LedControl(12,13,10,1);
//delay between updates for seven segment
unsigned long delaytime=250;

//declare variables for the motor pins
int motorUppNedPin1 = 7; // Blue - 28BYJ48 pin 1
int motorUppNedPin2 = 6; // Pink - 28BYJ48 pin 2
int motorUppNedPin3 = 5; // Yellow - 28BYJ48 pin 3
int motorUppNedPin4 = 4; // Orange - 28BYJ48 pin 4

//stepperbutton
int knappPin1 = 3;

//ballbutton
int knappPin2 = 8;

int motorSpeed = 1200; //variable to set stepper speed
int lookup[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};

// variable for the number of balls
int balls = 1;
// variable for the variable for how much rotation per ball
int rotation = 100 * balls;


void setup() {
  //declare the motor pins as outputs
  pinMode(motorUppNedPin1, OUTPUT);
  pinMode(motorUppNedPin2, OUTPUT);
  pinMode(motorUppNedPin3, OUTPUT);
  pinMode(motorUppNedPin4, OUTPUT);

  //declare button pins as input
  pinMode(knappPin1, INPUT);
  pinMode(knappPin2, INPUT);

  //start serial communication frequency 9600
  Serial.begin(9600);

  //neopixel setup
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  //for seven segment starts display
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);
}

void loop() {
  rotation = 100 * balls;

  // When the button is pressed, the stepper motor takes as many steps as needed (rotation) to load the correct number of balls, while simultaneously updating the NeoPixel ring constantly to show the progress.
  if (digitalRead(knappPin1)  == HIGH ){
    for(int i=0; i<rotation; i++){
      uppat();
      colorUpdate(i);
    }
    delay(1000);
    colorWipe(strip.Color(0,   0,   0), 10);
  }

  lc.setDigit(0,0,balls,false);
  Serial.println(digitalRead(knappPin2));
  
  // This goes through the number of balls and jumps back to 1 if you try to go higher than nine
  if (digitalRead(knappPin2)  == HIGH){
    if(balls<9){
      balls = balls + 1;
    }
    else{
      balls = 1;
    }
  delay(500);
  }
}

// Funktion that moves the steppermotor one step backwards
void nerat(){
  for (int i = 0; i < 8; i++)
  {
    setOutputUppNed(i);
    delayMicroseconds(motorSpeed);
    Serial.print("Ned ");
  }
}



// Funktion that moves the steppermotor one step forwards
void uppat(){
  for (int i = 7; i >= 0; i--)
  {
    setOutputUppNed(i);
    delayMicroseconds(motorSpeed);
    Serial.print("Upp ");
  }
}

//helpfuntion for uppat and nerat
void setOutputUppNed(int out)
{
  digitalWrite(motorUppNedPin1, bitRead(lookup[out], 0));
  digitalWrite(motorUppNedPin2, bitRead(lookup[out], 1));
  digitalWrite(motorUppNedPin3, bitRead(lookup[out], 2));
  digitalWrite(motorUppNedPin4, bitRead(lookup[out], 3));
}

//sets all the pixels on the neopixel to the same color one after the other
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

//sets a specific pixel to red based on mapped data from how far along in the rotation the stepper motor is
void colorUpdate(int rot) {
  int i = map(rot, rotation, 0, 24, 0);
  strip.setPixelColor(i, strip.Color(255, 0, 0));
  strip.show();
}
