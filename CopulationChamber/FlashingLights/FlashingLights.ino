/* Write an array of firing frequencies and pulse widths

 
 For the below code, the following convention is used:
 Frequencies are in Hz
 Pulse widths are in milliseconds
 
 The LED array is indexed as follows:
  _________________________________
 |  1   2   1   2   3   4   3   4  |
 |  1   2   1   2   3   4   3   4  |
 |  1   2   1   2   3   4   3   4  |
 |_________________________________|
 
 
 NOTE: The Buckpuck interpets a high control voltage as
 of and a low control voltage as on. It's stupid, but it
 probably made life easier for the guy who designed it. Oh
 well. Anyways, that explains why the digital write commands
 seem backwards.
 
 Stephen Thornquist June 28 2014
 */

const int pinStart = 10;
const int pinEnd = 14;
const int numPins = pinEnd - pinStart;
// Timescale says how many of our selected timescale are in a second
// so if we use microseconds, we should switch this to 1000000
const int timescale = 1000;
// startTime is how long until it should start
unsigned long startTime = 0;
// duration is how long the Arduino should run its protocol.
unsigned long duration = 7200000;
// endOfDays is when the protocol should end
unsigned long endOfDays = startTime + duration;
// Array of frequencies desired (in Hz)
// To tell a controller to be constantly off, input 0 for frequency
double freq[numPins] = {0, 10, 0, 10};
// Array of pulse widths desired (in milliseconds)
// You don't need to worry about pulse width for constant
// LEDs.
int pulseWidth[numPins] = {0, 50, 0, 50};

unsigned long lastOn[numPins]={ 0, 0, 0, 0 };

/* Load setup and initialize every pin as an output.
   Further, if the pin is set to be constant, initialize that.
*/   
void setup() {
  Serial.begin(9600);
  for(int pin = pinStart; pin < pinEnd; pin++){
    pinMode(pin,OUTPUT);
    if(freq[pin-pinStart] == 0) {
      digitalWrite(pin,HIGH);
    }
  }
}

// Run this loop ad electrical nauseum
void loop() {
  unsigned long currentTime = millis();
  if(currentTime > startTime && currentTime<endOfDays) {
    // Step through each pin, indexed from 0
    for(int pinScan = 0; pinScan < numPins; pinScan++){
      // See if this pin is supposed to be constantly on
      if(freq[pinScan]!=0) {
        // Now make sure the pin is supposed to be on (not yet implemented, was buggy)
          // If it has been the pulse width since the last time the LED
          // was turned on, turn it off.
          if((currentTime-lastOn[pinScan]) > pulseWidth[pinScan]) {
            Serial.write('Off');  
            digitalWrite(pinScan+pinStart, HIGH);
          }
           
          // If it's been timescale/frequency units of time since the LED
          // was last turned on, turn it on again. Then annotate 
          // having done so by updating the lastOn array.
          if((double(currentTime - lastOn[pinScan])*freq[pinScan])>=timescale){
            digitalWrite(pinScan + pinStart, LOW);
            Serial.write('On');
            lastOn[pinScan] = currentTime; 
          }
      }
    }
  }
}

