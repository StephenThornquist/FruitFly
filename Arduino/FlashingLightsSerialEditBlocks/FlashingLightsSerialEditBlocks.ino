/* Write an array of firing frequencies and pulse widths and change them with a computer

It can also flip the array of flashing lights if desired
 
Blocks of stimuli (e.g. flashing lights for 15 seconds out of every 5 minutes)
 
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
 
 Stephen Thornquist June 28 2014
 */
 
  // indicator is the pin corresponding to the indicator LED
const int indicator = 9;
const int numPins = 9;

// This line is to map the schematized well number above onto the right entry in the pin array
// wellMap's nth entry is the index of Well #n in the array pin. It basically
// exists to make up for my sloppy wiring job. When I make a PCB I can fix this
// to make it better.

int wellMap[numPins+1] = {9,5,4,7,6,3,8,2,1,0}; 
int pin[numPins] = {3,4,5,6,7,10,11,12,13};
// Timescale says how many of our selected timescale are in a second
// so if we use microseconds, we should switch this to 1000000
const int timescale = 1000;
// startTime is how long until it should start
unsigned long startTime = 00000;
// duration is how long the Arduino should run its protocol.
unsigned long duration = 120000000; // 2000 minutes
// endOfDays is when the protocol should end
unsigned long endOfDays = startTime + duration;
// Array of frequencies desired (in Hz)
// To tell a controller to be constantly off, input 0 for frequency
double freq[numPins] = {0,0,0,0,0,0,0,0,0};
// Array of pulse widths desired (in milliseconds)
// You don't need to worry about pulse width for constant
// LEDs.
double pulseWidth[numPins] = {90,80,70,60,50,40,30,20,10};

unsigned long lastOn[numPins]={ 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int blockDur[numPins] = {0,0,0,0,0,0,0,0,0};
int blockTime[numPins] = {0,0,0,0,0,0,0,0,0};
int hasStarted = 0;
unsigned long blockOn[numPins] = {0,0,0,0,0,0,0,0,0};

/* Load setup and initialize every pin as an output.
   Further, if the pin is set to be constant, initialize that.
*/   
void setup() {
  Serial.begin(9600);
  for(int k = 0; k < numPins; k++){
    pinMode(pin[k],OUTPUT);
    digitalWrite(pin[k],HIGH);
  }
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
  // The big loop checking whether each light should be on.
  if(currentTime > startTime && currentTime<endOfDays) {
    // Step through each pin, indexed from 0
    for(int pinScan = 0; pinScan < numPins; pinScan++){
      // See if this pin is supposed to be constantly off
      if(freq[pinScan]!=0) {
        // Now make sure the pin is supposed to be on
          // If it has been the pulse width since the last time the LED
          // was turned on, turn it off. Likewise, if it's been more than blockTime
          // since the block started, turn the light off (but only if blockDur isn't set to 0).
          if((currentTime-lastOn[pinScan]) > pulseWidth[pinScan] || ((((currentTime-startTime) - blockOn[pinScan]) > blockDur[pinScan]) && blockDur[pinScan] != 0)) {
            if(pinScan == 0) {
              digitalWrite(indicator,LOW);
            }
            digitalWrite(pin[pinScan], HIGH);
          }
           
          // If it's been timescale/frequency units of time since the LED
          // was last turned on, turn it on again. Then annotate 
          // having done so by updating the lastOn array.
          if((double(currentTime - lastOn[pinScan])*freq[pinScan])>=timescale && ((currentTime-startTime) - blockOn[pinScan]) < blockDur[pinScan]){
            if(pinScan == 0) {
              digitalWrite(indicator,HIGH);
            } 
            digitalWrite(pin[pinScan], LOW);
            lastOn[pinScan] = currentTime; 
          }
          // Update the blockOn
          if (((currentTime-startTime) - blockOn[pinScan]) > blockTime[pinScan]) {
            blockOn[pinScan] = currentTime;
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
  int inWell = Serial.parseInt();
  float inFreq = Serial.parseFloat();
  int inPW = Serial.parseInt();
  int bd = Serial.parseInt();
  int br = Serial.parseInt();
  int pinInd = wellMap[inWell];
  freq[pinInd] = inFreq;
  pulseWidth[pinInd] = inPW;
  blockDur[pinInd] = bd;
  blockTime[pinInd] = br;
}
