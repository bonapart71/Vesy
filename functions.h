//===============Доп функции======================================

//===========================Функция, которая переводит число в строку, длиной N символов ===========================

#define MAX_STRING_WITH_SPACE 20

char buff_with_space[MAX_STRING_WITH_SPACE];

char *ltoa_with_space(long num, uint8_t n)
{
  char temp[MAX_STRING_WITH_SPACE];

  ltoa(num, temp, 10);
  for (int i = 0; i < n; i++)
    buff_with_space[i] = ' ';
  if (strlen(temp)>n or strlen(temp) >= MAX_STRING_WITH_SPACE or n >= MAX_STRING_WITH_SPACE)
    return "ERR";
  strcpy(buff_with_space + n - strlen(temp), temp);
  return buff_with_space;
}

//======================================================================

/*
char * itoa6digit(long num) {
  for (int i = 0; i < 6; i++) {
    buff2[i] = ' ';
  }
  ltoa(num, buff2 + 5, 10);
  return ( buff2 + strlen(buff2) - 6 );
}
*/

//======================================================================

//Открыть КЛАПАН
void open_valve(int valve)
{
  // Установка низкого напряжения на пин "valve"
  digitalWrite(valve, HIGH);
  if (valve==VALVE_NALIV) valve_Naliv_open=true;
  if (valve==VALVE_SLIV)  valve_Sliv_open=true;

}

//======================================================================

//Закрыть КЛАПАН
void close_valve(int valve)
{
  // Установка высокого напряжения на пин "valve"
  digitalWrite(valve, LOW);
  if (valve==VALVE_NALIV) 
  {
    valve_Naliv_open=false;
    LOG2("valve naliv - ",valve_Naliv_open);
  }
  if (valve==VALVE_SLIV)  valve_Sliv_open=false;
}

//======================================================================

// Переменные, создаваемые процессом сборки,
// когда компилируется скетч
extern int __bss_end;
extern void *__brkval;

//=========================== Функция, возвращающая количество свободного ОЗУ (RAM) ===========================
int memoryFree()
{
  int freeValue;

  if ((int)__brkval == 0)
    freeValue = ((int)&freeValue) - ((int)&__bss_end);
  else
    freeValue = ((int)&freeValue) - ((int)__brkval);
  return freeValue;
}

//=========================== EERPOM ===========================

void store_to_eeprom_long(int address, long data)
{
  EEPROM.put(address, data);
}

void store_to_eeprom_int(int address, int data)
{
  EEPROM.put(address, data);
}

long read_from_eeprom_long(int address)
{
  long data;
  EEPROM.get(address, data);
  return data;
}

int read_from_eeprom_int(int address)
{
  int data;
  EEPROM.get(address, data);
  return data;
}

//=========================== Взвешивание ===========================

//Блок для сбора N взвешиваний и вычисления средней массы. Затем перевод в граммы
void set_ves()
{

  if (schetchik_kolichestvo_vzveshivanii < work_setting.kolvo_vzveshivanii)
  {
    //LOG1("Ves");  
    //LOG1(scale.get_units());  
    new_sred_wess_N_izmer = new_sred_wess_N_izmer + scale.get_units();
    schetchik_kolichestvo_vzveshivanii++;
  }
  else
  {
    schetchik_kolichestvo_vzveshivanii = 1;
    new_sred_wess_N_izmer = new_sred_wess_N_izmer / work_setting.kolvo_vzveshivanii;
    
    //sred_wess_N_izmer = 0.035274 * new_sred_wess_N_izmer - work_setting.tara;
    sred_wess_N_izmer = new_sred_wess_N_izmer - work_setting.tara;
  };
}

//=========================== СИМУЛЯЦИЯ НАЖАТИЯ КНОПОК ИЗ МОНИТОРА ПОРТА ===========================

/*
  void  serial_button_click() {
   if (Serial.available() > 0) {

    incomingByte = Serial.read();
       if(incomingByte == '1'){
          button_Naliv_pressed=true;
       } else if (incomingByte == '2'){
          button_Sliv_pressed=true;
      } else if (incomingByte == '3'){
          button_Menu_pressed=true;
        }
    }
  }
  */

//=========================== Read Buttons ===========================

void read_buttons()
{

  if (digitalRead(BUTTON_NALIV) == LOW)
  {
    button_Naliv_pressed = true;
    delay(200);
  }

  if (digitalRead(BUTTON_SLIV) == LOW)
  {
    button_Sliv_pressed = true;
    delay(200);
  }

  //Для срабатываения кнопки меню требуется длинное нажатие.
  //Если кнопка была отжата до срабатывания таймера, останавливаем таймер и сбрасываем состояние.

  if (button_Menu_pressed and digitalRead(BUTTON_MENU) == HIGH)
  {
    button_Menu_pressed = false;
    timer_Menu.stop();
  }

  if (digitalRead(BUTTON_MENU) == LOW)
  {
    //PRNLN("Menu pressed");
    //Если кнопка "Меню" нажата, и ее статус еще false, то запускаем таймер
    
    if (!button_Menu_pressed)
    {
      timer_Menu.start(vremya_Zaderzhka_Nazhatiya_Knopki_Menu);
    }
    button_Menu_pressed = true;
    delay(200);
  }

  //И когда таймер отработал и состояние не сбросилось, запускаем Setup
  
  if (timer_Menu.timesUp() && button_Menu_pressed)
  {
    //PRNLN("Menu Long pressed");
    button_Menu_pressed = false;
    button_Menu_long_pressed = true;
  }
}

//=============================================
