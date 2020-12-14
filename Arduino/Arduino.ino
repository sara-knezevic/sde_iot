#define RELAY_LIGHT_ROOM1 13
#define RELAY_LIGHT_ROOM2 12

#define EN_ROOM1 7
#define DC_PIN1_ROOM1 6
#define DC_PIN2_ROOM1 5

#define EN_ROOM2 4
#define DC_PIN1_ROOM2 3
#define DC_PIN2_ROOM2 2

#define TEMP_ROOM1 A0
#define TEMP_ROOM2 A1

#define LIGHT_SENSOR_ROOM1 A2
#define LIGHT_SENSOR_ROOM2 A3

static const uint8_t DEVICE_ADDRESS = 0x01;
static const uint8_t PC_ID = 0x02;

byte messageArray[16];

int lightSensorRoom1, lightSensorRoom2;
unsigned long previousMillis = 0;
bool freshAirIsOn = false;

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
  analogWrite(EN_ROOM1, 255); // == PWM
  analogWrite(EN_ROOM2, 255); // == PWM

  unsigned long currentMillis = millis();

  // Each minute
  if (currentMillis % 60000 == 0) {
    // Ventillation system
    if (freshAirIsOn) {
      airSuction(DC_PIN1_ROOM1, DC_PIN2_ROOM1, false);
      freshAir(DC_PIN1_ROOM1, DC_PIN2_ROOM1, true);
    } else {
      freshAir(DC_PIN1_ROOM1, DC_PIN2_ROOM1, false);
      airSuction(DC_PIN1_ROOM1, DC_PIN2_ROOM1, true);
    }

    freshAirIsOn = !freshAirIsOn;

    // Sensors
    float measuredTemperature1 = voltageToTemperature(analogRead(TEMP_ROOM1));
    float measuredTemperature2 = voltageToTemperature(analogRead(TEMP_ROOM2));
    float measuredIlluminance1 = convertIlluminance(analogRead(LIGHT_SENSOR_ROOM1));
    float measuredIlluminance2 = convertIlluminance(analogRead(LIGHT_SENSOR_ROOM2));

    sendSensorData(measuredTemperature1, measuredTemperature2, measuredIlluminance1, measuredIlluminance2);
  }
}

void sendSensorData(float T1, float L1, float T2, float L2) {

  uint32_t *addrT1, *addrL1, *addrT2, *addrL2;
  
  addrT1 = (uint32_t *)&T1;
  addrL1 = (uint32_t *)&L1;
  addrT2 = (uint32_t *)&T2;
  addrL2 = (uint32_t *)&L2;
  
  messageArray[0] = (uint8_t)(*addrT1 & 0xFF);
  messageArray[1] = (uint8_t)(*addrT1 >> 8) & 0xFF;
  messageArray[2] = (uint8_t)(*addrT1 >> 16) & 0xFF;
  messageArray[3] = (uint8_t)(*addrT1 >> 24) & 0xFF;
  
  messageArray[4] = (uint8_t)(*addrL1 & 0xFF);
  messageArray[5] = (uint8_t)(*addrL1 >> 8) & 0xFF;
  messageArray[6] = (uint8_t)(*addrL1 >> 16) & 0xFF;
  messageArray[7] = (uint8_t)(*addrL1 >> 24) & 0xFF;
  
  messageArray[8] = (uint8_t)(*addrT2 & 0xFF);
  messageArray[9] = (uint8_t)(*addrT2 >> 8) & 0xFF;
  messageArray[10] = (uint8_t)(*addrT2 >> 16) & 0xFF;
  messageArray[11] = (uint8_t)(*addrT2 >> 24) & 0xFF;
  
  messageArray[12] = (uint8_t)(*addrL2 & 0xFF);
  messageArray[13] = (uint8_t)(*addrL2 >> 8) & 0xFF;
  messageArray[14] = (uint8_t)(*addrL2 >> 16) & 0xFF;
  messageArray[15] = (uint8_t)(*addrL2 >> 24) & 0xFF;

  Serial.write(messageArray, 16);
  delay(1000);
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

float voltageToTemperature(float voltage) {
  float milivoltage = (voltage/1023.0) * 5000;
  float temperature = milivoltage / 10;
  return temperature;
}

float convertIlluminance(int resistance) {
  float cal = (250.0/(0.0048828125*resistance)) - 50.0;
  float difference = 33.0 - cal;
  float lux = int(33.0 + difference);
  return lux;
}
