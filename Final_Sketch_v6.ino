#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <FastLED.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();

const int R_Indicator = 2;  // Tactile Button
const int L_Indicator = 3;

const int Front_LED = 4;     // Data pin of LED strip
const int Back_LED = 5;

const int Crash_Alarm = 6;

#define NUM_LEDS 12  // Define the number of LEDs
#define COLOR_ORDER GRB

CRGB leds_front[NUM_LEDS]; // Define the front LED array
CRGB leds_back[NUM_LEDS]; // Define the back LED array

// // Prone to off by one errors
// const int Accel_LED_Start = 4;  // Adjust as needed
// const int Accel_LED_End = 7;    // Adjust as needed

const int desired_beeps = 6;

int R_Indicator_State = 0;
int L_Indicator_State = 0;

bool Buzzer_State = false;
int beeps = 0;

const float Threshold_Z = 4.0;
// const float Threshold_Y = 10.0;
// const float Threshold_X = 10.0;
const float Threshold_Crash = 16.0;

int consecutiveCount = 1;
const int consecutiveThreshold = 2;

bool conditionMet = false;

// Set Sample Frequentie //
// #define SAMPLE_FREQ 100  // sample frequency [Hz]

// const float sample_period = 1000/(SAMPLE_FREQ);   // sample period [millis seconds]

// Breadboard Configuration
// Terminal 3/5 -> Right Button
// Terminal 11 -> Front LED
// Terminal 15 -> Buzzer

void setup() {

  pinMode(R_Indicator, INPUT_PULLUP);
  pinMode(L_Indicator, INPUT_PULLUP);

  Wire.begin();

  pinMode(Front_LED, OUTPUT);
  pinMode(Back_LED, OUTPUT);

  pinMode(Crash_Alarm, OUTPUT);
  noTone(Crash_Alarm); // Initialize the buzzer as OFF

  // Initialize the front LED strip
  // .setCorrection() might account for color change. Either TypicalLEDStrip (standard) or TypicalPixelString (for ARGB).
  FastLED.addLeds<WS2812B, Front_LED, GRB>(leds_front, NUM_LEDS).setCorrection(TypicalPixelString);
  FastLED.addLeds<WS2812B, Back_LED, GRB>(leds_back, NUM_LEDS).setCorrection(TypicalPixelString);

  // Power failsafe
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);

  FastLED.clear();
  FastLED.show();
  
  Serial.begin(9600);
  // Serial.println("hi");

  accel.begin();
  //    if(!accel.begin())
  //  {
  //     Serial.println("No valid sensor found");
  //     while(1);
  // }
}

