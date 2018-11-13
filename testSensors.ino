void testSensors() {
  if (readyToTest == true ) {
    Serial.println("Testing");
    Serial.println(selectedUser);
    Serial.println(UUTserialNumber);


    Serial.println("\n\n0-Part Numbers,1-Count Halls,2-halls/head,3-# heads,4-Selected,5-addressed,6-address lines,7-# analog lines,8-highMax,9-highMin,10-lowMax,11-lowMin,12-diffMax,13-diffLow");

    for (int i = 0; i < PN_COLS - 1; i++) {

      Serial.print(partNumber[selectedPart][i]);
      Serial.print(", ");
    }

    //=====================Main Loop Testing=================
    int ml;
    int hds;
    int hls;
    int thalls;
    int totalHeads = partNumber[selectedPart][3];
    int totalHalls = partNumber[selectedPart][1];
    int hallsPerHead = partNumber[selectedPart][2];
    int highMax = partNumber[selectedPart][8];
    int highMin = partNumber[selectedPart][9];





    Serial.print("\ntotal heads = ");
    Serial.println(totalHeads);
    Serial.print("Halls per Head = ");
    Serial.println(hallsPerHead);
    Serial.print("total halls = ");
    Serial.println(totalHalls);
    Serial.print(" max: ");
    Serial.println(highMax);
    Serial.print(" min: ");
    Serial.println(highMin);



    for (ml = 0; ml < testingLoopCount; ml++) {

      //=====================head Loop Testing=================

      for (thalls = 0; thalls < totalHalls; thalls++) {



        Serial.print(thalls);
        Serial.print(" % ");
        Serial.print(totalHeads); //Zero indexed
        Serial.print(" = ");
        Serial.print(thalls % totalHeads);
        Serial.print("<- Head Number  :");
        Serial.print(thalls);
        Serial.print(" % ");
        Serial.print(totalHalls);
        Serial.print(" = ");
        Serial.print(thalls % hallsPerHead);
        Serial.println(" <-  Hall number ");


      }
      Serial.print("Main Loop Count: ");
      Serial.println(ml);
      Serial.println(testingLoopCount);
      int percentComplete = (float(ml+1) / float(testingLoopCount))*100;
      Serial.print(percentComplete);
      Serial.println("% Complete");
    }

    UUTserialNumber = "";
    readyToTest = false;
    lcdChanged = true;
    Serial.println("Done");



    delay(10000);





  }

}
