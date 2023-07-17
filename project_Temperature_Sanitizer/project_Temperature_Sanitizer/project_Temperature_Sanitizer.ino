// Libraries:
/* Built-In Library */
#include<Wire.h>

/* Install: Adafruit MLX90614 Library by Adafruit */
#include <Adafruit_MLX90614.h>

/* Download Zip: https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library */
#include<LiquidCrystal_I2C.h>

// Pin Numbers:
#define IR_Temp_Pin 3
#define IR_Sanitizer_Pin 2
#define Relay_Pin 4
#define Buzzer_Pin 5

// Constants:
const byte objLCDNumCols = 16; // LCD's number of columns
const byte objLCDNumRows = 2;  // LCD's number of rows

// Objects:
LiquidCrystal_I2C objLCD(0x27, objLCDNumCols, objLCDNumRows);
Adafruit_MLX90614 objMLX = Adafruit_MLX90614();

// Variables:
float valAmbientTemp, valTargetTemp;
float tempTH = 100.0;
unsigned long relayDelay = 400;
int Step = 1;

void setup() {
  /* Begin serial communication with Arduino and Arduino IDE (Serial Monitor) */
  Serial.begin(9600);

  /* Define inputs and outputs */
  pinMode(IR_Temp_Pin, INPUT);
  pinMode(IR_Sanitizer_Pin, INPUT);

  pinMode(Relay_Pin, OUTPUT);
  digitalWrite(Relay_Pin, LOW);

  pinMode(Buzzer_Pin, OUTPUT);
  digitalWrite(Buzzer_Pin, LOW);

  /* Initializes the MLX90614 */
  objMLX.begin();

  /* Initializes the LCD */
  objLCD.begin(16,2);
  objLCD.backlight();
}

void loop() {
  Serial.print(digitalRead(IR_Temp_Pin));
  Serial.print(":");
  Serial.println(digitalRead(IR_Sanitizer_Pin));

  if (Step == 1) {
    Serial.println("Step 1");
    objLCD.clear();
    objLCD.setCursor(0, 0);  //(colIdx, rowIdx)
    objLCD.print("Please check    ");
    objLCD.setCursor(0, 1);  //(colIdx, rowIdx)
    objLCD.print("your temperature");
    Step = 2;
  } else if (Step == 2 && !digitalRead(IR_Temp_Pin)) {
    Serial.println("Step 2");
    delay(50);
    valAmbientTemp = objMLX.readAmbientTempF(); /* Gets the values of ambient temperature */
    valTargetTemp = objMLX.readObjectTempF();   /* Gets the values of targeted object's temperature */
    Buzzer(1);
    objLCD.clear();
    objLCD.setCursor(0, 0);
    objLCD.print("Ambient:");
    objLCD.print(valAmbientTemp);
    objLCD.print((char)223);  /* Prints degree symbol using extended ASCII characters */
    objLCD.print("F");

    objLCD.setCursor(0, 1);
    objLCD.print("Target :");
    objLCD.print(valTargetTemp);
    objLCD.print((char)223);  /* Prints degree symbol using extended ASCII characters */
    objLCD.print("F");
  
    delay(2500);  // Delay of 2.5sec
    objLCD.clear();
    objLCD.setCursor(0, 0);
    if (valTargetTemp >= tempTH) {
      objLCD.print("Access denied!  ");
      Buzzer(3);
      Step = 1;
    } else {
      objLCD.print("Sanitize your   ");
      objLCD.setCursor(0, 1);  //(colIdx, rowIdx)
      objLCD.print("hands           ");
      Step = 3;
    }
  } else if (Step == 3 && !digitalRead(IR_Sanitizer_Pin)) {
    Serial.println("Step 3");
    digitalWrite(Relay_Pin, HIGH);
    delay(relayDelay);
    digitalWrite(Relay_Pin, LOW);
    Step = 1;
  }
}

void Buzzer(int n) {
  for (int i = 1; i <= n; i++) {
    digitalWrite(Buzzer_Pin, HIGH);
    delay(500);
    digitalWrite(Buzzer_Pin, LOW);
    delay(500);
  }
}
