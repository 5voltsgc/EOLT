/*
      _                          _       _           _  
   __| | ___ _ __  _ __ ___  ___(_) __ _| |_ ___  __| | 
  / _` |/ _ \ '_ \| '__/ _ \/ __| |/ _` | __/ _ \/ _` | 
 | (_| |  __/ |_) | | |  __/ (__| | (_| | ||  __/ (_| | 
  \__,_|\___| .__/|_|  \___|\___|_|\__,_|\__\___|\__,_| 
            |_|                                         
 */


void userInput() {
  /*Write once to the LCD to screen the fields user, Part #, Serial number
    Position the BLINKing curser in position of the user or part with the up or down buttons
    Select the user or part number with the left or right buttons - when selected write it to a varible

  */

  if (lcdChanged == true) {
    lcd.setCursor(0, 0);//Column, Row (Starts counting at 0)
    lcd.print("USER:");
    lcd.setCursor(0, 1);//Column, Row (Starts counting at 0)
    lcd.print("PART#:");
    lcd.setCursor(0, 2);//Column, Row (Starts counting at 0)
    lcd.print("SERIAL#:");
    lcdChanged = false;
    lcd.setCursor(6, 0);//Column, Row (Starts counting at 0)
    lcd.blink();
    userField = true;

  }

  buttonStateUp = digitalRead(buttonUp);
  buttonStateDown = digitalRead(buttonDown);
  buttonStateLeft = digitalRead(buttonLeft);
  buttonStateRight = digitalRead(buttonRight);

  // ====================Up Button====================

  if (buttonStateUp == HIGH) {
    lcd.setCursor(0, 3);//Column, Row (Starts counting at 0)
    lcd.print("LEFT/RIGHT FOR USER");
    lcd.setCursor(6, 0);//Column, Row (Starts counting at 0)
    lcd.blink();
    userField = true;

  }
  // ====================Down Button====================
  if (buttonStateDown == HIGH) {
    userField = false;


    lcd.setCursor(0, 3);//Column, Row (Starts counting at 0)
    lcd.print("LEFT/RIGHT FOR PART#");
    lcd.setCursor(7, 1);//Column, Row (Starts counting at 0)
    lcd.blink();
    userField = false;
  }

  // ====================Left Button====================

  if (buttonStateLeft == true) {


    if (userField == true) {
      if (operatorCounter <= 0) {
        operatorCounter = (countOperators);
      }
      operatorCounter--;
      lcd.setCursor(6, 0);  //Column, Row (Starts counting at 0)
      lcd.print("              ");
      lcd.setCursor(6, 0); //Column, Row (Starts counting at 0)
      lcd.print(operatorCounter);
      lcd.print("-");
      selectedUser = listUsers[operatorCounter];
      lcd.print(selectedUser);
      Serial.println(selectedUser);

      delay(100);

    }
    else {

      //Set the part number
      if (selectedPart <= 0) {
        selectedPart = PN_ROWS - 1; //because we are looking at the index for zero number, not the count of rows
      }
      else {
        selectedPart--;
      }
      lcd.setCursor(7, 1);  //
      lcd.print(selectedPart);
      lcd.print("-");
      lcd.print(partNumber[selectedPart][0]);
      delay(200);


    }


  }

  // ====================Right Button====================
  if (buttonStateRight) {
    //    Serial.println("Right Button");
    if (userField == true) {

      if (operatorCounter >= countOperators - 1 ) {
        operatorCounter = 0;
      }
      else {
        operatorCounter++;
      }


      lcd.setCursor(6, 0);  //Column, Row (Starts counting at 0)
      lcd.print("              ");
      lcd.setCursor(6, 0); //Column, Row (Starts counting at 0)
      lcd.print(operatorCounter);
      lcd.print("-");

      selectedUser = listUsers[operatorCounter];
      lcd.print(selectedUser);
      Serial.println(selectedUser);

      delay(100);

    }
    else {
      //Set the part number
      if (selectedPart >= PN_ROWS - 1) {
        selectedPart = 0;
      }
      else {
        selectedPart++;
      }
      lcd.setCursor(7, 1);  //
      lcd.print(selectedPart);
      lcd.print("-");
      lcd.print(partNumber[selectedPart][0]);
      delay(200);


    }

  }
  char key = keypad.getKey();

  if (key != NO_KEY) {

    Serial.println(key);
    
    
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):

    lcd.setCursor(0, 3);
    lcd.print("KEYPAD FOR SERIAL#  ");
    lcd.setCursor(colPosition + 8, 2);


    if (key == '*') {

      colPosition = 0;
      lcd.print("       ");
    }
    else if (key == ('#')) {
      
      testSensors(selectedUser, partNumber[selectedPart][0], partNumber[selectedPart][0] );
    }

    
    else {

      lcd.print(char(key));
      colPosition++;
      
    }


    if (colPosition > 5) {

      colPosition = 0;

    }

  }



}

