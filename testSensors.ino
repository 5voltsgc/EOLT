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
    int totalHeads = partNumber[selectedPart][3];
    int totalHalls = partNumber[selectedPart][2];



    Serial.print("total heads = ");
    Serial.println(totalHeads); 
    Serial.print("total halls = ");
    Serial.println(totalHalls);


    for (ml = 0; ml < testingLoopCount; ml++) {

      //=====================head Loop Testing=================

      for (hds = 0; hds < totalHeads; hds++) {
        //        Serial.print("head Loop Count: ");
        //        Serial.println(hds);


        //=====================Halls Loop Testing=================
        for (hls = 0; hls < totalHalls; hls++) {
          Serial.print(hds);
          Serial.print(" * ");
          Serial.print(totalHeads-1);//Zero indexed
          Serial.print(" + ");
          Serial.print(hds);
          Serial.print(" = ");
          Serial.println((hds * (totalHeads-1)) + hls);




//          Serial.print(hds * totalHeads + hls);
//          Serial.print(", ");

        }
      }
      Serial.print("Main Loop Count: ");
      Serial.println(ml);
    }
   UUTserialNumber = "";
   readyToTest = false;






  }

}
