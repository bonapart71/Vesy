//!!!! Функции открытия и закрытия клапанов можно удалить для экономии

void work()
{

  //PRNLN("work");

  read_buttons();

  if (button_Menu_long_pressed && !valve_Sliv_open && !valve_Naliv_open)
  {
    button_Menu_pressed = false;
    button_Menu_long_pressed = false;
    Sostoyanie_System = NASTROYKA;
    SD_Log("Vhod v nastroyki", 0);
    punkt_menu_vybran = MENU_VYBOR_DEYSTVIYA;
    led_Naliv.off();
    led_Sliv.off();
    led_Alarm.on();
    lcd.clear();
    return;
  }

  //=========================== ПРОЦЕСС НАЛИВА ===========================
  
  if (button_Naliv_pressed || avtonaliv==AVTONALIV_ON && !trebuetsy_doliv_ostatka)
  {
    // Если вес меньше чем вес "MAX-СЛИВ" и оба клапана закрыт, то можно наливать!
    if (sistema_stabilna && led_Naliv.isOn() && !valve_Sliv_open && !valve_Naliv_open)
    {
      open_valve(VALVE_NALIV);
      valve_Naliv_open = true;
      led_Naliv.blink(200);
      led_Sliv.off();
      sistema_stabilna = false;
      timer_Zaderzhka_Na_Uspokoenie.start(vremya_na_stabilizaciya);
      SD_Log("Klapan NALIV otkryt", sred_wess_N_izmer);
    }
    button_Naliv_pressed = false;
  }
  //=========================== ПРОЦЕСС CЛИВА ===========================
  
  // Если кнопка 2(button_Sliv_pressed)нажата (старт процесса cливания) то проверить условия:
  if (button_Sliv_pressed || trebuetsy_doliv_ostatka)
  {

    if (sistema_stabilna && led_Sliv.isOn() && valve_Naliv_open == false && valve_Sliv_open == false)
    {
      open_valve(VALVE_SLIV);
      // Присвоить значение весу "ves_pered_Slivom" значение веса "sred_wess_N_izmer"
      if (trebuetsy_doliv_ostatka)
      {
        ves_pered_Slivom = ves_pered_Slivom_pri_sboe;
      }
      else
      {
        ves_pered_Slivom = sred_wess_N_izmer;
      }
      
      valve_Sliv_open = true;
      led_Sliv.blink(200);
      led_Naliv.off();
      cycle++;
      sistema_stabilna = false;
      timer_Zaderzhka_Na_Uspokoenie.start(vremya_na_stabilizaciya);

      //SD_Zapis_Wess_poslednego_naliva(round(sred_wess_N_izmer));

      store_to_eeprom_long(EEPROM_VES_PERED_SLIVOM_PRI_SBOE, round(sred_wess_N_izmer));
	  if (trebuetsy_doliv_ostatka){
		  SD_Log("Klapan SLIV otkryt. Doliv", sred_wess_N_izmer);
	  } 
		else{	
			SD_Log("Klapan SLIV otkryt", sred_wess_N_izmer);
		}
	  
	  trebuetsy_doliv_ostatka = false;
    }
    button_Sliv_pressed = false;
  }
  //=========================== ОСТАНОВ НАЛИВА ===========================

  if (sred_wess_N_izmer > work_setting.max_naliv && valve_Naliv_open == true)
  {
    close_valve(VALVE_NALIV);
    valve_Naliv_open = false;
    button_Naliv_pressed = false;
    button_Sliv_pressed = false;
    sistema_stabilna = false;
    timer_Zaderzhka_Na_Uspokoenie.start(vremya_na_stabilizaciya);
    SD_Log("Klapan NALIV zakryt", sred_wess_N_izmer);
  }

  //=========================== ОСТАНОВ СЛИВ ===========================
  
  if (sred_wess_N_izmer < ves_pered_Slivom - work_setting.max_sliv && valve_Sliv_open == true)
  {
    close_valve(VALVE_SLIV);
    valve_Sliv_open = false;
    button_Naliv_pressed = false;
    button_Sliv_pressed = false;
    sistema_stabilna = false;
    timer_Zaderzhka_Na_Uspokoenie.start(vremya_na_stabilizaciya);

    //SD_Zapis_Wess_poslednego_naliva(0);
    store_to_eeprom_long(EEPROM_CYCLE, cycle);
    store_to_eeprom_long(EEPROM_VES_PERED_SLIVOM_PRI_SBOE, 0);
    SD_Log("Klapan SLIV zakryt", sred_wess_N_izmer);
    SD_Log("Ves SLIV",ves_pered_Slivom-sred_wess_N_izmer);
  }

  //=========================== ПРОВЕРКА НА ПРОТЕКАНИЕ ===========================

  if (!sistema_stabilna && timer_Zaderzhka_Na_Uspokoenie.timesUp())
  {
    sistema_stabilna = true;
    SD_Log("Sistema stabilizirovana", sred_wess_N_izmer);
    ves_proverki = sred_wess_N_izmer;

    LOG1(F("Alarm system started"));
    if (!valve_Sliv_open && !valve_Naliv_open)
    {
      if (ves_proverki < work_setting.max_naliv)
        led_Naliv.on();
      else
        led_Naliv.off();

      if (ves_proverki > work_setting.max_sliv + work_setting.zapas)
        led_Sliv.on();
      else
        led_Sliv.off();
      timer_Proverki.startAndRepeat(vremya_proverki_na_UTECHKU);
    }
    else
      timer_Proverki.startAndRepeat(vremya_proverki_na_ZASOR);
  }

  if (timer_Proverki.timesUp() && sistema_stabilna)
  {
    LOG1(F("Alarm system check"));
    if (!valve_Sliv_open && !valve_Naliv_open)
    {
      if (ves_proverki - sred_wess_N_izmer > work_setting.max_ves_utechki)
        set_alarm(ALARM_UTECHKA_SLIV);
      else if (sred_wess_N_izmer - ves_proverki > work_setting.max_ves_utechki)
        set_alarm(ALARM_UTECHKA_NALIV);
    }
    else
    {
      if (abs(ves_proverki - sred_wess_N_izmer) < min_ves_sliva_v_period_vremeni)
      {
        if (valve_Sliv_open)
          set_alarm(ALARM_ZASOR_SLIV);
        else
          set_alarm(ALARM_ZASOR_NALIV);
      }
      else
      {
        ves_proverki = sred_wess_N_izmer;
      }
    }
  }
  // =========================== ВЫВОД НА ПЕЧАТЬ ===========================
  // =======================================================================
  // =======================================================================

  //delay(200);
  // Установка задержки () мксек

  if (timer_Display.timesUp())
  {

    PRN(F(" Цикл ВЫГРУЗ="));
    PRN(cycle);
    PRN(F("  Клапан ЗАГРУЗ_"));
    PRN(valve_Naliv_open);
    PRN(F("  Клапан ВЫГРУЗ_"));
    PRN(valve_Sliv_open);
    PRN(F(" Вес="));
    PRN(round(sred_wess_N_izmer));
    PRNLN();

    lcd.setCursor(0, 0);
    lcd.print(F("Z="));
    lcd.setCursor(2, 0);
    lcd.print(ltoa_with_space(cycle, 6));
    lcd.setCursor(10, 0);
    if (valve_Naliv_open)
      lcd.print("NALIV");
      
    else if (valve_Sliv_open)
    {
      lcd.print("SLIV ");
      lcd.setCursor(10, 1);
      //LOG1("Slivaaem. Ves:");
      //LOG1(ltoa_with_space(round(ves_pered_Slivom-sred_wess_N_izmer), 6));
      lcd.print(ltoa_with_space(round(ves_pered_Slivom-sred_wess_N_izmer), 6));
    }
    else
    {
      lcd.print("     ");
      lcd.setCursor(10, 1);
      lcd.print("      ");
    }  

    lcd.setCursor(0, 1);
    lcd.print(F("W="));
    // lcd.setCursor(5,1);
    // lcd.print(F("    "));
    lcd.setCursor(2, 1);
    lcd.print(ltoa_with_space(round(sred_wess_N_izmer), 6));
    //lcd.clrScrt();


    if (!SD_works_good)
    {
      lcd.setCursor(10, 1);
      lcd.print("SD_ERR");
    } 
    //else{
      //lcd.setCursor(9, 1);
      //lcd.print("      ");
    //}
  }
}
