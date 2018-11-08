void userInput() {
  /*Write once to the LCD to screen the fields user, Part #, Serial number
    Position the BLINKing curser in position of the user or part with the up or down buttons
    Select the user or part number with the left or right buttons - when selected write it to a varible

  */

  buttonStateUp = digitalRead(buttonUp);
  buttonStateDown = digitalRead(buttonDown);
  buttonStateLeft = digitalRead(buttonLeft);
  buttonStateRight = digitalRead(buttonRight);

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

  if (buttonStateLeft == true) {
    lcdChanged = true;

    switch (editingRow) {
      case 0:
        if (operatorCounter <= 0) {//0 is for user row
          operatorCounter = (countOperators);
        }
        userChanged = true;
        operatorCounter--;

      case 1:
        if (selectedPart <= 0) {
          selectedPart = PN_ROWS - 1; //because we are looking at the index for zero number, not the count of rows
        }
        else {
          selectedPart--;
        }
        delay(100);
        break;
    }
  }


  // ====================Right Button====================
  if (buttonStateRight) {
    lcdChanged = true;
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
        delay(100); // it appears to skip a few part numbers with a short press, i still want a long press to move multiples
        break ;
    }
  }
}

