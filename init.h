
bool init_comleted()
{

  if (ves_pered_Slivom_pri_sboe == 0)
  {

    return true;
  }
  else
  {
    //PRNLN("init");
    read_buttons();

    //экран Надпись "SLIV PRERVAN" "OSTATOK:текущий вес"
    // Выбор: 1-Норм работа 2-Слить остаток 3-Доз Слив
    //действия

    if (timer_Display.timesUp() || button_Naliv_pressed || button_Sliv_pressed || button_Menu_pressed)
    {

      if (button_Naliv_pressed)
      {
        button_Naliv_pressed = false;
        punkt_menu_vybran--;
        punkt_menu_vybran = constrain(punkt_menu_vybran, 1, 2);
        lcd.clear();
      }
      else if (button_Sliv_pressed)
      {
        button_Sliv_pressed = false;
        punkt_menu_vybran++;
        punkt_menu_vybran = constrain(punkt_menu_vybran, 1, 2);
        lcd.clear();
      }

      else if (button_Menu_pressed && punkt_menu_vybran != 0)
      {

        if (punkt_menu_vybran == 2)
        {
          SD_Log("Posle sboya vybran punkt - Trebuetsy doliv ostatka", 0);
          SD_Log("Ves pered sboem", ves_pered_Slivom_pri_sboe);
          SD_Log("Tekushiy ves", sred_wess_N_izmer);
          trebuetsy_doliv_ostatka = true;
        }
        else if (punkt_menu_vybran == 1)
        {
          SD_Log("Posle sboya vybran punkt - Normalnaya rabota", 0);
          SD_Log("Ves pered sboem", ves_pered_Slivom_pri_sboe);
          SD_Log("Tekushiy ves", sred_wess_N_izmer);
          store_to_eeprom_long(EEPROM_VES_PERED_SLIVOM_PRI_SBOE, 0);
        }
        led_Alarm.off();
        lcd.clear();
        
        button_Sliv_pressed = false;
        button_Naliv_pressed = false;
        button_Menu_pressed = false;
        button_Menu_long_pressed = false;
        Sostoyanie_Alarm=NO_ALARM;
        return true;
      }

      lcd.setCursor(0, 0);
      lcd.print(menu_init_item[punkt_menu_vybran]);

      lcd.setCursor(0, 1);
      lcd.print(F("D:"));

      lcd.setCursor(2, 1);
      lcd.print(ltoa_with_space(work_setting.max_sliv - ves_pered_Slivom_pri_sboe + round(sred_wess_N_izmer), 6));

      lcd.setCursor(8, 1);
      lcd.print(F("V:"));

      lcd.setCursor(10, 1);
      lcd.print(ltoa_with_space(round(sred_wess_N_izmer), 6));
    }
    return false;
  }
}
