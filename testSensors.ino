void testSensors() {  //this is called at the userInput start button


  timedOut = false;
  homeStepper();

  //Write white to LEDs on top only not the ends
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


  //================================== rapid move to starting test location =============================
  stepperX.setMaxSpeed(1000.0);      //
  stepperX.setAcceleration(5000.0);  // Set acceleration of stepper, from testing 10,000 was a good acceleration
  stepperX.runToNewPosition(testStartLocation); //This is a blocking function, such that it doesn't go to the next step until complete

  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  String partNumberFolder = String((partNumber[selectedPart][0]));

  if (!SD.exists(partNumberFolder)) {
    SD.mkdir(partNumberFolder);
    Serial.print("Made folder: ");
    Serial.println(partNumberFolder);
  }

  String partNumberAndFolder = (partNumberFolder) + "/" + UUTserialNumber + ".txt";
  myFile = SD.open(partNumberAndFolder , FILE_WRITE);
  Serial.print("Opened folder and file: ");
  Serial.println(partNumberAndFolder);

  currentMillis = millis();
  float percentComplete;
  myFile.print("User Name: ");
  myFile.println(selectedUser);
  myFile.print("Part Number: ");
  myFile.println(partNumberFolder);
  myFile.print("Serial Number: ");
  myFile.println(UUTserialNumber);
  myFile.print("Date and Time to be added: ");


  for ( int i = 0; i < testingLoopCount; i++) {
    percentComplete = ((float(i) / float(testingLoopCount)));
    colorProgess(percentComplete);

    stepperX.runToNewPosition(testStartLocation + i);
    randNumber = random(1000);
    myFile.print(i);
    myFile.print(" - ");
    myFile.println(randNumber);
  }
  long elapsedTimeMS = millis() - currentMillis;
  myFile.println(elapsedTimeMS);
  myFile.close();
  stepperX.runToNewPosition(+10);


  //code here for writing to SD card
  previousMillis = millis(); // Reset the timer





}
