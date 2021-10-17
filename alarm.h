//=========================== Действия при тревоге ===========================

  void set_alarm(byte ALARM_TYPE)
  {
  timer_Proverki.startAndRepeat(vremya_povtoreniya_soobsheniya_ob_alarme);
  led_Sliv.off();
  led_Naliv.off();
  led_Alarm.blink();
  sound_Alarm.blink(2000);
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
