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

int lightsRoom1 = 0;
int ventillationOnRoom1 = 0; // ventillation state
bool freshAirIsOnRoom1 = true;

int lightsRoom2 = 0;
int ventillationOnRoom2 = 0; // ventillation state
bool freshAirIsOnRoom2 = true;

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
    if (ventillationOnRoom1) {
      freshAirIsOnRoom1 = !freshAirIsOnRoom1;
      if (freshAirIsOnRoom1) {
        airSuction(DC_PIN1_ROOM1, DC_PIN2_ROOM1, false);
        freshAir(DC_PIN1_ROOM1, DC_PIN2_ROOM1, true);
      } else {
        freshAir(DC_PIN1_ROOM1, DC_PIN2_ROOM1, false);
        airSuction(DC_PIN1_ROOM1, DC_PIN2_ROOM1, true);
      }
    }

    // Ventillation system Room 2
    // Autonomous loop
    if (ventillationOnRoom2) {
      freshAirIsOnRoom2 = !freshAirIsOnRoom2;
      if (freshAirIsOnRoom2) {
        airSuction(DC_PIN1_ROOM2, DC_PIN2_ROOM2, false);
        freshAir(DC_PIN1_ROOM2, DC_PIN2_ROOM2, true);
      } else {
        freshAir(DC_PIN1_ROOM2, DC_PIN2_ROOM2, false);
        airSuction(DC_PIN1_ROOM2, DC_PIN2_ROOM2, true);
      }
    }

    sendData();
    interrupts();
  }
}

void getCommands() {
   byte rcv[3];
   Serial.readBytes(rcv, 3);
   device = rcv[0];
   room = rcv[1];
   state = rcv[2];

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

  // Sensors
  float T1 = voltageToTemperature(analogRead(TEMP_ROOM1));
  float T2 = voltageToTemperature(analogRead(TEMP_ROOM2));
  float L1 = convertIlluminance(analogRead(LIGHT_SENSOR_ROOM1));
  float L2 = convertIlluminance(analogRead(LIGHT_SENSOR_ROOM2));
  
  uint32_t *addrT1, *addrL1, *addrT2, *addrL2;
  
  addrT1 = (uint32_t *)&T1;
  addrL1 = (uint32_t *)&L1;
  addrT2 = (uint32_t *)&T2;
  addrL2 = (uint32_t *)&L2;

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
  
  reportArray[16] = (uint8_t)ventillationOnRoom1;
  reportArray[17] = (uint8_t)lightsRoom1;
  reportArray[18] = (uint8_t)ventillationOnRoom2;
  reportArray[19] = (uint8_t)lightsRoom2;
  
  Serial.write(reportArray, 20);
  Serial.flush();
  delay(1000);
}

void ventillationState(int room, int state) {
  if (room == 1) {
      ventillationOnRoom1 = state;
      if (state == 1) {
        analogWrite(EN_ROOM1, 255);
        if (freshAirIsOnRoom1) {
          airSuction(DC_PIN1_ROOM1, DC_PIN2_ROOM1, false);
          freshAir(DC_PIN1_ROOM1, DC_PIN2_ROOM1, true);
        } else {
          freshAir(DC_PIN1_ROOM1, DC_PIN2_ROOM1, false);
          airSuction(DC_PIN1_ROOM1, DC_PIN2_ROOM1, true);
        }
      } else {
        analogWrite(EN_ROOM1, 0);
        airSuction(DC_PIN1_ROOM1, DC_PIN2_ROOM1, false);
        freshAir(DC_PIN1_ROOM1, DC_PIN2_ROOM1, false);
      }
   } else if (room == 2) {
      ventillationOnRoom2 = state;
      if (state == 1) {
        analogWrite(EN_ROOM2, 255);
        if (freshAirIsOnRoom2) {
          airSuction(DC_PIN1_ROOM2, DC_PIN2_ROOM2, false);
          freshAir(DC_PIN1_ROOM2, DC_PIN2_ROOM2, true);
        } else {
          freshAir(DC_PIN1_ROOM2, DC_PIN2_ROOM2, false);
          airSuction(DC_PIN1_ROOM2, DC_PIN2_ROOM2, true);
        }
      } else {
        analogWrite(EN_ROOM2, 0);
        airSuction(DC_PIN1_ROOM2, DC_PIN2_ROOM2, false);
        freshAir(DC_PIN1_ROOM2, DC_PIN2_ROOM2, false);
      } 
   }
}

void lightState(int room, int state) {
  if (room == 1) {
      lightsRoom1 = state;
      if (state == 1) {
        digitalWrite(RELAY_LIGHT_ROOM1, HIGH);
      } else {
        digitalWrite(RELAY_LIGHT_ROOM1, LOW);
      }
   } else if (room == 2) {
      lightsRoom2 = state;
      if (state == 1) {
        digitalWrite(RELAY_LIGHT_ROOM2, HIGH);
      } else {
        digitalWrite(RELAY_LIGHT_ROOM2, LOW);
      }
   }
}

void freshAir(int pin1, int pin2, bool on) {
  if (on) {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);
  } else {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
  }
}

void airSuction(int pin1, int pin2, bool on) {
  if (on) {
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);
  } else {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
  }
}

float voltageToTemperature(int voltage) {
  float milivoltage = (voltage/1023.) * 5000.;
  float temperature = milivoltage / 10.;
  return temperature;
}

float convertIlluminance(int lightVoltage) {
  float MAX_ADC_READING = 1023.0;
  float ADC_REF = 5.0;
  float resistorVoltage = (float)lightVoltage / (MAX_ADC_READING * ADC_REF);
  float ldrVoltage = ADC_REF - resistorVoltage;
  float ldrResistance = ldrVoltage /resistorVoltage * 5.0;
  float lux = 28704689.27 * pow(ldrResistance, -2.415); 
  return lux;
}
