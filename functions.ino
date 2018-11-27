
void updateLCD() {
  if (lcdChanged == true) {
    lcd.setCursor(0, 0);//Column, Row (Starts counting at 0)

    if (userChanged) {
      lcd.print("USER:              ");
      lcd.setCursor(6, 0);//Column, Row (Starts counting at 0)
      selectedUser = listUsers[operatorCounter];
      lcd.print(selectedUser);
      userChanged = false;

    }
    lcd.setCursor(0, 1);//Column, Row (Starts counting at 0)
    lcd.print("PART#: ");
    lcd.print(partNumber[selectedPart][0]);
    lcd.setCursor(0, 2);//Column, Row (Starts counting at 0)
    lcd.print("SERIAL#: ");
    lcd.print(UUTserialNumber);
    lcd.print("     ");

    lcdChanged = false;

    switch (editingRow) {
      case 0: //editing the 0 row on LCD or the user name
        lcd.setCursor(0, 3);//Column, Row (Starts counting at 0)
        lcd.print("LEFT/RIGHT FOR USER");
        lcd.setCursor(6, 0);//Column, Row (Starts counting at 0)

        break;

      case 1: //editing the 1 row on LCD or the part number
        lcd.setCursor(0, 3);//Column, Row (Starts counting at 0)
        lcd.print("LEFT/RIGHT FOR PART#");
        lcd.setCursor(7, 1);//Column, Row (Starts counting at 0)

        break;

      case 2://editing the 2 row on LCD or the serial number
        lcd.setCursor(0, 3);
        lcd.print("KEYPAD FOR SERIAL#  ");
        lcd.setCursor(9 + UUTserialNumber.length(), 2);
        readyToTest = false;

        if (UUTserialNumber.length() == maxSerialNumberLength) {
          lcd.setCursor(0, 3);
          lcd.print("GREEN BTN TO START");
          readyToTest = true;  //use this to verify 6 digits are placed in the serial number spot
        }

        break;
    }
  }
}




void homeStepper() {
  digitalWrite(disableStepperDriverPin, LOW);  // LOW enables the stepper, HIGH disables the stepper
  if (homed == false) {


    stepperX.setMaxSpeed(2000.0);      //Set max speed of stepper
    stepperX.setAcceleration(5000.0);  // Set acceleration of stepper
    stepperX.setCurrentPosition(0);    // Set the current postion as home position
    initial_homing = -1; //this changes based on if going backwards or forwards

    while ( digitalRead(homeSwitchNO) == false) {

      stepperX.moveTo(initial_homing);
      initial_homing = initial_homing - 1; //moving more in each loop
      stepperX.run();   //Go to stepperX.moveTo position
    }

    stepperX.setCurrentPosition(0);    // reset the home position with it's current postion - this also requires a setmaxSpeed and setAcceleration
    stepperX.setMaxSpeed(1000.0);      //Set max speed of stepper (Slower to get better accuracy) 100 is a good starting number
    stepperX.setAcceleration(5000.0);  // Set acceleration of stepper
    initial_homing = 1;  //this changes based on if going backwards or forwards

    while ( digitalRead(homeSwitchNO) == true) {
      //      Serial.println("Stepper is movingclockwise to go to home . . . . . . . . . . . . . . . .");
      stepperX.moveTo(initial_homing);
      initial_homing++;  //Decrement the go to postion by -1
      stepperX.run();   //Go to set postion
      delay(1);   //A delay here but should be a millis function?

    }
    stepperX.setCurrentPosition(0);    // reset the home position with it's current postion it is now homed - this also requires a setmaxSpeed and setAcceleration
    homed = true;



    //    Serial.println("Homing is complete . . . . . . . . . . . . . . . .");


  }
}

void cycleFlatPlate() {

  /*
    colorWipe(strip.Color(0, 0, 0, 0), strip.Color(0, 0, 255, 0)); // Set colors to blue during cycle
    digitalWrite(disableStepperDriverPin, LOW);  // Low enables the stepper, high disables the stepper
    homed = false;
    homeStepper();

    Serial.println("Begining cycle - Going out . . . . . . . . . . . . . . . .");
    stepperX.setMaxSpeed(448.0);      //Changed to 448 = 5 seconds of travel
    stepperX.setAcceleration(10000.0);  // Set acceleration of stepper, from testing 10,000 was a good acceleration
    stepperX.runToNewPosition(cyclePulses); //This is a blocking function, such that it doesn't go to the next step until complete


    delay(8000);  // this is to stop the plate at the end of the stroke long enough for the operator to stop the test.  we only want to read one pass on the test.

    Serial.println("going in. . . . . . . . . . . . . . . .");
    stepperX.setMaxSpeed(1000.0);      //Set max speed of stepper, from testing 750 was a good speed  (Changed to 400 to identify variation in the test results)
    stepperX.runToNewPosition(0); //This is a blocking function, such that it doesn't go to the next step until complete


    previousMillis = currentMillis; // after a cycle reset the timed out feature

    colorWipe(strip.Color(0, 0, 0, 0), strip.Color(0, 0, 0, 255)); // Set colors to white for idle or homing

    delay(8000);
    digitalWrite(disableStepperDriverPin, HIGH);
  */
}

void colorWipe(uint32_t color1, uint32_t color2) {

  for (uint16_t i = 0; i < 5; i++) {
    strip.setPixelColor(i, color1);  //LED's 1, 2, 3, 4
    strip.setPixelColor(i + 56, color1); //LED's 57, 58, 59, 60
    strip.setPixelColor(i + 60, color1); //LED's 61, 62, 63, 64
    strip.setPixelColor(i + 116, color1); //LED's 116, 117, 118, 119
  }

  for (uint16_t j = 4; j < 56; j++) {

    strip.setPixelColor(j, color2); // LED's 3 through 55
    strip.setPixelColor(j + 60, color2); // LED's 63 through 116
  }

  strip.show();

}





void colorProgess(float percentageComplete) { // send an integer between 0 and 100
  if (percentageComplete > 0 &&  percentageComplete < 1) { // error checking to verify the percent complete is between 0 and 100

    int k = (percentageComplete * 56);

    //    for (uint16_t j = 4; j < k; j++) {
    //
    //      strip.setPixelColor(j, 0, 0, 255, 0); // LED's 3 through 55
    //      strip.setPixelColor(j + 60, 0, 0, 255, 0); // LED's 63 through 116
    //    }
    for (uint16_t j = 4; j < k; j++) {
      strip.setPixelColor(j, 0, 0, 255, 0); // LED's 3 through 55
    }

    for (uint16_t j = 115; j > (119 - k) ; j--) {//j = starting point, 119-k count of led,
      strip.setPixelColor(j, 0, 45, 255, 0); // LED's 3 through 55
    }

    strip.show();
  }

  // now update the progress


}
