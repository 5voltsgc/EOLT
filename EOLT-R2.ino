/*
  Sketch End Of Line Tester - EOLT

  User Interface:
  User name:
  The users selects their name from a pre-programmed list or array of names.  The list can only be updated in the main program.
  The left and right buttons move up and down the list of names
  Part Number:
  Is selected by pressing the down button, then using the left/right buttons to select a pre-programmed part number to test.
  This list also conatins the testing parameters, like heads, halls, number of analog pins, and digital select pins, as well as
  testing pass fail criteria.

  LCD:
  As the users selects user, part number, and serial number.  varibles are updated and passed to the LCD update function.

  Testing:
  With the User Input completed the "Press Green button to Start" will be displayed on the LCD
  1. The defect plate moves into position - about .75" between the defect and the sensor heads
  2. The plate will begin moving 1 step in a loop
      2.1. test array will be updated with each hall reading - Based on heads per hall, and how to activate the heads - a digital out. Select or Addressed
      2.2. the neopixels will be updated
      2.3. Write one line to the SD card for the Long form or all the reading CSV file
      2.4. Update the Consolidated Array - reading the new ADC value, if higher then one in array replace, else move on. and visa versa for low
      2.5. Move to next step and repeat ~500 times for the 1.75 inches

  When at the end of the cycles ~1.5", compare the results fo the Highs, Lows, and Diff to see if it passed
  If Passed
    Update the Neopixles to green
    update LCD screen - "Unit Passed"

  If failed
    Update the Neopixles to red
    update LCD screen - "Unit failed"
    Press left arrow to see why?


  After a preset amount of time the "timedOut" varible will be set to true and the arduino will shut down the stepper motor by applying power to the
  enable input on the DQ542MA microstep driver.  Low level for enabled.

  Created November 5, 2018
  By Jeff Watts
*/



// ====================for SD Card====================
#include <SD.h>

// ====================for stepper====================
// Library created by Mike McCauley at http://www.airspayce.com/mikem/arduino/AccelStepper/
#include <AccelStepper.h>
const byte disableStepperDriverPin = 34;  //Enable pin on DQ542MA, When high, the stepper driver goes into sleep mode reducing heat on the driver and stepper motor
const byte stepperDirectionPin = 33; // Direction pin on DQ542MA
const byte stepperStepPin = 32; //Pulse pin on DQ542MA
const byte homeSwitchNO = 35;// Normally open position of switch
bool disableStepper = false;  // varible to disable the stepper after timed out

// ====================Stepper Travel Variables====================
// Used to store test cycle distance 20 tooth pulley Dia=.509 - circumference = 1.25", pulses per revolution=400, travel distance=5.5"
long cyclePulses = ((5.5 / (1.25 * 3.14)) * 400); //((5.5/(1.25 * 3.14)) * 400) = 560.5 (pulses for test travel)
long initial_homing = -1; // Used to Home Stepper at startup
bool startCycleTest = false;
bool startCycleTestButton = false;
bool homed = false;  //varible to indicate homed

// ====================AccelStepper Setup====================
AccelStepper stepperX(1, stepperStepPin, stepperDirectionPin);   // 1 = Driver interface

//====================for NeoPixels=======================
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

const byte neopixelsPin = 37; // Pin the Neopixels data is connected with a 470ohm resitor
const byte numNeoPixels = 120; //two strips at 60 each
const byte neopixelBrightness = 20; //brightness
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numNeoPixels, neopixelsPin, NEO_GRBW + NEO_KHZ800);

//====================for Elasped time=======================
bool timedOut = false;  //when timed out high disable stepper driver and neopixels
unsigned long previousMillis = 0;        // will store last time timedOut was updated
const long interval = 60000;           // interval at which to blink (milliseconds)  1 minute = 60,000 milliseconds, 10 min=600,000
unsigned long currentMillis;

//====================Inputs / Outputs=======================
const byte buttonUp = 40; //This is pulled down to ground by a 10K ohm resistor
const byte buttonDown = 42; //This is pulled down to ground by a 10K ohm resistor
const byte buttonLeft = 43; //This is pulled down to ground by a 10K ohm resistor
const byte buttonRight = 41; //This is pulled down to ground by a 10K ohm resistor
const byte buttonStart = 53; //This is pulled down to ground by a 10K ohm resistor

boolean buttonStateUp;
boolean buttonStateDown;
boolean buttonStateLeft;
boolean buttonStateRight;
boolean buttonStateGreen;