void loop() {

  // // Clear LED
  // FastLED.clear();
  // FastLED.show();

  // Define Values for ADXL345 Accelerometer

  sensors_event_t event; 
  accel.getEvent(&event);

  Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("   ");
  Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("   ");
  Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("   ");
  Serial.println("m/s^2 ");
  delay(100);

  float Accel_X = event.acceleration.x;
  float Accel_Y = event.acceleration.y;
  float Accel_Z = event.acceleration.z;


  // Indicator System

  R_Indicator_State = digitalRead(R_Indicator);
  L_Indicator_State = digitalRead(L_Indicator);

  // // For Debugging
  // Serial.print("R_Indicator State: ");
  // Serial.println(R_Indicator_State);

  // // For Debugging
  // Serial.print("L_Indicator State: ");
  // Serial.println(L_Indicator_State);

  if (R_Indicator_State == LOW) {

    // For Debugging
    Serial.println("Right Indicator button is pressed.");

    while (R_Indicator_State == LOW) {
    // Making final 4 LEDs on both strips blink yellow

      for (int i = 4; i <= 7; i++) {
        leds_front[i] = CRGB::White;
        leds_back[i] = CRGB::Red;
        
      }

    FastLED.show();
    
      for (int i = 8; i <= 11; i++) {
      leds_front[i] = CRGB::Yellow;
      leds_back[i] = CRGB::Yellow;    // Might have to change orientation
      FastLED.show();
      delay(100); // // _ms pause
    }

    delay(100);
    
    // Turn off LEDs
      for (int i = 8; i <= 11; i++) {
      leds_front[i] = CRGB::Black;
      leds_back[i] = CRGB::Black;
    }
    
    FastLED.show();
    delay(100);

    R_Indicator_State = digitalRead(R_Indicator);

    }
  }

  else if (L_Indicator_State == LOW) {

    // For Debugging
    Serial.println("Left Indicator button is pressed.");

    while (L_Indicator_State == LOW) {
    // Making final 4 LEDs on both strips blink yellow

        for (int i = 4; i <= 7; i++) {

      leds_front[i] = CRGB::White;
      leds_back[i] = CRGB::Red;

        }

      FastLED.show();
    
      for (int i = 3; i >= 0; i--) {
      leds_front[i] = CRGB::Yellow;
      leds_back[i] = CRGB::Yellow;
      FastLED.show();
      delay(100); // _ms pause
    }

    delay(100);
    
    // Turn off LEDs
      for (int i = 3; i >= 0; i--) {
      leds_front[i] = CRGB::Black;
      leds_back[i] = CRGB::Black;
    }
    
    FastLED.show();
    delay(100);

    L_Indicator_State = digitalRead(L_Indicator);

    }
  }
  
  // else {
  //   // Turn off all LEDs

  //   for (int i = 0; (i <= 3) || (i >= 8 && i <= 11); i++) {
  //     leds_front[i] = CRGB::Black;
  //     leds_back[i] = CRGB::Black;
  //   }

  //   // for (int i = 4; i <= 7; i++) {
  //   //   leds_front[i] = CRGB::Red;
  //   //   leds_back[i] = CRGB::Red;

  //   FastLED.show();
  
  else {
    // Turn off all LEDs except the middle 4 LEDs, which will be red

    for (int i = 4; i <= 7; i++) {
            leds_front[i] = CRGB::White;
            leds_back[i] = CRGB::Red;

        }
    
      FastLED.show();
    
    }


  // Brake Light System

  if (abs(Accel_Z) > Threshold_Z && abs(Accel_Z) < Threshold_Crash) {

    // For Debugging
    Serial.println("Regular Acceleration is detected.");

    // Acceleration - Make the middle 4 LEDs green
    for (int i = 4; i <= 7; i++) {
      leds_front[i] = CRGB::White;
      leds_back[i] = CRGB::Green;
    }

    FastLED.show();

  }
  
  else if (Accel_Z < -Threshold_Z && abs(Accel_Z) < Threshold_Crash) {

    // For Debugging
    Serial.println("Regular Deceleration is detected.");

    // Deceleration - Make the middle 4 LEDs red
    for (int i = 4; i <= 7; i++) {
      leds_front[i] = CRGB::White;
      leds_back[i] = CRGB::Green;
    }

    FastLED.show();
  }
  
  else if (abs(Accel_Z) < Threshold_Z){

    // For Debugging
    Serial.println("No notable Acceleration or Deceleration is detected.");

    // No significant acceleration or deceleration
    for (int i = 4; i <= 7; i++) {
      leds_front[i] = CRGB::White;
      leds_back[i] = CRGB::Red;
    }
  
    FastLED.show();

  }

    // Crash Detection System

  // if (abs(Accel_X) > Threshold_Crash || abs(Accel_Y) > Threshold_Crash) {
  if (abs(Accel_Z) > Threshold_Crash) {

    // For Debugging
    Serial.println("Crash is detected.");
    Serial.println(Accel_Z);
    // Serial.println(beeps);
    // Serial.println(Buzzer_State);

      for (int i = 4; i <= 7; i++) {
    leds_front[i] = CRGB::Green;
    leds_back[i] = CRGB::Green;

      }

    FastLED.show();
    delay(100);
    FastLED.clear();
    delay(100);

    int beeps = 0;

    if (!Buzzer_State) {

      Buzzer_State = true;
      beeps = 0; // Reset the beep counter
    }

    else if (Buzzer_State) {

      Buzzer_State = true;

    }
  
  while (Buzzer_State && beeps < 6) {

    // Added line not verified - goal is for LED to go red while buzzer rings
    //   for (int i = 4; i <= 7; i++) {
    // leds_front[i] = CRGB::Red;
    // leds_back[i] = CRGB::Red;

    // FastLED.show();
    //   }
    
    Serial.println("Crash alarm goes off!");
    
    tone(Crash_Alarm, 255);
    delay(100);
    noTone(Crash_Alarm);
    delay(100);
    beeps++;

    digitalRead(Buzzer_State);
    }
  
  bool Buzzer_State = false;

  // Serial.println(beeps);
  // Serial.println(Buzzer_State);
  }

  else {

    noTone(Crash_Alarm);

  }

  int beeps = 0;
  bool Buzzer_State = false;

  // Serial.println(beeps);
  // Serial.println(Buzzer_State);
  
}


