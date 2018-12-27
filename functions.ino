

void homeStepper() {
  digitalWrite(disableStepperDriverPin, LOW);  // LOW enables the stepper, HIGH disables the stepper
  stepperX.setMaxSpeed(100.0);      //Set max speed of stepper
  stepperX.setAcceleration(100.0);  // Set acceleration of stepper
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
    stepperX.moveTo(initial_homing); //go home as defined in setup, it can be close to the microswitch of the plate in the middle
    initial_homing++; //Decrement the go to postion by -1
    stepperX.run();   //Go to set postion
    delay(1);         //A delay time enough to read the microswitch
  }
  stepperX.setCurrentPosition(0);    // reset the home position with it's current postion it is now homed - this also requires a setmaxSpeed and setAcceleration
  homed = true;
}


void colorWipe(uint32_t color1, uint32_t color2) {//1st color is for the ends, the second is for the top row
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

    int k = (percentageComplete * 56); //I only want 56 neopixels to turn on and I need the percentage example: 10% of 56 = 5, (not 5.6 because it's an integer, it rounds down)
    for (uint16_t j = 55; j > (58 - k); j--) {    //(Starting Neopixel to turn on; Where to stop ; Step )
      strip.setPixelColor(j, 0, 0, 255, 0); // LED's 3 through 55
    }
    for (uint16_t j = 64; j < (61 + k) ; j++) {   // (Starting Neopixel to turn on; Where to stop ; Step )
      strip.setPixelColor(j, 0, 0, 255, 0); // LED's 3 through 55
    }
    strip.show();
  }
}

void selectedReadSensors() {
  /*steps to read sensors consits of
     1. Note: Pass fail array needs to be set up before entering this function, set the max to 0's and mins to 32,767 in uutPassFail[X][0&1]
        this should only be done once at the beginning of the test
     2. Loop through the heads (0 to last head)
     2.1. Turn on digital output, switch - head 0 is D28 set HIGH, etc... for each head
     2.2. Hall Loop (Loop through halls 0 - last hall)
     2.2.1 loop Read ADC for each hall
        2.2.1 write read hall value to uutVerbose[x] where x is hall number
        2.2.2 read uutPassFail[x][0] and compare to current read value if larger write
        2.2.3 read uutPassFail[x][1] and compare to current read value if smaller write
     3. Return to testing loop

  */

  int16_t hallReading;
  //  Serial.print("part number testing is a Seleceted type ");
  for (int i = 0; i < column9numberHeads; i++) {
    switch (i) {// in each case only one head is read as indicated by the HIGH in each case
      case 0: //head 0
        digitalWrite(headSelect0, HIGH);
        digitalWrite(headSelect1, LOW);
        digitalWrite(headSelect2, LOW);
        digitalWrite(headSelect3, LOW);

        break;

      case 1://head 1
        digitalWrite(headSelect0, LOW);
        digitalWrite(headSelect1, HIGH);
        digitalWrite(headSelect2, LOW);
        digitalWrite(headSelect3, LOW);

        break;

      case 2: //head 2
        digitalWrite(headSelect0, LOW);
        digitalWrite(headSelect1, LOW);
        digitalWrite(headSelect2, HIGH);
        digitalWrite(headSelect3, LOW);


        break;
      case 3: //head 3
        digitalWrite(headSelect0, LOW);
        digitalWrite(headSelect1, LOW);
        digitalWrite(headSelect2, LOW);
        digitalWrite(headSelect3, HIGH);


        break;
    }
    for (int j = 0; j < column8numberHallsPerHead; j++) { // now that the head is selected read each of its HALLS
      hallReading = ads.readADC_SingleEnded(j);

      uutVerbose[i * column8numberHallsPerHead + j] = hallReading; //write the Hall value into the verbose array

      //=================compare max in pass / fail array=================================
      if (uutPassFail[i * column8numberHallsPerHead + j][0] < hallReading) {
        uutPassFail[i * column8numberHallsPerHead + j][0] = hallReading;
      }
      //=================compare min in pass / fail array=================================
      if (uutPassFail[i * column8numberHallsPerHead + j][1] > hallReading) {
        uutPassFail[i * column8numberHallsPerHead + j][1] = hallReading;
      }
      uutPassFail[i * column8numberHallsPerHead + j][4] = i;  //write the head number to the uutPassFail array once if different
    }
  }
}


