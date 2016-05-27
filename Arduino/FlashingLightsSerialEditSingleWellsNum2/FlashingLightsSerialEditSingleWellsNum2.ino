/* Write an array of firing frequencies and pulse widths and change them with a computer
 
 For the below code, the following convention is used:
 Frequencies are in Hz
 Pulse widths are in milliseconds
 
 The LED array is indexed as follows:
 -----------------
||  1     2    3  ||
||                ||
||  4     5    6  ||
||                ||
||  7     8    9  ||
||----------------||
 
 
 NOTE: The Buckpuck interpets a high control voltage as
 off and a low control voltage as on. It's stupid, but it
 probably made life easier for the guy who designed it. Oh
 well. Anyways, that explains why the digital write commands
 seem backwards.
 
 Stephen Thornquist March 22, 2015
 */
 
 // indicator is the pin corresponding to the indicator LED
const int indicator = 23;
const int numPins = 12;

// This line is to map the schematized well number above onto the right entry in the pin array
// wellMap's nth entry is the index of Well #n in the array pin. It basically
// exists to make up for my sloppy wiring job. When I make a PCB I can fix this
// to make it better.
// our input looks like: {0,1,2,3,4,5,6,7,8,9,10,11,12}
//int wellMap[numPins+1] = {0,1,2,3,4,5 ,6,7,8,9,10,11,12};
//int wellMap[numPins+1] = {0 ,1,2 ,3 ,4,5,6,7,8,9,10,11,12};
int wellMap[numPins+1] = {12,4,10,11,5,2,8,9,3,0,6 ,7 ,1};
int pin[numPins] = {24,25,26,27,28,29,30,31,32,33,34,35};
// Timescale says how many of our selected timescale are in a second
// so if we use microseconds, we should switch this to 1000000
const int timescale = 1000;
// startTime is how long until it should start
unsigned long startTime = 00000;
// duration is how long the Arduino should run its protocol.
unsigned long duration = 12000000; // 200 minutes
// endOfDays is when the protocol should end
unsigned long endOfDays = startTime + duration;

// This block is for single wells with ongoing frequenices
// Array of frequencies desired (in Hz)
// To tell a controller to be constantly off, input 0 for frequency
double freq[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
// Array of pulse widths desired (in milliseconds)
// You don't need to worry about pulse width for constant
// LEDs.
double pulseWidth[numPins] = {90,80,70,60,50,40,30,20,10,10,10,10};

// This next block is for paired pulses:
// an array indicating at what time you gave the signal to count down from the delay
unsigned long pinInit[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};

// Delay until start (minutes)
double del[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
// Gap between pulses (seconds)
double gap[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
// Array of pulse widths desired (in milliseconds)
// You don't need to worry about pulse width for constant
// LEDs.
// First pulse
double p1[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
// Second pulse
double p2[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};

// This section is for block stimuli
int blockDur[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
int blockTime[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long blockOn[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};

// Last on info for light flashing

unsigned long lastOn[numPins]={ 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0 };

int hasStarted = 0;

String mode = "";
char charBuffer;
/* Load setup and initialize every pin as an output.
   Further, if the pin is set to be constant, initialize that.
*/   
void setup() {
  Serial.begin(9600);
  for(int k = 0; k < numPins; k++){
    pinMode(pin[k],OUTPUT);
    digitalWrite(pin[k],HIGH);
  }
  // give the mode information time to be written to the serial port
  delay(1000);
  while(Serial.available()){
    char buffer = Serial.read();
    mode.concat(buffer);
  }
  if(mode == "") {
    Serial.write("ERROR: No mode selected!");
    delay(100000);
  }
  hasStarted = 0;
  pinMode(indicator,OUTPUT);
  Serial.print("Arduino on");
  Serial.print(mode);
}

// Run this loop ad electrical nauseum
void loop() {
  unsigned long currentTime = millis();
  // Check to see if it's time to start or not and then turn on the light
  if((currentTime > startTime) && hasStarted == 0) {
    hasStarted = 1;
    Serial.write("Starting expt!");
  }
  // single well with consistent stim mode
  if(mode=="singleWells") {
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
        }
      }
    }
  }
  // paired pulse mode
  if(mode=="pairedPulse"){
    if(currentTime > startTime && currentTime<endOfDays ) {
    // Step through each pin, indexed from 0
      for(int pinScan = 0; pinScan < numPins; pinScan++){
        // See if time delay has passed since the signal was given
        if((currentTime - pinInit[pinScan]) > del[pinScan]) {
            // Are we in the regime of p1? If so, turn on the light
            if((currentTime - pinInit[pinScan] - del[pinScan]) < p1[pinScan]) {
              digitalWrite(pin[pinScan],LOW);
            }
            // Are we in the gap? If so, light off!
            else if((currentTime - pinInit[pinScan] - del[pinScan]-p1[pinScan]) < gap[pinScan]) {
             digitalWrite(pin[pinScan],HIGH); 
            }
            // Are we in p2? Turn it back on
            else if((currentTime - pinInit[pinScan] - del[pinScan]-p1[pinScan]-gap[pinScan]) < p2[pinScan]) {
             digitalWrite(pin[pinScan],LOW); 
            }
            else {
              digitalWrite(pin[pinScan],HIGH);
            }
        }
        else {
          digitalWrite(pin[pinScan],HIGH);
        }
      }
    }
  }
  
  // blocks of stim mode
  if (mode == "blocks") {
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
  if (mode == "singleWells") {
    int inWell = Serial.parseInt();
    float inFreq = Serial.parseFloat();
    int inPW = Serial.parseInt();
    // Translate the well request into which pin to modify
    int pinInd = wellMap[inWell];
    freq[pinInd] = inFreq;
    pulseWidth[pinInd] = inPW;
  }
  if (mode=="pairedPulse") {
    unsigned long timeNow = millis();
    int inWell = Serial.parseInt();
    double inDelay = Serial.parseFloat();
    float inP1 = Serial.parseFloat();
    double inGap = Serial.parseFloat();
    int inP2 = Serial.parseInt();
    // Translate the well request into which pin to modify
    int pinInd = wellMap[inWell];
    pinInit[pinInd] = timeNow;
    del[pinInd] = inDelay*60000;
    gap[pinInd] = inGap*1000;
    p1[pinInd] = inP1;
    p2[pinInd] = inP2;
  }
  if (mode == "blocks") {
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
  while(Serial.available()) {
    Serial.read();
  }
}

