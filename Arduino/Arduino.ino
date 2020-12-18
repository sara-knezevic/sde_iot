#define RELAY_LIGHT_ROOM1 13
#define RELAY_LIGHT_ROOM2 12

#define EN_ROOM1 7
#define DC_PIN1_ROOM1 6
#define DC_PIN2_ROOM1 5

#define EN_ROOM2 2
#define DC_PIN1_ROOM2 3
#define DC_PIN2_ROOM2 4

#define TEMP_ROOM1 A0
#define TEMP_ROOM2 A1

#define LIGHT_SENSOR_ROOM1 A2
#define LIGHT_SENSOR_ROOM2 A3

byte reportArray[20];

uint8_t device, room, state;
int pickedDevice, pickedRoom, pickedState;

int lightSensorRoom1, lightSensorRoom2;
unsigned long previousMillis = 0;

int lightsRoom1 = 0;         // light initial state
int ventillationOnRoom1 = 0; // ventillation on/off intial state
bool freshAirIsOnRoom1 = true; // fresh air initial state

int lightsRoom2 = 0;         // light initial state
int ventillationOnRoom2 = 0; // ventillation initial state
bool freshAirIsOnRoom2 = true; // fresh air initial state

void setup() {
  Serial.begin(9600);
  
  pinMode(RELAY_LIGHT_ROOM1, OUTPUT);
  pinMode(RELAY_LIGHT_ROOM2, OUTPUT);
  
  pinMode(EN_ROOM1, OUTPUT);
  pinMode(DC_PIN1_ROOM1, OUTPUT);
  pinMode(DC_PIN2_ROOM1, OUTPUT);

  pinMode(EN_ROOM2, OUTPUT);
  pinMode(DC_PIN1_ROOM2, OUTPUT);
  pinMode(DC_PIN2_ROOM2, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (Serial.available() > 0) {
    getCommands();
  }
  
  // 1 minute interval
  if (currentMillis % 60000 == 0) {
    noInterrupts();
    // Ventillation system Room 1
    // Autonomous loop
    // Check if ventillation is on
    if (ventillationOnRoom1) {
      // Flip fresh air/air suction state
      freshAirIsOnRoom1 = !freshAirIsOnRoom1;
      if (freshAirIsOnRoom1) {
        // Fresh air! (+ direction)
        airSuction(DC_PIN1_ROOM1, DC_PIN2_ROOM1, false);
        freshAir(DC_PIN1_ROOM1, DC_PIN2_ROOM1, true);
      } else {
        // Air suction! (- direction)
        freshAir(DC_PIN1_ROOM1, DC_PIN2_ROOM1, false);
        airSuction(DC_PIN1_ROOM1, DC_PIN2_ROOM1, true);
      }
    }

    // Ventillation system Room 2
    // Autonomous loop
    // Check if ventillation is on
    if (ventillationOnRoom2) {
      // Flip fresh air/air suction state
      freshAirIsOnRoom2 = !freshAirIsOnRoom2;
      if (freshAirIsOnRoom2) {
        // Fresh air! (+ direction)
        airSuction(DC_PIN1_ROOM2, DC_PIN2_ROOM2, false);
        freshAir(DC_PIN1_ROOM2, DC_PIN2_ROOM2, true);
      } else {
        // Air suction! (- direction)
        freshAir(DC_PIN1_ROOM2, DC_PIN2_ROOM2, false);
        airSuction(DC_PIN1_ROOM2, DC_PIN2_ROOM2, true);
      }
    }

    // Send sensor and device state data.
    sendData();
    interrupts();
  }
}

void getCommands() {
   byte rcv[3];
   // Read 3 bytes sent over Serial from PC
   Serial.readBytes(rcv, 3);
   device = rcv[0]; // Ventillation 1; Lights 2; Report 0;
   room = rcv[1];   // Room 1; Room 2;
   state = rcv[2];  // State 0; State 1;

   pickedDevice = (int)device;
   pickedRoom = (int)room;
   pickedState = (int)state;

   if (pickedDevice == 1) { // ventillation
      ventillationState(pickedRoom, pickedState);
   } else if (pickedDevice == 2) { // lights
      lightState(pickedRoom, pickedState);
   } else if (pickedDevice == 0) { // report request
      sendData();
   }
}

void sendData() {

  // Get sensor data converted through their corresponding function
  float T1 = voltageToTemperature(analogRead(TEMP_ROOM1));
  float T2 = voltageToTemperature(analogRead(TEMP_ROOM2));
  float L1 = convertIlluminance(analogRead(LIGHT_SENSOR_ROOM1));
  float L2 = convertIlluminance(analogRead(LIGHT_SENSOR_ROOM2));

  // initialize address pointers
  uint32_t *addrT1, *addrL1, *addrT2, *addrL2;

  // assign pointers to float values addresses
  addrT1 = (uint32_t *)&T1;
  addrL1 = (uint32_t *)&L1;
  addrT2 = (uint32_t *)&T2;
  addrL2 = (uint32_t *)&L2;

  // store sensor floats into array
  reportArray[0] = (uint8_t)(*addrT1 & 0xFF);
  reportArray[1] = (uint8_t)(*addrT1 >> 8) & 0xFF;
  reportArray[2] = (uint8_t)(*addrT1 >> 16) & 0xFF;
  reportArray[3] = (uint8_t)(*addrT1 >> 24) & 0xFF;
  
  reportArray[4] = (uint8_t)(*addrL1 & 0xFF);
  reportArray[5] = (uint8_t)(*addrL1 >> 8) & 0xFF;
  reportArray[6] = (uint8_t)(*addrL1 >> 16) & 0xFF;
  reportArray[7] = (uint8_t)(*addrL1 >> 24) & 0xFF;
  
  reportArray[8] = (uint8_t)(*addrT2 & 0xFF);
  reportArray[9] = (uint8_t)(*addrT2 >> 8) & 0xFF;
  reportArray[10] = (uint8_t)(*addrT2 >> 16) & 0xFF;
  reportArray[11] = (uint8_t)(*addrT2 >> 24) & 0xFF;
  
  reportArray[12] = (uint8_t)(*addrL2 & 0xFF);
  reportArray[13] = (uint8_t)(*addrL2 >> 8) & 0xFF;
  reportArray[14] = (uint8_t)(*addrL2 >> 16) & 0xFF;
  reportArray[15] = (uint8_t)(*addrL2 >> 24) & 0xFF;

  // store device states
  reportArray[16] = (uint8_t)ventillationOnRoom1;
  reportArray[17] = (uint8_t)lightsRoom1;
  reportArray[18] = (uint8_t)ventillationOnRoom2;
  reportArray[19] = (uint8_t)lightsRoom2;
  
  Serial.write(reportArray, 20);
  Serial.flush();
  delay(1000);
}

// Ventillation system control
void ventillationState(int room, int state) {
  // Room 1
  if (room == 1) {
      // Assign given state to the room
      ventillationOnRoom1 = state;
      if (state == 1) { 
        // Ventillation ON
        // Turn on voltage to motor driver
        analogWrite(EN_ROOM1, 255);
        if (freshAirIsOnRoom1) {
          airSuction(DC_PIN1_ROOM1, DC_PIN2_ROOM1, false);
          freshAir(DC_PIN1_ROOM1, DC_PIN2_ROOM1, true);
        } else {
          freshAir(DC_PIN1_ROOM1, DC_PIN2_ROOM1, false);
          airSuction(DC_PIN1_ROOM1, DC_PIN2_ROOM1, true);
        }
      } else {          
        // Ventillation OFF
        // Turn off voltage for motor driver
        analogWrite(EN_ROOM1, 0);
        airSuction(DC_PIN1_ROOM1, DC_PIN2_ROOM1, false);
        freshAir(DC_PIN1_ROOM1, DC_PIN2_ROOM1, false);
      }
   // Room 2
   } else if (room == 2) {
      // Assign given state to the room
      ventillationOnRoom2 = state;
      if (state == 1) {
        // Ventillation ON
        // Turn on voltage to motor driver
        analogWrite(EN_ROOM2, 255);
        if (freshAirIsOnRoom2) {
          airSuction(DC_PIN1_ROOM2, DC_PIN2_ROOM2, false);
          freshAir(DC_PIN1_ROOM2, DC_PIN2_ROOM2, true);
        } else {
          freshAir(DC_PIN1_ROOM2, DC_PIN2_ROOM2, false);
          airSuction(DC_PIN1_ROOM2, DC_PIN2_ROOM2, true);
        }
      } else {
        // Ventillation OFF
        // Turn off voltage to motor driver
        analogWrite(EN_ROOM2, 0);
        airSuction(DC_PIN1_ROOM2, DC_PIN2_ROOM2, false);
        freshAir(DC_PIN1_ROOM2, DC_PIN2_ROOM2, false);
      } 
   }
}

// Light system control
void lightState(int room, int state) {
  // Room 1
  if (room == 1) {
      // Assign state 
      lightsRoom1 = state;
      // Toggle light
      if (state == 1) {
        digitalWrite(RELAY_LIGHT_ROOM1, HIGH);
      } else {
        digitalWrite(RELAY_LIGHT_ROOM1, LOW);
      }
   // Room 2
   } else if (room == 2) {
      // Assign state
      lightsRoom2 = state;
      // Toggle light
      if (state == 1) {
        digitalWrite(RELAY_LIGHT_ROOM2, HIGH);
      } else {
        digitalWrite(RELAY_LIGHT_ROOM2, LOW);
      }
   }
}

// Toggle fresh air
void freshAir(int pin1, int pin2, bool on) {
  if (on) {
    // spin in + direction
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);
  } else {
    // stop spinning
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
  }
}

// Toggle air suction
void airSuction(int pin1, int pin2, bool on) {
  if (on) {
    // spin in - direction
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);
  } else {
    // stop spinning
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
  }
}

// Voltage to Temperature conversion
float voltageToTemperature(int voltage) {
  float millivoltage = (voltage/1023.) * 5000.; // input voltage in millivoltage units
  float temperature = millivoltage / 10.;       // 10mv = 1 degree
  return temperature;
}

float convertIlluminance(int lightVoltage) {
  float MAX_ADC_READING = 1023.0;                                            // ADC resolution
  float ADC_REF = 5.0;                                                       // Arduino/Sensor input voltage
  float resistorVoltage = (float)lightVoltage / (MAX_ADC_READING * ADC_REF); // calculate resistor (5kOhm) voltage
  float ldrVoltage = ADC_REF - resistorVoltage;                              // calculate LDR voltage (max - resistor)
  float ldrResistance = ldrVoltage /resistorVoltage * 5.0;                   // calculate LDR resistance (voltage divider)
  float lux = 28704689.27 * pow(ldrResistance, -2.415);                      // put LDR resistance in function (derived from Excel)
  return lux;                                                                // and calculate lux
}
