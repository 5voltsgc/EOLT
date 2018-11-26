void testSensors() {  //this is called at the userInput start button


  timedOut = false;
  homeStepper();

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

  myFile = SD.open("test.txt", FILE_WRITE);
  for ( int i = 0; i < testingLoopCount; i++) {
    stepperX.runToNewPosition(testStartLocation + i);
    randNumber = random(1000);
    myFile.println(randNumber);
  }
  myFile.close();
  stepperX.runToNewPosition(+10);
  Serial.println("Test done - should not start ugain until green button pressed");

  //code here for writing to SD card

  delay(800);
  digitalWrite(disableStepperDriverPin, HIGH); // High disables the stepper
  homed = false;


}
