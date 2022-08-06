/**
 * MIT License
 *
 * Copyright (c) 2022 Avi G
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// This is simplest sketch for inferencing. This sketch is optimized for debugging. This is not optimized for lowest power.

#include <NDP.h>
#include <NDP_utils.h>
#include <Arduino.h>
#include "TinyML_init.h"
#include "NDP_init.h"
#include "SAMD21_init.h"
#include "SAMD21_lowpower.h"

#define DO_NOT_WAIT_FOR_SERIAL_PORT 0                       // For battery powered autonomus usage, use this option
#define WAIT_FOR_SERIAL_PORT 1                              // For debugging purposes use this option, program will wait until serial port is turned on, Arduino IDE Tools --> Serial Monitor
#define NDP_MICROPHONE 0                                    // Model type (Audio or Sensor) and this slection MUST match
#define NDP_SENSOR 1                                        // Model type (Audio or Sensor) and this slection MUST match

int match = 0;                                              // This variable indicates which class has matched

void NDP_service(){                                     // This is user's sandbox to do things when a classifier is detected
  switch (match){                                           // match is slighly modified in sequence. When no classifier is asserted after posterior filter then 0 is returned. Classifier0 is retrurnd as 1 and so on
    case 0:
      break;                                                // No classifier is asserted after posterior filter
    case 1:                                                 // Go classifier in go/stop model or classifier0 or first classifier in Edge impulse list asserted
      digitalWrite(LED_GREEN, HIGH);                        // Turning on green light as an assertion to one of the classifier asserted
      Serial.println("Classifier 0 detected");    
      delay(1000);                                          // Delay is added for LED light persistence
      digitalWrite(LED_GREEN, LOW);                         // Green LED is turned off 
      break;
    case 2:                                                 // Stop classifier in go/stop model or classifier1 or second classifier in Edge impulse list asserted
      digitalWrite(LED_RED, HIGH);                          // Turning on Red light as an assertion to another classifier asserted
      Serial.println("Classifier 1 detected");   
      delay(1000);                                          // Delay is added for LED light persistence                                         
      digitalWrite(LED_RED, LOW);                           // Red LED is turned off 
      break;
    default:
      break;
  }
}

void setup(void) {
  //SAMD21_init(DO_NOT_WAIT_FOR_SERIAL_PORT);               // Setting up SAMD21 (0) will not wait for serial port
  SAMD21_init(WAIT_FOR_SERIAL_PORT);                        // Setting up SAMD21 (1) will wait and RGB LED will be Red
  NDP_init("ei_model.bin",NDP_MICROPHONE);                  // Setting up NDP, Stuck Blue LED means model is not read 
  Serial.println(" \n Setup done"); 
  delay(2000);                                              // Flushing out buffer
  NDP.poll();                                               // Flushing out buffer
}

void loop() {
    delay(22);                                             // The model is set for 24ms inference refresh period. 2ms are allowed for house keeping time
    match = NDP.poll();                                    // This API fetches last assertion of the match, in this sketch polling is done without interrupt evaluation
    //Serial.print("Match :");                             // Continuous matches can be printed but screen will get busy
    //Serial.println(match);
    NDP_service();                                         // Service call based on match
}