//   // Crash Detection System

  
//   if (Accel_Z > Threshold_Crash) {
//     consecutiveCount++;
//     conditionMet = true;
    
//     if (consecutiveCount == consecutiveThreshold && conditionMet) {
//       // Perform your desired action when the condition is detected three times consecutively
//       Serial.println("Crash condition detected three times consecutively. ...");

//       Serial.println("Crash lights go off...");
//       for (int i = 4; i <= 7; i++) {
//         leds_front[i] = CRGB::Green;
//         leds_back[i] = CRGB::Green;

//       }

//       FastLED.show();
//       delay(100);
//       FastLED.clear();
//       delay(100);

//     int beeps = 0;
  
//     if (!Buzzer_State) {

//       Buzzer_State = true;
//       beeps = 0; // Reset the beep counter
//     }

//     else if (Buzzer_State) {

//       Buzzer_State = true;

//     }
  
//     while (Buzzer_State && beeps < 6) {

//     // Added line not verified - goal is for LED to go red while buzzer rings
//     //   for (int i = 4; i <= 7; i++) {
//     // leds_front[i] = CRGB::Red;
//     // leds_back[i] = CRGB::Red;

//     // FastLED.show();
//     //   }
    
//     Serial.println("Crash alarm goes off!");
    
//     tone(Crash_Alarm, 255);
//     delay(100);
//     noTone(Crash_Alarm);
//     delay(100);
//     beeps++;

//     digitalRead(Buzzer_State);
//     }
  
//   bool Buzzer_State = false;

//           }

//   else {
//     // Reset the consecutive count if the condition is not detected
//     consecutiveCount = 0;
//     }
//   // Serial.println(beeps);
//   // Serial.println(Buzzer_State);

//        // For Debugging
//     // Serial.println("Crash is detected.");
//     // Serial.println(beeps);
//     // Serial.println(Buzzer_State);

//   // if (abs(Accel_X) > Threshold_Crash || abs(Accel_Y) > Threshold_Crash) {

//   //   // For Debugging
//   //   Serial.println("Crash is detected.");
//   //   // Serial.println(beeps);
//   //   // Serial.println(Buzzer_State);

//   //   FastLED.show();
//   //   delay(100);
//   //   FastLED.clear();
//   //   delay(100);

//   //   int beeps = 0;

//   //   if (!Buzzer_State) {

//   //     Buzzer_State = true;
//   //     beeps = 0; // Reset the beep counter
//   //   }

//   //   else if (Buzzer_State) {

//   //     Buzzer_State = true;

//   //   }
  
//   // while (Buzzer_State && beeps < 6) {

//   //   // Added line not verified - goal is for LED to go red while buzzer rings
//   //   //   for (int i = 4; i <= 7; i++) {
//   //   // leds_front[i] = CRGB::Red;
//   //   // leds_back[i] = CRGB::Red;

//   //   // FastLED.show();
//   //   //   }
    
//   //   Serial.println("Crash alarm goes off!");
    
//   //   tone(Crash_Alarm, 255);
//   //   delay(100);
//   //   noTone(Crash_Alarm);
//   //   delay(100);
//   //   beeps++;

//   //   digitalRead(Buzzer_State);
//   //   }
  
//   // bool Buzzer_State = false;

//   // // Serial.println(beeps);
//   // // Serial.println(Buzzer_State);
//   // }

//   // else {

//   //   noTone(Crash_Alarm);

//   // }

//   // int beeps = 0;
//   // bool Buzzer_State = false;

//   // // Serial.println(beeps);
//   // // Serial.println(Buzzer_State);
//       // Example command: You can replace this with your own action
//   //     digitalWrite(LED_BUILTIN, HIGH); // Turn on the built-in LED
//   //     conditionMet = true; // Set a flag to avoid executing the command repeatedly
//   //   }
//   // }
  
//   //   else {
//   //   consecutiveCount = 0; // Reset the consecutive count
//   //   conditionMet = false; // Reset the flag
//   //   digitalWrite(LED_BUILTIN, LOW); // Turn off the built-in LED
//   // }

//   // else if (abs(Accel_Z) > Threshold_Crash) {
//   //   Serial.print("Single Crash Detected.");
//   //   consectiveCount++;



//     // while (abs(Accel_Z) > Threshold_Crash) {
//     //   consecutiveCount = consecutiveCount + 1;
          
//     //     if (consecutiveCount >= consecutiveThreshold) {

  
//   }
// }