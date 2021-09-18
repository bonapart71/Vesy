// ====== БИБЛИОТЕКИ ======================
//test2
//test3

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <DS3231.h>
#include "HX711.h"
#include <EEPROM.h>

#include <ModbusRtu.h>
#include <TimerOne.h>

#include "def.h"
#include "BonTimer.h"
#include "BonLed.h"
#include "var.h"
#include "modbus.h"

#include "functions.h"
#include "config_file.h"
#include "bon_sd.h"
#include "setup.h"
#include "init.h"

#include "work.h"

//=========================== SETUP ======================================================================
void setup()
{
  Serial.begin(9600);

  //Инициализируем модбас.
  Timer1.initialize(50000);                              //  Инициализация таймера 1, период 500 мкс
  Timer1.attachInterrupt(Modbus_Timer_Interrupt, 50000); //  Задаем обработчик прерываний
  MODBUS_SERIAL.begin(9600);
  ModbusSlave.start();

  //Инициализируем часы
  rtc.begin();

  //Инициализируем экран
  lcd.init();
  lcd.backlight();

  //По умолчанию начальное состояние системы - Инициализация.
  Sostoyanie_System = INIZIALIZACIYA;
  //Sostoyanie_System = NASTROYKA;

  //Считываем настройки из EEPROM. Если в первой ячейке нет 11111, значит нужно идти в настройки и сохранить их в еепром из файла
  if (read_setting_from_eeprom() == 11111)
    print_settings(work_setting);
  else
    Sostoyanie_System = NASTROYKA;

  //Можно установить нужный цикл следующей командой. Для первоначальных настроек
  //store_to_eeprom_long(EEPROM_CYCLE , 17);
  cycle = read_from_eeprom_long(EEPROM_CYCLE);

  //В случае сбоя питания во время последнего цикла налива, переменная из EEPROM будет
  //отлична от нуля и равна весу системы перед последним циклом налива
  //Включаем мигание лампы Аларм. Обработка будет производиться в модуле инициализации
  ves_pered_Slivom_pri_sboe = read_from_eeprom_long(EEPROM_VES_PERED_SLIVOM_PRI_SBOE);

  LOG1("Ves Pered slivom");
  LOG1(ves_pered_Slivom_pri_sboe);

  if (ves_pered_Slivom_pri_sboe != 0)
  {
    led_Naliv.off();
    led_Sliv.off();
    led_Alarm.blink();
    SD_Log("Sliv prervan!", 0);
    SD_Log("Ves pered slivom pri sboe:", ves_pered_Slivom_pri_sboe);
  }

  //++Установка точного времени:
  //set_date_time_from_file();

  // rtc.setTime(20,41,30);
  /*PRN(F("Time:"));
  PRN(rtc.getDateStr());
  PRN(space);
  PRN(rtc.getTimeStr());
  PRN(space);
  */

  pinMode(BUTTON_NALIV, INPUT_PULLUP);
  pinMode(BUTTON_SLIV, INPUT_PULLUP);
  pinMode(BUTTON_MENU, INPUT_PULLUP);
  pinMode(VALVE_NALIV, OUTPUT);
  pinMode(VALVE_SLIV, OUTPUT);
  digitalWrite(VALVE_NALIV, LOW);
  digitalWrite(VALVE_SLIV, LOW);

  // Установка калибровочного коэффициента
  scale.set_scale();

  //+++ Обнуляем вес тары
  //scale.tare();

  // Применяем калибровку
  scale.set_scale(work_setting.calibration_factor);

  //Система включена. Вывод в лог.
  SD_Log("Sistema vkluchena", sred_wess_N_izmer);

  //Запускаем таймеры
  timer_Zaderzhka_Na_Uspokoenie.start(vremya_Zaderzhka_Na_Uspokoenie);
  timer_Display.startAndRepeat(vremya_obnovleniya_DISPLAY);
}

// ===========================LOOP================================================================================
void loop()
{
  //Обновляем регистры Модбас
  Modbus_Update_Registers();

  // Для моргания лампочек
  led_Naliv.work(millis());
  led_Sliv.work(millis());
  led_Alarm.work(millis());

  //==========Взвешиваем===============
  set_ves();

  // ===========================Состояние РАБОТА ====================================================

  if (Sostoyanie_System == RABOTA)
  {
    work();
  }

  //============================Состояние НАСТРОЙКА==================================================

  else if (Sostoyanie_System == NASTROYKA)
  {
    if (setup_comleted())
    {
      Sostoyanie_System = RABOTA;
      led_Naliv.off();
      led_Sliv.off();
      lcd.clear();
    }

    //============================Состояние ИНИЦИАЛИЗАЦИЯ==============================================
  }
  else if (Sostoyanie_System == INIZIALIZACIYA)
  {
    if (init_comleted())
    {
      Sostoyanie_System = RABOTA;
      led_Naliv.off();
      led_Sliv.off();
      lcd.clear();
    }

    //============================Состояние ТРЕВОГА==================================================
  }
  else if (Sostoyanie_System == ALARM)
  {
    if (timer_Proverki.timesUp())
    {
      SD_Log("Zhdem...", sred_wess_N_izmer);
      LOG1(F("Zhdem..."));
      LOGD;
    }
  }
}
