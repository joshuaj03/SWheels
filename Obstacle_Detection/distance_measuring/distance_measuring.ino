#define NUM_SENSORS 7

#define LED_BLUE 2

const int TRIG_PIN = 13;
const int echo_pins[NUM_SENSORS] = {14,27,26,25,33,32,4};
unsigned long previousMillis = 0;
unsigned long currentMillis;
const long interval = 100;

void setup() {
  Serial.begin(115200);

  pinMode(LED_BLUE, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(echo_pins[i], INPUT);
  }
  digitalWrite(LED_BLUE, HIGH);
}

void loop() {
  float distance[NUM_SENSORS];

  for (int i = 0; i < NUM_SENSORS; i++) {
    distance[i] = get_distance(echo_pins[i]);
    delay(30);
  }
  Serial.write((uint8_t*)distance, sizeof(distance));
  Serial.println();
  for (int i = 0; i < NUM_SENSORS; i++) {
    if (distance[i] > 0 && distance[i] < 500) {
      Serial.print("Obstacle detected at Sensor ");
      Serial.print(i);
      Serial.print(" | Distance: ");
      Serial.print(distance[i]);
      Serial.println(" cm");
    } else {
      Serial.print("Obstacle detected at Sensor ");
      Serial.print(i);
      Serial.println("Out of range...");
    }
  }
  delay(100);  
}

float get_distance(int echo_pin) {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(echo_pin, HIGH, 25000);
  if (duration == 0) return -1;
  float distance = (duration * 0.0343) / 2; // Distance calculated in cm

  return distance;
}