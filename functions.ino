void homeStepper() {

}

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

        if (UUTserialNumber.length() == maxSerialNumberLength) {
          lcd.setCursor(0, 3);
          lcd.print("GREEN BTN TO START");
          
          
        }
        break;
    }
  }
}

void serialNumberKeyPad() {
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


