#define currentPin    A0                   // Current sense pin
#define voltagePin    A1                   // Voltage sense pin
#define RPMPin        3                    // Digital pin where RPM sensor output is attached 
#define loopCount     8                    // ADC loop to get averages of the values
#define loopFrequency 10                   // Hz
#define loopPeriod    1000 / loopFrequency // Loop takes place each n ms
#define numPoles      14                   // Number of motor magnet poles

// Factors to convert measured voltage (ADC) into measure voltage and current
// Power module uses 0m5ohm resistor and INA138 with 750k Rl 
const float currMultFactor = 5.0/76.8; //5.0/(1024.0 * 0.075)
// Voltage divider with 1488ohm and 13660ohm resistor
//const float voltMultFactor = 75740.0/1523712.0; //5.0*15148.0/(1024.0*1488.0)
// From measurement, 14,78V -> 302
const float voltMultFactor = 14.78/302.0;

volatile unsigned long interruptCount;

unsigned long beginTime = 0;

void countInterrupts() {
    interruptCount++;
}

void setup() {
  pinMode(RPMPin, INPUT); //INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RPMPin), countInterrupts, CHANGE);
  Serial.begin(115200); 
  Serial.println("Begin");
  
}

void loop() {
  if((millis() - beginTime) >= loopPeriod){
    beginTime = millis();
    // Measure RPM
    noInterrupts();
    unsigned long measuredInterrupts = interruptCount;
    interruptCount = 0;
    interrupts() ;
    // loopFrequency used as multiplication because value is under 1s
    float rpm = (measuredInterrupts * 60 * loopFrequency) / numPoles;
    int currentVal = 0; 
    int voltageVal = 0;
    for(int i = 0; i < loopCount; i++){
      currentVal += analogRead(currentPin);
      voltageVal += analogRead(voltagePin);
    }
    float currentValFloat = (currentVal >> 3) * currMultFactor;
    float voltageValFloat = (voltageVal >> 3) * voltMultFactor;
    Serial.print(beginTime);
    Serial.print(",");
    Serial.print(voltageValFloat);
    Serial.print(",");
    Serial.print(currentValFloat);
    Serial.print(",");
    Serial.println(rpm);
  }         
}
