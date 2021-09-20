// Пункт меню
uint8_t punkt_menu_vybran = 0;

// Меню 1
const char *const menu_init_item[] = {
    "SLIV PRERVAN",
    "1-NORM RABOTA",
    "2-SLIV OSTATOK"};

// Меню 2
enum menu_setup_
{
  MENU_VYBOR_DEYSTVIYA,
  MENU_RUCHNOY_SLIV,
  MENU_TARA,
  MENU_TO_EEPR,
  MENU_TO_FILE,
  MENU_EXIT
};

const char *const menu_setup_item[] = {
    "VYBOR DEYSTVIYA",
    "1-RUCHNOY SLIV",
    "2-TARA:",
    "3-SETTNG TO EEPR",
    "4-SETTNG TO FILE:",
    "5-EXIT",
};



bool setup_comleted()
{
  // Serial.println(F("Setup started"));

  //Если идет аварийный слив, и таймер вышел, закрываем клапан
  //Нажатая кнопка слива каждый цикл продляет таймер и он не заканчивается.
  //Если кнопку отжали, таймер держит открытым клапан в течении секунды
  if (timer_Ruchnoy_Sliv.timesUp() && Ruchnoy_Sliv)
  {
    Ruchnoy_Sliv = false;
    close_valve(VALVE_SLIV);
    led_Sliv.off();
    SD_Log("Ruchnoy sliv ostanovlen", sred_wess_N_izmer);
  }

  read_buttons();

  if (timer_Display.timesUp() || button_Naliv_pressed || button_Sliv_pressed || button_Menu_pressed)
  {
    if (button_Naliv_pressed)
    {
      button_Naliv_pressed = false;
      punkt_menu_vybran--;
      punkt_menu_vybran = constrain(punkt_menu_vybran, MENU_RUCHNOY_SLIV, MENU_EXIT);
      lcd.clear();
    }
    else if (button_Sliv_pressed)
    {
      button_Sliv_pressed = false;
      punkt_menu_vybran++;
      punkt_menu_vybran = constrain(punkt_menu_vybran, MENU_RUCHNOY_SLIV, MENU_EXIT);
      lcd.clear();
    }
    else if (button_Menu_pressed)
    {
      //button_Menu_pressed = false;
      if (punkt_menu_vybran == MENU_VYBOR_DEYSTVIYA)
      {
        button_Menu_pressed = false;
      }
      //===========Ручной слив
      else if (punkt_menu_vybran == MENU_RUCHNOY_SLIV)
      {
        button_Menu_pressed = false;
        //lcd.clear();
        //Включаем таймер ручного слива. Если кнопку удерживать, каждый цикл таймер запускается заново
        timer_Ruchnoy_Sliv.start(1000);
        //Если статус ручного слива не установлен, устанавливаем его и открываем клапан.
        if (!Ruchnoy_Sliv)
        {
          Ruchnoy_Sliv = true;
          open_valve(VALVE_SLIV);
          led_Sliv.blink();
          SD_Log("Zapushen ruchnoy sliv", sred_wess_N_izmer);
          delay(100);
        }
      }
      //=============Тарирование
      else if (punkt_menu_vybran == MENU_TARA && button_Menu_long_pressed == true)
      {
        button_Menu_pressed = false;
        button_Menu_long_pressed = false;
        SD_Log("Zapusheno tarirovanie", 0);
        SD_Log("Tekushiy ves", sred_wess_N_izmer);
        SD_Log("Tekushaya tara", work_setting.tara);
        work_setting.tara = sred_wess_N_izmer + work_setting.tara;
        store_to_eeprom_long(EEPROM_TARA, work_setting.tara);
        SD_Log("Novaya tara", work_setting.tara);
        delay(2000);
      }

      else if (punkt_menu_vybran == MENU_TO_EEPR && button_Menu_long_pressed == true)
      {
        SD_Log("Zapusheno zagruzka nastroek iz fayla", 0);

        if (read_settings_from_file())
        {
          button_Menu_pressed = false;
          button_Menu_long_pressed = false;
          SD_Log("Zagruzka nastroek zavershena", 0);
		  
		  SD_Log_All_Settings();
          //+++Печать настроек в лог!!!
          
		  lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("OK");
          delay(3000);
        }
        else
        {
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("FAIL!!!!");
          delay(3000);
        }
      }
      else if (punkt_menu_vybran == MENU_TO_FILE && button_Menu_long_pressed == true)
      {
        button_Menu_pressed = false;
        button_Menu_long_pressed = false;
        SD_Log("Zapushenа pechat nastroek v fayl", 0);
        save_settings_to_file();
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("OK");
        delay(3000);
      }

      //=============Выход из меню

      else if (punkt_menu_vybran == MENU_EXIT)
      {
        button_Sliv_pressed = false;
        button_Naliv_pressed = false;
        button_Menu_pressed = false;
        button_Menu_long_pressed = false;
        
        led_Alarm.off();
        led_Naliv.off();
        led_Sliv.off();
        punkt_menu_vybran = 0;
        sistema_stabilna = false;
        lcd.clear();
        return true;
      }
    }
    lcd.setCursor(0, 0);
    lcd.print(F("NASTROYKA"));

    lcd.setCursor(0, 1);
    lcd.print(menu_setup_item[punkt_menu_vybran]);

    //Если выбран пункт Тарирование, то выводим на экран текущий вес.
    if (punkt_menu_vybran == 2)
    {
      lcd.setCursor(8, 1);
      lcd.print(ltoa_with_space(round(sred_wess_N_izmer), 6));
    }
  }
  return false;
}
