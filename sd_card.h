


void SD_Log(char message[], float ves)
{

  LOG1P(rtc.getDateStr())
  LOG1P(rtc.getTimeStr())
  LOG1P(message);
  //LOG1P(F("Value:"));
  LOG1P('\t');
  LOG1P(ves);
  LOG1P('\t');
  LOG1P('\t');
  LOG1P(F("Cycle:"));
  LOG1(cycle);


  if (SD_check("SD_log"))
  {
    File dataFile = SD.open("log.txt", FILE_WRITE);
    if (dataFile)
    {
      dataFile.print(rtc.getDateStr());
      dataFile.print(space);
      dataFile.print(rtc.getTimeStr());
      dataFile.print(space);
      dataFile.print(message);
      dataFile.print(space);
      //dataFile.print(F("Cycle;"));
      dataFile.println(ves);
      dataFile.print(space);
      //dataFile.print(F("Ves;"));
      dataFile.print(cycle);
      dataFile.close();
    }
    else
    {
      LOG1(F("Error opening file LOG.TXT"));
      LOGD;
      SD_works_good = false;
    }
  }
}


void SD_Log_All_Settings()
{
  SD_Log("Current settngs ---------------",0);
  SD_Log("Tara",work_setting.tara);  
  SD_Log("Max Naliv",work_setting.max_naliv);  
  SD_Log("Max Sliv",work_setting.max_sliv); 
  SD_Log("Zapas",work_setting.zapas); 
  SD_Log("Max ves utechki",work_setting.max_ves_utechki); 
  SD_Log("Current settngs end ---------------",0);
   
}



/*
//Массив для хранения даты и времени при установке текущего времени
int datetime[5];

void set_date_time_from_file()
{
 char buff[];
 char buff2[];

  File dataFile = SD.open("time.txt");
  if (dataFile)
  {
    n = 0;
    while (dataFile.available())
    {
      buff2[n] = dataFile.read();
      n++;
    }
    Serial.print("time:");
    Serial.println(buff2);

    n = 0;
    buff[2] = 0;
    for (byte i = 0; i < 5; i++)
    {
      buff[0] = buff2[n];
      buff[1] = buff2[n + 1];
      datetime[i] = atoi(buff);
      Serial.println(datetime[i]);
      n = n + 3;
    }
    rtc.setDate(datetime[2], datetime[1], 2000 + datetime[0]);
    rtc.setTime(datetime[3], datetime[4], 0);

    dataFile.close();
    SD.remove("time.txt");
  }
}
*/
