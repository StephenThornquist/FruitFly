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
 of and a low control voltage as on. It's stupid, but it
 probably made life easier for the guy who designed it. Oh
 well. Anyways, that explains why the digital write commands
 seem backwards.
 
 Stephen Thornquist June 28 2014
 */
 
 // indicator is the pin corresponding to the indicator LED
const int indicator = 9;
const int pinStart = 10;
const int pinEnd = 14;
const int numPins = pinEnd - pinStart;
// Timescale says how many of our selected timescale are in a second
// so if we use microseconds, we should switch this to 1000000
const int timescale = 1000;
// startTime is how long until it should start
unsigned long startTime = 0;
// duration is how long the Arduino should run its protocol.
unsigned long duration = 1200000; // 20 minutes
// endOfDays is when the protocol should end
unsigned long endOfDays = startTime + duration;
// Array of frequencies desired (in Hz)
// To tell a controller to be constantly off, input 0 for frequency
double f = 5;
double w = 5;
double freq[numPins] = {f, 0, f, 0};

// Array of pulse widths desired (in milliseconds)
// You don't need to worry about pulse width for constant
// LEDs.
double pulseWidth[numPins] = { w, 0, w, 0};

unsigned long lastOn[numPins]={ 0, 0, 0, 0 };

// flipTime tells you how long to run a pulse pattern before switching
// to its inverse (in milliseconds).
unsigned long flipTime = 6000;
// the int flip keeps track of whether or not the state is flipped
int hasStarted = 0;
/* Load setup and initialize every pin as an output.
   Further, if the pin is set to be constant, initialize that.
*/
void setup() {
  Serial.begin(9600);
  for(int pin = pinStart; pin < pinEnd; pin++){
    pinMode(pin,OUTPUT);
    digitalWrite(pin,HIGH);
  }
    hasStarted = 0;
    pinMode(indicator,OUTPUT);
    Serial.println("Arduino on");
    /* digitalWrite(indicator,HIGH);
    delay(1000);
    digitalWrite(indicator,LOW);
    hasStarted = 1; */
}

// Run this loop ad electrical nauseum
void loop() {
  unsigned long currentTime = millis();
  // Wait for serial input to tell you it's time to start
  /*
    if(Serial.available() > 0 && hasStarted == 0) {
      char input;
      while(SerialEvent()) {  
        char input = Serial.read();
          switch(input) {
            case('s'):
              digitalWrite(indicator,HIGH);
              delay(1000);
              digitalWrite(indicator,LOW);
              startTime = currentTime;
              hasStarted = 1;
              endOfDays = currentTime + duration;
              Serial.println(endOfDays);
              Serial.println("Starting expt!");
              break;
           // Change the frequency
           case('f'): { 
              Serial.println("Changing f");
              String fString;
              int inputNum = Serial.read();
              Serial.println(inputNum);
              while(isdigit(inputNum)) {
                fString = fString+String(inputNum);
                inputNum = Serial.read();
              }
              f = (double)(fString.toInt());
              Serial.println(fString);
              break;
           }
           // Change the pulse width
           case('w'): {
             Serial.println("Changing w");
             String wString;
             int inputNum = Serial.read();
             Serial.println(inputNum);
             while(isdigit(inputNum)) {
               wString = wString+String(inputNum);
               inputNum = Serial.read();
             }
             w = (double)(wString.toInt());
             Serial.println(wString);
             break;
           }
          }
      }
      double freq[numPins] = {f, 0, f, 0};
      double pulseWidth[numPins] = {w,0,w,0};
    } */
  // check to see if it's time to flip
  if( Serial.available() > 0 && hasStarted == 1 ) {
    // turn off all the lights first so I don't cause a seizure
    for(int i = 0; i < numPins; i++) {
      digitalWrite(pinStart+i, HIGH);
    }
    Serial.println("flip!");
    String readByte = Serial.readString();
    flipWells(readByte);
  }

  if(hasStarted == 1 && currentTime<endOfDays) {
    // Step through each pin, indexed from 0
    for(int pinScan = 0; pinScan < numPins; pinScan++){
      // See if this pin is supposed to be constantly on
      if(freq[pinScan]!=0) {
        // Now make sure the pin is supposed to be on (not yet implemented, was buggy)
          // If it has been the pulse width since the last time the LED
          // was turned on, turn it off.
          if((currentTime-lastOn[pinScan]) > pulseWidth[pinScan]) {
            digitalWrite(pinScan+pinStart, HIGH);
          }
           
          // If it's been timescale/frequency units of time since the LED
          // was last turned on, turn it on again. Then annotate 
          // having done so by updating the lastOn array.
          if((double(currentTime - lastOn[pinScan])*freq[pinScan])>=timescale){
            digitalWrite(pinScan + pinStart, LOW);
            lastOn[pinScan] = currentTime; 
          }
      }
    }
  }
  if(currentTime > endOfDays) {
    digitalWrite(indicator,HIGH);
    delay(10000000);
  }
}

/* flip function flips the frequency and pulse width arrays on a chamber-by-chamber basis
*/
void flip(int wellNum) {
  double dummyFreq[2] = {};
  double dummyPulse[2] = {};
  dummyFreq[0] = freq[2*wellNum];
  dummyFreq[1] = freq[2*wellNum + 1];
  dummyPulse[0] = pulseWidth[2*wellNum];
  dummyPulse[1] = pulseWidth[2*wellNum + 1];
  freq[2*wellNum] = dummyFreq[1];
  freq[2*wellNum + 1] = dummyFreq[0];
  pulseWidth[2*wellNum] = dummyPulse[1];
  pulseWidth[2*wellNum + 1] = dummyPulse[0];
  digitalWrite(2*wellNum+pinStart,HIGH);
  digitalWrite(2*wellNum+1+pinStart,HIGH);
}

/* Flip several wells */
void flipWells(String wells) {
  for( int i = 0; i < wells.length(); i++ ){
    Serial.println( (int) wells[i] - '0');
    flip( (int) wells[i] - '0');
  }
}
// Function for when something gets written to the serial port
void serialEvent() {  
  if(Serial.available() > 0 && hasStarted == 0) {
   while(Serial.available()){ 
     char input = Serial.read();
        switch(input) {
            case('s'):
              digitalWrite(indicator,HIGH);
              delay(1000);
              digitalWrite(indicator,LOW);
           //   startTime = currentTime;
              hasStarted = 1;
           //   endOfDays = currentTime + duration;
              Serial.println(endOfDays);
              Serial.println("Starting expt!");
              break;
           // Change the frequency
           case('f'): { 
              Serial.println("Changing f");
              String fString;
              char next = Serial.read();
              Serial.println(next);
              while(next != '~') {
                fString = fString + next;
                next = Serial.read();
                Serial.println(next);
              }
              f = (double)(fString.toInt());
              Serial.println(fString);
              break;
           }
           // Change the pulse width
           case('w'): {
             Serial.println("Changing w");
             String wString;
             char next = Serial.read();
             Serial.println(next);
             while(next != '~') {
               wString = wString+next;
               next=Serial.read();
               Serial.println(next);
             }
             w = (double)(wString.toInt());
             Serial.println(wString);
             break;
           }
          }
         }
      double freq[numPins] = {f, 0, f, 0};
      double pulseWidth[numPins] = {w,0,w,0};
    }
}
