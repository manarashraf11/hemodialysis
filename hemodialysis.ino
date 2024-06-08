#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTPIN 4      // Pin to which the DHT11 is connected
#define DHTTYPE DHT11 // Type of the DHT sensor

DHT dht(DHTPIN, DHTTYPE);

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Buzzer control pin
int buzzerPin = 6;

// Water flow sensor pin
int flowSensorPin = 5; 

int bloodPumpPin = 7;
int dilysatePumpPin = 8;

// Ultrasonic sensor pins
const int echoPin = 2; 
const int trigPin = 3;  

// Desired flow rate in L/min and temperature in C
float desiredFlowRate = 1;
float desiredTemperature = 23;

float currentFlowRate = 0.0;
float temperature = 0.0;

void setup() {

  pinMode(buzzerPin, OUTPUT);

  pinMode(flowSensorPin, INPUT);

  pinMode(bloodPumpPin, OUTPUT);
  pinMode(dilysatePumpPin, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  lcd.init(); // initialize the lcd 
  lcd.backlight(); // open the backlight
  lcd.clear();

  Serial.begin(9600);
  dht.begin();
}

void loop() {

  digitalWrite(bloodPumpPin, LOW);
  digitalWrite(dilysatePumpPin, HIGH);
  // Read the current flow rate
  int flow = pulseIn(flowSensorPin, HIGH);  // Measure pulse width
  currentFlowRate = 1000.0 / (flow * 2.5);  // Convert pulse width to flow rate

  temperature = dht.readTemperature();
  // lcd.clear();
  // lcd.print("Flow Rate " + String((int)currentFlowRate) + "L/m");
  // lcd.setCursor(0, 1);
  // lcd.print("DilysateTemp " + String((int)temperature) + "C");

  Serial.print("temperature ");
  Serial.print(temperature);
  Serial.println(" C");
  Serial.print("flow rate ");
  Serial.print(currentFlowRate);
  Serial.println(" L/min");

  // Check for occlusion
  if (currentFlowRate < 0.9 * desiredFlowRate){
    Serial.println("Occlusion detected!");
    digitalWrite(bloodPumpPin, LOW);
    digitalWrite(dilysatePumpPin, LOW);

    digitalWrite(buzzerPin, HIGH);  // Turn the buzzer on
    delay(200);  // Wait for 200 milliseconds
    digitalWrite(buzzerPin, LOW);  // Turn the buzzer off
  }

  // check for temperature
  if (desiredTemperature + 2 < temperature || temperature < desiredTemperature - 2){
    Serial.println("Heater fail!");
    digitalWrite(bloodPumpPin, LOW);
    digitalWrite(dilysatePumpPin, LOW);

    digitalWrite(buzzerPin, HIGH);  // Turn the buzzer on
    delay(200);  // Wait for 200 milliseconds
    digitalWrite(buzzerPin, LOW);  // Turn the buzzer off
  }

  // Ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;
  Serial.println(distance);

  // Check for air bubbles
  if (distance > 10) {  // Adjust the threshold as needed
    Serial.println("Air bubbles detected!");
    digitalWrite(bloodPumpPin, HIGH);
    digitalWrite(dilysatePumpPin, LOW);

    digitalWrite(buzzerPin, HIGH);  // Turn the buzzer on
    delay(200);  // Wait for 200 milliseconds
    digitalWrite(buzzerPin, LOW);  // Turn the buzzer off
    delay(200);  // Wait for 200 milliseconds
    digitalWrite(buzzerPin, HIGH);  // Turn the buzzer on
    delay(200);  // Wait for 200 milliseconds
    digitalWrite(buzzerPin, LOW);  // Turn the buzzer off
  }

  delay(1000);
}