//====================Users=======================
int countOperators = 24;
char listUsers[24][11] = {
  "Anita",
  "Bruce",
  "Cheryl",
  "Debbie",
  "Deborah",
  "Edward",
  "Jeff K.",
  "Jeff W.",
  "Joby",
  "John",
  "Operator 1",
  "Operator 2",
  "Operator 3",
  "Operator 4",
  "Operator 5",
  "Operator 6",
  "Operator 7",
  "Operator 8",
  "Operator 9",
  "Sheldon",
  "Sop",
  "Stephen",
  "Vikki",
  "Wade",
};

int operatorCounter = 0; //used to hold the point in the array of users 0 equals the first user
String selectedUser; // used to hold text of the selected User
boolean userChanged = true;


//====================Global Varibles=======================
int editingRow = 0;
boolean readyToTest = false;
const int testingLoopCount = 50;


//====================PartNumbers=======================
const byte PN_ROWS = 10; //for Rows,  change this number to match the count of rows - 10 rows equals 10 rows starting count from 1.  As in 1,2,3,4,5,6,7,8,9,10
const byte PN_COLS = 15; //four columns

//Make the array in excel and export as a csv file with these headers
//0-Part Numbers,1-Count Halls,2-halls/head,3-# heads,4-Selected,5-addressed,6-address lines,7-# analog lines,8-highMax,9-highMin,10-lowMax,11-lowMin,12-diffMax,13-diffLow
long partNumber[PN_ROWS][PN_COLS] = {
  {107287, 8, 2, 4, 1, 0, 4, 2, 1000, 700, -600, -900, 1450, 1750},
  {107297, 8, 2, 4, 1, 0, 4, 2, 1000, 700, -600, -900, 1450, 1750},
  {108144, 8, 2, 4, 1, 0, 4, 2, 1000, 700, -600, -900, 1450, 1750},
  {108150, 8, 2, 4, 1, 0, 4, 2, 1000, 700, -600, -900, 1450, 1750},
  {112497, 6, 2, 3, 1, 0, 3, 2, 1000, 700, -600, -900, 1450, 1750},
  {121248, 12, 3, 4, 1, 0, 4, 3, 1000, 700, -600, -900, 1450, 1750},
  {121250, 18, 6, 3, 0, 1, 3, 3, 1000, 700, -600, -900, 1450, 1750},
  {121334, 15, 5, 3, 0, 1, 3, 3, 1000, 700, -600, -900, 1450, 1750},
  {121335, 15, 5, 3, 0, 1, 3, 3, 1000, 700, -600, -900, 1450, 1750},
  {121791, 12, 6, 2, 0, 1, 3, 2, 1000, 700, -600, -900, 1450, 1750}
};

int selectedPart = 2;
int partCounter = 0;

//====================LCD Screen=======================
// Connect via i2c, default address #0 (A0-A2 not jumpered)
#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"
#include "Arduino.h"
Adafruit_LiquidCrystal lcd(0);
boolean lcdChanged = true; // update the LCD screen when ever a change happens to the LCD data - Set it to update on first time through

//====================4X4 Key Pad=======================
#include "Keypad.h"

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
int colPosition = 0;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {47, 46, 45, 44}; //connect to the row pinouts of the keypad - four in a row
byte colPins[COLS] = {39, 38, 49, 48}; //connect to the column pinouts of the keypad - four in a row
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//====================Serial Number =======================

String UUTserialNumber; //
const byte maxSerialNumberLength = 6;
const String serialnumberStartsWith = "B";





void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  strip.begin(); // begin neopixels
  strip.show(); // Initialize all neopixels to 'off'
  pinMode(buttonStart, INPUT);
  pinMode(buttonRight, INPUT);
  pinMode(buttonLeft, INPUT);
  pinMode(buttonDown, INPUT);
  pinMode(buttonUp, INPUT);

  pinMode(disableStepperDriverPin, OUTPUT);
  digitalWrite(disableStepperDriverPin, LOW);  // Low enables the stepper, high disables the stepper

  // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
  lcd.clear();
  // ====================Software Version====================
  String softwareVersion = "0.0.0.1"; // update this as the program changes
  String softwareDate = "11/13/2018"; // update this as the program changes

  lcd.print("Software Date:");
  lcd.setCursor(0, 1);//Column, Row (Starts counting at 0)
  lcd.print(softwareDate);
  lcd.setCursor(0, 2);//Column, Row (Starts counting at 0)
  lcd.print("Software Verision:");
  lcd.setCursor(0, 3);//Column, Row (Starts counting at 0)
  lcd.print(softwareVersion);
  delay(4000);
  lcd.clear();
  lcd.blink();
}

void loop() {
  // put your main code here, to run repeatedly:
  userInput();
  updateLCD();
  serialNumberKeyPad();

  //=============================================remove testing when finished debugging
  testSensors();
}









