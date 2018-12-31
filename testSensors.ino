void testSensors() {  //this is called at the userInput start button
  timedOut = false;

  //================================== Open SD card, exit out of testing function if not found  =============================
  lcd.setCursor(0, 3);//Column, Row (Starts counting at 0)
  lcd.print("Initializing SD card");
  //  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    lcd.setCursor(0, 3);//Column, Row (Starts counting at 0)
    lcd.print("initialization failed");
    //    Serial.println("initialization failed!");
    delay(1000);
    lcd.setCursor(0, 3);//Column, Row (Starts counting at 0)
    lcd.print("Check SD Card       ");

    for (int d = 0; d < 10 ; d++) { // this is a blocking function with flashing light to check the SD and safety exit function
      colorWipe(strip.Color(0, 0, 0, 0), strip.Color(255, 128, 0, 0));
      delay(250);
      colorWipe(strip.Color(0, 0, 0, 0), strip.Color(191, 255, 0, 0));
      delay(250);
      colorWipe(strip.Color(0, 0, 0, 0), strip.Color(0, 64, 255, 0));
      delay(250);
      colorWipe(strip.Color(0, 0, 0, 0), strip.Color(255, 0, 191, 0));
      delay(250);
    }
    colorWipe(strip.Color(0, 0, 0, 0), strip.Color(0, 0, 0, 0));
    return; //Exit out of testing to give the operator a chance to fix SD card and start test over
  }
  lcd.setCursor(0, 3);//Column, Row (Starts counting at 0)
  lcd.print("SD initializated    ");




  //================================== unit under test varibles =============================

  selectOrAddressed = (partNumber[selectedPart][4]);  //0 is addressed, and 1 is selected
  float percentComplete; //used for keeping the percent complete value, a float has decimals
  currentMillis = millis(); // Now is set to currentMillis or when the test started (Now), and at the end of the test the difference is the ellapsed time for the test.
  boolean uutPassed = true;  // varible to use for indicating a pass or fail
  //================================== Set neopixels to white =============================
  for (uint16_t i = 0; i < 5; i++) {
    strip.setPixelColor(i, 0, 0, 0, 0);  //LED's 1, 2, 3, 4
    strip.setPixelColor(i + 56, 0, 0, 0, 0); //LED's 57, 58, 59, 60
    strip.setPixelColor(i + 60, 0, 0, 0, 0); //LED's 61, 62, 63, 64
    strip.setPixelColor(i + 116, 0, 0, 0, 0); //LED's 116, 117, 118, 119
  }
  for (uint16_t j = 4; j < 56; j++) {

    strip.setPixelColor(j, 0, 0, 0, 255); // LED's 3 through 55
    strip.setPixelColor(j + 60, 0, 0, 0, 255); // LED's 63 through 116
  }

  //================================== Home =============================
  homeStepper();  // function that homes the stepper plate if needed


  //================================== rapid move to starting test location =============================
  stepperX.setMaxSpeed(1000.0);      //
  stepperX.setAcceleration(5000.0);  // Set acceleration of stepper, from testing 10,000 was a good acceleration
  stepperX.runToNewPosition(testStartLocation); //This is a blocking function, such that it doesn't go to the next step until complete


  //================================== Iniitalize the uutPassFail Arraywith MAX and MIN values =============================
  initializeUUTPassFail(); //initialize the uutPassFail array


  /*================================== collect all testing data =============================
    For RawData.csv -  //Note- limiations of the SD library limit to 8 caharacters for file names
    Here is a sample of what RawData.csv should look like:  Rows - There will only be: HEADS*HALLS=TOTAL ROWS
    0-DATE, 1-TIME, 2-PART#, 3-SERIAL#, (4- MAX)  , (5-MIN) , (6-DIFF), (7-PASS), 8-HALL/HEAD, 9-HEADS, (10-HEAD#), (11- HALL#), 12-USER, 13-FIXTURE,
    12/01/2018,8:08, 121248, B12345   ,    3500   , -589    , 4089    , 1       ,3           ,4       , 0         , 0          , JEFF   , 10,
    12/01/2018,8:08, 121248, B12345   ,    3250   , -125    , 3375    , 1       ,3           ,4       , 0         , 1          , JEFF   , 10,
    12/01/2018,8:08, 121248, B12345   ,    1250   , 25      , 1225    , 0       ,3           ,4       , 0         , 2          , JEFF   , 10,
    12/01/2018,8:08, 121248, B12345   ,    1250   , 25      , 1225    , 0       ,3           ,4       , 1         , 0          , JEFF   , 10,

    (These) values will be collected during the run, all others are static in test cycle and repeated in the file

    For Verbose file or partnumber\Serial#.csv
    Column-0, Column 1 -> (Heads * Halls)
    Step-X, 179, 180, ...

     At the end of the test, or all the steps are complete

     Date:
     Time:
     Part Number:
     Serial Number:
     Number Heads:
     Number Halls/Head:
     User:
     Fixture Number:

  */

  todaysDate = String(now.year()) + '/' + String(now.month()) + '/' + String(now.day());
  timeNow = String(now.hour()) + ':' + String(now.minute()) + ':' + String(now.second());
  String column0Date = todaysDate;
  String column1Time = timeNow; //change code for when RTC is initialized
  String column2PartNumber = String((partNumber[selectedPart][0]));
  String column3SerialNumber = serialNumArray;
  column8numberHallsPerHead = partNumber[selectedPart][2];
  column9numberHeads = partNumber[selectedPart][3];
  String column12User = selectedUser;
  byte column13Fixture =   readIdentifier;
  int16_t diffMaxLimit = partNumber[selectedPart][12];
  int16_t diffMinLimit = partNumber[selectedPart][13];

  txtSerialNumber = ""; //write the serial number to this string instead of array
  for (int i = 0; i < 6; i++) {
    txtSerialNumber = txtSerialNumber + serialNumArray[i];
  }




  //================================== Open the SD card for partnumber\Serial#.csv or Verbose=============================
  if (!SD.exists(column2PartNumber)) {
    SD.mkdir(column2PartNumber);
  }


  String partNumberAndFolder = (column2PartNumber) + "/" + txtSerialNumber + ".csv";
  myFile = SD.open(partNumberAndFolder , FILE_WRITE);
  lcd.setCursor(0, 3);//Column, Row (Starts counting at 0)
  lcd.print("                    ");
  lcd.setCursor(0, 3);//Column, Row (Starts counting at 0)
  lcd.print(partNumberAndFolder);

  for ( int i = 0; i < testingLoopCount; i++) {
    percentComplete = ((float(i) / float(testingLoopCount)));
    colorProgess(percentComplete);

    stepperX.runToNewPosition(testStartLocation + i);

    switch (selectOrAddressed) { //0 is addressed, and 1 is selected
      case 0:
        addressedReadSensor();
        break;
      case 1:
        selectedReadSensors();
        break;
    }

    myFile.print("Step - ");
    myFile.print(i);
    myFile.print(", ");
    for (int j = 0; j <  column9numberHeads * column8numberHallsPerHead; j++) {
      myFile.print(uutVerbose[j]);  //dump uutVerbose array to SD card
      myFile.print(", ");  // this also leaves a comma at the end of the line
    }

    myFile.println();
  }
  myFile.print("Date: ,");
  myFile.println(todaysDate);
  myFile.print("Time: ,");
  myFile.println(timeNow);
  myFile.print("Part Number: ,");
  myFile.println(column2PartNumber);
  myFile.print("Serial Number: ,");
  myFile.println(txtSerialNumber);
  myFile.print("Number Heads: ,");
  myFile.println(column9numberHeads);
  myFile.print("Number Halls/Heads: ,");
  myFile.println(column8numberHallsPerHead);
  myFile.print("User Name: ,");
  myFile.println(column12User);
  myFile.print("Tester Identifier: ,");
  myFile.println(readIdentifier);
  myFile.print("The Set Current Difference Max Value: ,");
  myFile.println(diffMaxLimit);
  myFile.print("The Set Current Difference Min Value: ,");
  myFile.println(diffMinLimit);

  long elapsedTimeMS = millis() - currentMillis;
  myFile.print("Test took (ms): ,");
  myFile.println(elapsedTimeMS);
  myFile.close();

  // turn off digital wirte to heads
  digitalWrite(headSelect0, LOW);
  digitalWrite(headSelect1, LOW);
  digitalWrite(headSelect2, LOW);
  digitalWrite(headSelect3, LOW);


  stepperX.runToNewPosition(+10); //move to almost home
  previousMillis = millis(); // Reset the timer - any time you have an interaction with the tester this is reset to 0, when the elapsed time runs out the tester shuts down: neopixels & stepper off

  //================================== Compare the Difference to Max and Min =============================
  //================================== Used to create EOLTRawDate.csv =============================
  //here is a loop to read the pass uutPassFail[addressedhall][0] and compare to max and min values
  // as the test was looping the uutPassFail array recored the max and min for each hall
  //the uutPassFail array columns are: 0-MAX, 1-MIN, 2-Diff, 3-PASS, 4-Head number, 5-Hall Number.

  //================================Compare MAX and MIN and update uutPassFail array ====================================
  for (int k = 0; k < column9numberHeads * column8numberHallsPerHead; k++) {
    uutPassFail[k][2] = uutPassFail[k][0] - uutPassFail[k][1]; // Max[0] - Min[1] = Diff[2]
    if (uutPassFail[k][2] < diffMaxLimit && uutPassFail[k][2] > diffMinLimit) {
      uutPassFail[k][3] = 1;  //Pass written to the row
    } else {
      uutPassFail[k][3] = 0; //Fail
    }

    //============This equation doesn't give the right head number based on the step=================================
    //right now it returns (k % column8numberHallsPerHead) => 0,1,2,0,1,2,0... it should => 0,0,0,1,1,1,2,2,2,3,3,3  based halls per head

    //    uutPassFail[k][4] = k % column8numberHallsPerHead; //Write head number using the MODULAS function

    //==========================================================================================================

    uutPassFail[k][5] = k % column8numberHallsPerHead; //Write hall number using the MODULAS function
    //    uutPassFail[k][5] = k % column9numberHeads; //Write hall number using the MODULAS function// this line erroniuoslly reported the head number not the hall number
  }

  //=====================================Open and write to RawData.csv =============================

  if (SD.exists("RawData.csv")) {
    myFile = SD.open("RawData.csv" , FILE_WRITE);
  }
  else {//Write the header to the RawData.csv only once at the top of the csv file
    myFile = SD.open("RawData.csv" , FILE_WRITE);
    //Print the first row of the RawData.csv and only print it once
    myFile.println("0-Date, 1-Time, 2-Part#, 3-Serial#, 4-Max, 5-Min, 6-Diff, 7-Pass ,8-Halls/Head, 9-Head, 10-Head#, 11-Hall#, 12-User, 13-Set Diff Max, 14-Set Diff Min, 15-EEPROM Id,");
  }

  if (myFile) {
    lcd.setCursor(0, 3);//Column, Row (Starts counting at 0)
    lcd.print("RawData.csv         ");
    delay(1000);
  }
  else {
    lcd.setCursor(0, 3);//Column, Row (Starts counting at 0)
    lcd.print("can't open RawData  ");
    delay(1000);
    lcd.print("No data written!    ");
    delay(1000);
    return;
  }

  String sensorTestReport = ""; //String to print to LCD - this will be long
  sensorTestReport = "REPORT HEAD:HALL-DIFF ";

  for (int k = 0; k < column9numberHeads * column8numberHallsPerHead; k++) {
    myFile.print(todaysDate); //0-Date
    myFile.print(",");
    myFile.print(timeNow); //1-Time
    myFile.print(",");
    myFile.print(column2PartNumber); //2-Part#
    myFile.print(",");
    myFile.print(txtSerialNumber); //3-Serial#
    myFile.print(",");
    myFile.print(uutPassFail[k][0]); // 4-Max
    myFile.print(",");
    myFile.print(uutPassFail[k][1]); // 5-Min
    myFile.print(",");
    myFile.print(uutPassFail[k][2]); // 6-Dif
    myFile.print(",");

    if (uutPassFail[k][3] == 1) {
      myFile.print("Pass,");  // 7-Pass/Fail
      sensorTestReport = sensorTestReport + (uutPassFail[k][4]) + ":" + (uutPassFail[k][5]) + "-" + (uutPassFail[k][2]) + " PASS,    ";
    } else {
      myFile.print("Fail,");
      uutPassed = false;  //at the beginning of this test, uutPassed was set to true, if any value fails it gets set to false
      sensorTestReport = sensorTestReport + (uutPassFail[k][4]) + ":" + (uutPassFail[k][5]) + "-" + (uutPassFail[k][2]) + " FAIL,    ";
    }
    myFile.print(column8numberHallsPerHead); // 8-Halls / head
    myFile.print(",");
    myFile.print(column9numberHeads); // 9-Halls / head
    myFile.print(",");
    myFile.print(uutPassFail[k][4]); // 10-Head Number
    myFile.print(",");
    myFile.print(uutPassFail[k][5]); // 11-Hall Number
    myFile.print(",");
    myFile.print(column12User); //12-User
    myFile.print(",");
    myFile.print(diffMaxLimit);
    myFile.print(",");
    myFile.print(diffMinLimit);
    myFile.print(",");
    myFile.print(readIdentifier); //13-EEPROM Id
    myFile.println(",");
  }

  //==============================Close RawData.csv=================================
  myFile.close();
  digitalWrite(disableStepperDriverPin, HIGH); //High disable the stepper

  if (uutPassed == true) {
    colorWipe(strip.Color(0, 0, 0, 0), strip.Color(0, 225, 0, 0));  // Set all neopixels to off.  The 1st is for the 4 end strips, and the 2nd the top neopixels
  } else {//Failed
    colorWipe(strip.Color(0, 0, 0, 0), strip.Color(255, 0, 0, 0));  // Set all neopixels to RED.  The 1st is for the 4 end strips, and the 2nd the top neopixels
  }

  lcd.setCursor(0, 0);
  lcd.print("LEFT:REV. DOWN=PAUSE");

  scrollLongMessageLCD(sensorTestReport);  //Send the long test report to be printed on LCD
  colorWipe(strip.Color(0, 0, 0, 0), strip.Color(0, 0, 0, 0)); //Turn off the neopixles after the test
  lcdChanged = true;
  userChanged = true;

}
