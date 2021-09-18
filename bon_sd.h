

void SD_Log(char message[], float ves)
{

  LOG1P(rtc.getDateStr())
  LOG1P(rtc.getTimeStr())
  LOG1P(message);
  LOG1P(F("Cycle:"));
  LOG1P(cycle);
  LOG1P(F("Ves:"));
  LOG1(ves);

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
      dataFile.print(F("Cycle:"));
      dataFile.print(cycle);
      dataFile.print(space);
      dataFile.print(F("Ves:"));
      dataFile.println(ves);
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

// Действия при тревоге
void set_alarm(byte ALARM_TYPE)
{
  timer_Proverki.startAndRepeat(vremya_povtoreniya_soobsheniya_ob_alarme);
  led_Sliv.off();
  led_Naliv.off();
  led_Alarm.blink();
  close_valve(VALVE_NALIV);
  close_valve(VALVE_SLIV);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ALARM!!!");
  lcd.setCursor(0, 1);

  switch (ALARM_TYPE)
  {
  case ALARM_ZASOR_NALIV:
    lcd.print("ZASOR! NALIV!");
    SD_Log("ALARM ZASOR! NALIV!", sred_wess_N_izmer);
    break;
  case ALARM_ZASOR_SLIV:
    lcd.print("ZASOR! SLIV!");
    SD_Log("ALARM ZASOR! SLIV!", sred_wess_N_izmer);
    break;
  case ALARM_UTECHKA_NALIV:
    lcd.print("UTECHKA! NALIV!");
    SD_Log("ALARM UTECHKA! NALIV!", sred_wess_N_izmer);
    break;
  case ALARM_UTECHKA_SLIV:
    lcd.print("UTECHKA! SLIV!");
    SD_Log("ALARM UTECHKA! SLIV!", sred_wess_N_izmer);
    break;
  }

  //++СДЕЛАТЬ СИРЕНУ!!!
  Sostoyanie_System = ALARM;
}