void addressedReadSensor() {
  /*steps to read sensors consits of
     1. Note: Pass fail array needs to be set up before entering this function, set the max to 0's and mins to 32,767 in uutPassFail[X][0&1]
        this should only be done once at the beginning of the test
     2. Loop through the HEADS (0 to last head)
     3. Loop through HALLS  (0 to last)
        3.1 Switch on HEAD
            3.1.1 Case 0 through last turning on/off digital outputs
        3.2 read ADC HALL (0 to last)
        3.3 write read hall value to uutVerbose[x] where x is hall number
        3.4 read uutPassFail[x][0] and compare to current read value if larger swap
        3.5 read uutPassFail[x][1] and compare to current read value if smaller swap

     4. Return to testing loop

  */
  //long uutVerbose[18];
  //long uutPassFail[18][3];

  //  Serial.print("part number testing is a addressed type ");
  int16_t hallReading;

  for (int i = 0; i < column9numberHeads; i++) {
    //Head Loop
    for (int j = 0; j < column8numberHallsPerHead; j++) {
      //Hall Loop
      int addressedhall = i * column8numberHallsPerHead + j;
      switch (j) {//halls
        case 0:
          digitalWrite(headSelect0, LOW);
          digitalWrite(headSelect1, LOW);
          digitalWrite(headSelect2, LOW);
          digitalWrite(headSelect3, LOW);
          break;
        case 1:
          digitalWrite(headSelect0, HIGH);
          digitalWrite(headSelect1, LOW);
          digitalWrite(headSelect2, LOW);
          digitalWrite(headSelect3, LOW);
          break;
        case 2:
          digitalWrite(headSelect0, LOW);
          digitalWrite(headSelect1, HIGH);
          digitalWrite(headSelect2, LOW);
          digitalWrite(headSelect3, LOW);
          break;
        case 3:
          digitalWrite(headSelect0, HIGH);
          digitalWrite(headSelect1, HIGH);
          digitalWrite(headSelect2, LOW);
          digitalWrite(headSelect3, LOW);
          break;
        case 4:
          digitalWrite(headSelect0, LOW);
          digitalWrite(headSelect1, LOW);
          digitalWrite(headSelect2, HIGH);
          digitalWrite(headSelect3, LOW);
          break;
        case 5:
          digitalWrite(headSelect0, HIGH);
          digitalWrite(headSelect1, LOW);
          digitalWrite(headSelect2, HIGH);
          digitalWrite(headSelect3, LOW);
          break;
      }

      hallReading = ads.readADC_SingleEnded(i); //use the Head loop i to count 0 - last head

      //-------------------------------Code to verify proper function remove when done---------------
      //      hallReading = random(1000);
      //      uutVerbose[addressedhall] = hallReading; //write the Hall value into the bverbose array
      //      Serial.print("Hall number = ");
      //      Serial.print(i);
      //      Serial.print("  -  Reading = ");
      //      Serial.print(hallReading);
      //      Serial.print("  =these should be equal= ");
      //      Serial.println(uutVerbose[addressedhall]);


      //      hallReading = random(1000);
      //-------------------------------Code to verify proper function remove when done---------------
      uutVerbose[addressedhall] = hallReading; //write the Hall value into the verbose array


      //=================compare max in pass / fail array=================================
      if (uutPassFail[addressedhall][0] < hallReading) {
        uutPassFail[addressedhall][0] = hallReading;
      }
      //=================compare min in pass / fail array=================================
      if (uutPassFail[addressedhall][1] > hallReading) {
        uutPassFail[addressedhall][1] = hallReading;
      }
      uutPassFail[addressedhall][4] = i;//write the head number to the uutPassFail
    }
  }
}


void initializeUUTPassFail() {//row 0 is MAX, 1 is MIN, 2 is Diff, 3 PASS, 4 Head number, 5 hall Number.
  for (int i = 0; i < MAXHALLS; i++) {
    uutPassFail[i][0] = 0; //initalize the MAX values for each hall to 0 zero
    uutPassFail[i][1] = 32766; //initalize the MIN values for each hall to interger max value 2^12=32,767
    uutPassFail[i][2] = 0; // Set the Differenace to 0
    uutPassFail[i][3] = 0; // PASS, 0 equals a fail, and a 1 is a pass
    uutPassFail[i][4] = 0; // HEAD Number
    uutPassFail[i][5] = 0; // HALL
  }
}
