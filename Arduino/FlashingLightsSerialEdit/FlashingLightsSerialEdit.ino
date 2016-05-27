/* Write an array of firing frequencies and pulse widths and change them with a computer

It can also flip the array of flashing lights if desired
 
 For the below code, the following convention is used:
 Frequencies are in Hz
 Pulse widths are in milliseconds
 
 The LED array is indexed as follows:
  _________________________________
 |  3   4   3   4   1   2   1   2  |
 |  3   4   3   4   1   2   1   2  |
 |  3   4   3   4   1   2   1   2  |
 | Crickmore Lab / HMS Instr.      |
 |_________________________________|
 
 
 NOTE: The Buckpuck interpets a high control voltage as
 off and a low control voltage as on. It's stupid, but it
 probably made life easier for the guy who designed it. Oh
 well. Anyways, that explains why the digital write commands
 seem backwards.
 
 Stephen Thornquist March 22, 2015
 */
 
 // indicator is the pin corresponding to the indicator LED
const int indicator = 9;
const int numPins = 9;
int pin[numPins] = {13,13,13,13};
// Timescale says how many of our selected timescale are in a second
// so if we use microseconds, we should switch this to 1000000
const int timescale = 1000;
// startTime is how long until it should start
unsigned long startTime = 00000;
// duration is how long the Arduino should run its protocol.
unsigned long duration = 12000000; // 200 minutes
// endOfDays is when the protocol should end
unsigned long endOfDays = startTime + duration;
// Array of frequencies desired (in Hz)
// To tell a controller to be constantly off, input 0 for frequency
double freq[numPins] = {0,0,0,0};
// Array of pulse widths desired (in milliseconds)
// You don't need to worry about pulse width for constant
// LEDs.
double pulseWidth[numPins] = {1,1,1,1};

unsigned long lastOn[numPins]={ 0, 0, 0, 0};

// flipTime tells you how long to run a pulse pattern before switching
// to its inverse (in milliseconds).
unsigned long flipTime = 300000;
// the int flip keeps track of whether or not the state is flipped
int flipBit = 0;
int hasStarted = 0;
unsigned long blockOn = 0;
/* Load setup and initialize every pin as an output.
   Further, if the pin is set to be constant, initialize that.
*/   
void setup() {
  Serial.begin(9600);
  for(int k = 0; k < numPins; k++){
    pinMode(pin[k],OUTPUT);
    digitalWrite(pin[k],HIGH);
  }
    blockOn = 0;
    hasStarted = 0;
    pinMode(indicator,OUTPUT);
    Serial.write("Arduino on");
    /* digitalWrite(indicator,HIGH);
    delay(1000);
    digitalWrite(indicator,LOW);
    hasStarted = 1; */
}

// Run this loop ad electrical nauseum
void loop() {
  unsigned long currentTime = millis();
  // Check to see if it's time to start or not and then turn on the light
  if((currentTime > startTime) && hasStarted == 0) {
    digitalWrite(indicator,HIGH);
    delay(1000);
    digitalWrite(indicator,LOW);
    hasStarted = 1;
    Serial.write("Starting expt!");
  }
  
  if(currentTime > startTime && currentTime<endOfDays ) {
    // Step through each pin, indexed from 0
    for(int pinScan = 0; pinScan < numPins; pinScan++){
      // See if this pin is supposed to be constantly on
      if(freq[pinScan]!=0) {
        // Now make sure the pin is supposed to be on (not yet implemented, was buggy)
          // If it has been the pulse width since the last time the LED
          // was turned on, turn it off.
          if((currentTime-lastOn[pinScan]) > pulseWidth[pinScan]) {
            if(pinScan == 0) {
              digitalWrite(indicator,LOW);
            }
            digitalWrite(pin[pinScan], HIGH);
          }
           
          // If it's been timescale/frequency units of time since the LED
          // was last turned on, turn it on again. Then annotate 
          // having done so by updating the lastOn array.
          if((double(currentTime - lastOn[pinScan])*freq[pinScan])>=timescale){
            if(pinScan == 0) {
              digitalWrite(indicator,HIGH);
            } 
            digitalWrite(pin[pinScan], LOW);
            lastOn[pinScan] = currentTime; 
          }
      }
      else {
        digitalWrite(pin[pinScan],HIGH);
        if(pinScan == 0) {
          digitalWrite(indicator, LOW);
        }
      }
    }
  }
  if(currentTime > endOfDays) {
    digitalWrite(indicator, LOW);
    for(int pinScan = 0; pinScan < numPins; pinScan++) {
      digitalWrite(pin[pinScan], HIGH);
    }
  }
}


// For when something gets written to serial port
void serialEvent() {
  /* Freeze wells in the off state */
  for(int k = 0; k < numPins; k++){
    digitalWrite(pin[k],HIGH);
  }
  float inFreq = Serial.parseFloat();
  int inPW = Serial.parseInt();
  // Translate the well request into which pin to modify
  for(int i = 0; i < numPins; i++) {
      freq[i] = inFreq;
      pulseWidth[i] = inPW;
  }
}
