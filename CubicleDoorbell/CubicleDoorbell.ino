/*
 * Blinkerton Labs  2018
 * 
 * Adjusted to use the Blinkm in parallel with a 
 * sense resisor on the switch.
 * Will measure the source current for the BlinkM
 * and the switch.  
*/
const byte pinAnalogInput   = 0;  //  Analog input for the current sense resistor
const byte pinLED           = 3;  //  Output to the P-Channel MOSFET

//  The BlinkM normally draws betwen 20mA and 50mA.  With a 100 ohm resistor in 
//  parallel, it will cause the current draw to jump 50mA when the switch is pressed.
//  There is a 1 ohm sense resistor in the ground path.
//  1 ohm * 50mA = 50mV Set the threshold greather than 50mV
const int analogThreshold = 7;  //  5V/1023 = 4.89mV    70mV/4.89mV = 14.31

const long debounceDelay = 20;

int rollingMeanAnalogRead;


void setup() 
{
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(pinLED,  OUTPUT);

  //  Using a P-Channel MOSFET, so it's active low
  digitalWrite(pinLED, HIGH);

  Serial.begin(9600);
}


void loop() 
{


  //  Read the analog sensor and calculate the mean
  rollingMeanAnalogRead = clacRollingMean(analogRead(pinAnalogInput), false);
  
  //  Print out the value for the serial plotter
  Serial.println(rollingMeanAnalogRead);
  
  
  if (rollingMeanAnalogRead > analogThreshold)
  {
    Serial.println(rollingMeanAnalogRead);
    //  Wait a bit, then check it again
    pulseOutputLED(pinLED);

    //  Reset the rolling average
    int holderValue = clacRollingMean(0, true);
  }
}


//  Function to pulse the output LED when the button is pressed
void pulseOutputLED(const byte pinOutput)
{
  const int   numberOfPulses = 3;
  const long  delayBetweenPules = 100;

  //  Number of cycles to ramp up to 100, then back down
  const long  delayBetweenRampcycles = 2;
  const int   rampStartValue = 0;
  const int   rampEndValue = 255;

  //  Outer loop for the pulses
  for (int pulseIndex = 0; pulseIndex < numberOfPulses; pulseIndex++)
  {

    //digitalWrite(pinLED, HIGH);

    //  Inner loop for the ramp cycles
    for (int cycleIndex = rampEndValue; cycleIndex >= rampStartValue; cycleIndex--)
    {
      analogWrite(pinOutput, cycleIndex);
      delay(delayBetweenRampcycles);
    }
    
    for (int cycleIndex = rampStartValue; cycleIndex <= rampEndValue; cycleIndex++)
    {
      analogWrite(pinOutput, cycleIndex);
      delay(delayBetweenRampcycles);
    }

    //digitalWrite(pinLED, HIGH);
    
    delay(delayBetweenPules);

  }

  //  Make sure to turn off the LEDs
  digitalWrite(pinLED, HIGH);
  
}


//  Function to blink the output LEDs
void flashLEDs(const byte pin)
{
  const int   numberOfFlashes = 1;
  const long  delayBetweenFlahses = 150;

  const int   pulsesPerFlash = 2;
  const long  delayOfEachPulse = 30;

  digitalWrite(pin, HIGH);

  for (int flashIndex = 0; flashIndex < numberOfFlashes; flashIndex++)
  {

    for (int pulseIndex = 0; pulseIndex < pulsesPerFlash; pulseIndex++)
    {
      digitalWrite(pin, LOW);
      delay(delayOfEachPulse);
      digitalWrite(pin, HIGH);
      delay(delayOfEachPulse);
    }

    delay(delayBetweenFlahses);
  }

  digitalWrite(pin, HIGH);
  
}


//  Function to calculate a rolling median value
int clacRollingMean(int inputValueInt, bool resetCount)
{
  const int ROLLING_WINDOW_SIZE = 10;

  static int valueArray[ROLLING_WINDOW_SIZE];
  static int windowIndex = 0;

  //  Reset the rolling average to zero
  if (true == resetCount)
  {
    for (windowIndex = 0; windowIndex < ROLLING_WINDOW_SIZE; windowIndex++)
    {
      valueArray[windowIndex] = 0;
    }

    return 0;
  }

  //  Add the input value to the array
  valueArray[windowIndex] = inputValueInt;

  //  Holder for the summation
  //  Since this will be analog data, 4,294,967,295 / 1,023 = 4.2 million for the max window size
  unsigned long summationValue = 0;
  unsigned long meanAsLong = 0;

  //  Calculate the mean
  for (int meanIndex = 0; meanIndex < ROLLING_WINDOW_SIZE; meanIndex++)
  {
    summationValue += (unsigned long)(valueArray[meanIndex]);
  }

  meanAsLong = summationValue / ((unsigned long)ROLLING_WINDOW_SIZE);


  //  Increment or reset the window index
  if (ROLLING_WINDOW_SIZE == windowIndex)
  {
      windowIndex = 0;
  }
  else
  {
    windowIndex++;
  }

  return (int)meanAsLong;
}






