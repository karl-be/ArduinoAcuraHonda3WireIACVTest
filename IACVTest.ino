// Arduino Based PWM Control for 3 Wire PWM based IACV Idle Air Control Valve
// Useful for triggering and testing 3 wire IACVs where 2 coils are brought to ground in an either static ground + PWM or PWM + inverse PWM system. 
//
//
//   WIRING FOR 3 Wire IACV Honda / Acura 2001 CL / TL 2001 J32 and J35 possibly others. 
//       For inverse PWM hook up another mosfet on P11 and enable IACVInverseOutputEnabled in the configuration replacing the ground connection of the IACV
//           with the inverse PWM signal.
//
//                               MOSFET                   IACV
//                           Gate  Drain Source       P1   P2   P3
//                            |      |      |         |     |    |
//    Arduino                 |      |      |         |     |    |
//        ->P10 - - - - - - - |      |- - -^|^- - - -^|^- -^|^- -|
//                                          |         |     |
//        ->GND - - - - - - - - - - - - - - |- - - - -|     |
//                                          |               |
//    12V Source                            |               |
//                                          ^               |
//       ->12V - - - - - - - - - - - - - - - - - - - - - - - 
//                                          ^          
//       ->GND - - - - - - - - - - - - - - -|
//
// Notes: 
//            |       
//      A - -^|^- -   Indicates that the wire from L to R does *not* connect. EG Mosfet DRAIN directly connects to IACV P3 and does not touch the other wires. 
//            |
//
//      If using a benchtop power supply ensure that the grounds are all equal between the Arduino power and the 12V source. Using a battery would work as well. 
//
//      During my testing I was able to get smooth articulation of the valve down to ~100 HZ although jitter was noticable. Below 75 HZ was unusable as it caused the valve
//          to retract between pulses. 200+ HZ is optomial and very smooth running was found around 500 - 1000HZ. 
//      
//      This driver is designed to control coils that have an internal layout as follows:  
//  
//                     
//                 P2 ( VCC )
//                     |
//                - - - - - - - 
//                /           /
//               COIL       COIL
//                /           /
//                |           | 
//                |           |
//                P1          P3
// Configuration: 
//
// IACVOutputPIN Used for primary (non inverted) 
const int IACVOutputPinA = 10;
// IACVOutputPIN Used for inverted driving (only when IACVInverseOutputEnabled).
const int IACVOutputPinB = 11;
// Enable to output on PinB
const bool IACVInverseOutputEnabled = false;

// The Base Frequency That The Duty Cycle Will Pulse Through (Usual Values Are ~75 to 500)
const int IACVBaseFrequencyHZ = 1000;

// The minimum and maximum duty cycles that we will sweep through
const float minimumDutyCycleInPercent = 5;
const float maximumDutyCycleInPercent = 100;
// The time for a single sweep to occur
const int sweepTimeMS = 2000;


// Internal Values Do Not Configure.
float microSecondsPerFullWidth = -1;
float currentSweepPercent = 0;
bool isSweepAdding = true;

void setup() {
  microSecondsPerFullWidth = (float)(1000000 /  IACVBaseFrequencyHZ);
  
  pinMode(IACVOutputPinA, OUTPUT);
  pinMode(IACVOutputPinB, OUTPUT);
}

void loop() {
  // Calculate Duty Cycle, Using maximum and minimum cycle percents
  float currentPercent = (((currentSweepPercent / (float)100) * (maximumDutyCycleInPercent - minimumDutyCycleInPercent)) + minimumDutyCycleInPercent) / 100.0;

  // Calculate the delay in us
  int microsecondsHigh = (int)(microSecondsPerFullWidth * currentPercent);
  int microsecondsWait = microSecondsPerFullWidth - microsecondsHigh;

  // Calculate our sweep percent offset based on our full width time 
  float sweepOffset = ((float)microSecondsPerFullWidth / (float)1000) / (float)sweepTimeMS * (float)100;
  if(!isSweepAdding) {
    // Ensure we are 'adding' in the right direction
    sweepOffset = sweepOffset * -1;
  }

  // Add to the percent, clamp to 100 and invert if invalid.
  currentSweepPercent = currentSweepPercent + sweepOffset;
  if(currentSweepPercent > 100) {
    currentSweepPercent = 100;
    isSweepAdding = !isSweepAdding;
  } 
  if(currentSweepPercent < 0) {
    currentSweepPercent = 0;
    isSweepAdding = !isSweepAdding;
  }

  // Do the timing and write to the pins.
  digitalWrite(IACVOutputPinA, HIGH);
  if(IACVInverseOutputEnabled) {
    digitalWrite(IACVOutputPinB, LOW);
  }
  del(microsecondsHigh); 
  digitalWrite(IACVOutputPinA, LOW); 
  if(IACVInverseOutputEnabled) {
    digitalWrite(IACVOutputPinB, HIGH);
  }
  del(microsecondsWait);
}

// Calling Delay Microseconds with a value larger than 16k results in no delay occuring (overflow issues) 
void del(long value) {
  if (value > 16383) {
    delay(value / 1000);
  } else {
    delayMicroseconds(value);
  }
}
