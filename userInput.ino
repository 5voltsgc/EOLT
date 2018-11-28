void userInput() {
  // Update the debouncer and get the changed state
  debouncerButtonUp.update();
  debouncerButtonDown.update();
  debouncerButtonLeft.update();
  debouncerButtonRight.update();
  debouncerButtonStart.update();  
  buttonStateUp = debouncerButtonUp.rose();
  buttonStateDown = debouncerButtonDown.rose();
  buttonStateLeft = debouncerButtonLeft.rose();
  buttonStateRight = debouncerButtonRight.rose();
  buttonStateStart = debouncerButtonStart.rose();


  if (buttonStateStart == HIGH) {
    Serial.println("Green button has been pressed");
  }
  // ====================Start Button====================
  if (buttonStateStart == HIGH  && readyToTest == true ) {
    testSensors();

  }


  // ====================Up Button====================

  if (buttonStateUp == HIGH) {
    lcdChanged = true; //this is varible is set to true anytime a varible is changed
    editingRow = 0;  //0 is for first row user, 2 part number, 3 serial number

  }

  // ====================Down Button====================
  if (buttonStateDown == HIGH) {
    lcdChanged = true; //this is varible is set to true anytime a varible is changed
    editingRow = 1;  //0 is for first row user, 2 part number, 3 serial number

  }

  // ====================Left Button====================

  if (buttonStateLeft == HIGH) {
    lcdChanged = true;


    switch (editingRow) {
      case 0:  //editing the operator line
        if (operatorCounter <= 0) {//0 is for user row
          operatorCounter = (countOperators);
        }
        userChanged = true;
        operatorCounter--;
        break;

      case 1: //editing the part number line
        if (selectedPart <= 0) {
          selectedPart = PN_ROWS - 1; //because we are looking at the index for zero number, not the count of rows
        }
        else {
          selectedPart--;
        }
        break;
    }
    delay(100);// it appears to skip a few part numbers with a short press, I still want a long press to move multiples
  }


  // ====================Right Button====================
  if (buttonStateRight) {
    lcdChanged = HIGH;
    switch (editingRow) {
      case 0:
        if (operatorCounter >= countOperators - 1 ) {
          operatorCounter = 0;
        }
        else {
          operatorCounter++;
        }
        userChanged = true;
        break;

      case 1:
        if (selectedPart >= PN_ROWS - 1) {
          selectedPart = 0;
        }
        else {
          selectedPart++;
        }
        break ;
    }
    delay(100); // it appears to skip a few part numbers with a short press, I still want a long press to move multiples
  }

  char key = keypad.getKey();

  if (key != NO_KEY) {
    editingRow = 2;
    lcdChanged = true; //this is varible is set to true anytime a varible is changed

    if (UUTserialNumber.length() > maxSerialNumberLength - 1 ) {
      UUTserialNumber = char(key);

    }
    else
    {
      UUTserialNumber += char(key); //use " += " to concatenate
    }
  }

}
