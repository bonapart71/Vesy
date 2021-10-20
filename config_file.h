
void save_settings_to_eeprom()
{
long temp;
  work_setting = new_setting;
  
//11111 -Магическое число. Если оно есть в первом адресе еепром, значит настройки были сохранены
//Иначе - карточка новая, и первым делом нужно идти в настройки.

  store_to_eeprom_int(EEPROM_STATUS, 11111);
  store_to_eeprom_long(EEPROM_TARA, new_setting.tara);
  temp = round(new_setting.calibration_factor * CALIBRATION_FACTOR_MULTIPLIER);
  store_to_eeprom_long(EEPROM_CALIBRATION_FACTOR, temp);
  store_to_eeprom_long(EEPROM_MAX_NALIV, new_setting.max_naliv);
  store_to_eeprom_long(EEPROM_MAX_SLIV, new_setting.max_sliv);
  store_to_eeprom_long(EEPROM_ZAPAS, new_setting.zapas);
  store_to_eeprom_long(EEPROM_MAX_VES_UTECHKI, new_setting.max_ves_utechki);
  store_to_eeprom_long(EEPROM_KOLVO_VZVESHIVANIY, new_setting.kolvo_vzveshivanii);
  LOG1("Setting stored to EEPROM");
  LOGD;
}

int read_setting_from_eeprom()
{
long temp;

  work_setting.tara = read_from_eeprom_long(EEPROM_TARA);
  temp =read_from_eeprom_long(EEPROM_CALIBRATION_FACTOR) ;
  work_setting.calibration_factor =  temp ;
  work_setting.calibration_factor /= CALIBRATION_FACTOR_MULTIPLIER;
  work_setting.max_naliv = read_from_eeprom_long(EEPROM_MAX_NALIV);
  work_setting.max_sliv = read_from_eeprom_long(EEPROM_MAX_SLIV);
  work_setting.zapas = read_from_eeprom_long(EEPROM_ZAPAS);
  work_setting.max_ves_utechki = read_from_eeprom_long(EEPROM_MAX_VES_UTECHKI);
  work_setting.kolvo_vzveshivanii = read_from_eeprom_int(EEPROM_KOLVO_VZVESHIVANIY);
  LOG1("Setting read from EEPROM");
  LOGD;
  return read_from_eeprom_int(EEPROM_STATUS);
}

void print_settings(setting_data setting)
{
  LOG2EQ(setting_names[TARA], setting.tara);
  LOG2EQ(setting_names[CALIBRATION_FACTOR], setting.calibration_factor);
  LOG2EQ(setting_names[MAX_NALIV], setting.max_naliv);
  LOG2EQ(setting_names[MAX_SLIV], setting.max_sliv);
  LOG2EQ(setting_names[ZAPAS], setting.zapas);
  LOG2EQ(setting_names[MAX_VES_UTECHKI], setting.max_ves_utechki);
  LOG2EQ(setting_names[KOLVO_VZVESHIVANIY], setting.kolvo_vzveshivanii);
}


//Функция для определения строк с комментариями или пустых строк
bool string_is_comment(char buff[])
{
  for (uint8_t i = 0; i < strlen(buff); i++)
  {
    if (buff[i] == ' ' or buff[i] == 9 or buff[i] == '\r')
      continue; //пробел или табуляция, перевод каретки - пропускаем символ
    else
    {
      if (buff[i] == '#')
        return true; // Если первый найденый символ это знак комментария, значит строку можно игнорировать - возвращаем true
      else
        return false; // Если первый найденый символ это НЕ знак комментария, значит строку обрабатывать - возвращаем false
    }
  }
  return true; //Если не нашли не одного символа, кроме пробелов значит строка пустая  - возвращаем true
}

bool parse_string(char buff[])
{
  char *n;
  long temp;
  if (string_is_comment(buff))
    return true;

  for (uint8_t i = 0; i < SETTING_COUNT; i++)
  {
    if (strstr(buff, setting_names[i]))
    {                        //Сравниваем ислледуемую cо всеми возможными именами настроек. И если нашли то:
      n = strchr(buff, '='); //Ищем '=' в строке.  Возвращается ссылка на положение символа
      if (n == NULL)
      { //Символ '=' должен быть. Если его нет - возвращаем ошибку
        LOG1("Not found '=' in string");
        return false;
      }
      n++; //Переводим ссылку на следующий символ после '='. Это будет наша подстрока с числом.
      switch (i)
      { // В зависимости от совпадения с именем настройки, выбираем какой параметр обновляем
      case TARA:
        new_setting.tara = atol(n);
        break;
      case CALIBRATION_FACTOR: //Число в конфиг файле хранится в целочисленном формате. После конвертации делим на CALIBRATION_FACTOR_MULTIPLIER для получения коэффициента калибровки
        temp = atol(n);
        new_setting.calibration_factor = temp;
        new_setting.calibration_factor /= CALIBRATION_FACTOR_MULTIPLIER;
        break;
      case MAX_NALIV:
        new_setting.max_naliv = atol(n);
        break;
      case MAX_SLIV:
        new_setting.max_sliv = atol(n);
        break;
      case ZAPAS:
        new_setting.zapas = atol(n);

        break;
      case MAX_VES_UTECHKI:
        new_setting.max_ves_utechki = atol(n);
        break;
      case KOLVO_VZVESHIVANIY:
        new_setting.kolvo_vzveshivanii = atoi(n);
        break;
      case 8:
        //Put more data to setting names and use more case
        break;
      }
      return true;
    }
  }
  LOG1("No config name found in line");
  return false;
}

//=========================== ===========================

//Длина буфера строки
#define BUFF_LEN_CONFIG 150

/*Функция считывает строки из файла конфигураций, парсит их и если все ок, заносит данные в work_setting. И возвращает 0
Если есть ошибка в строке файла - возвращаем номер строки (work_setting не меняем)
Если ошибка открытия файла - возвращаем -1 */
int read_strings_from_file()
{
  char buff[BUFF_LEN_CONFIG];
  uint8_t buff_len;
  int string_count;
  char ch;
  
  LOG1(F("Trying reag new config"));

  File datafile = SD.open(SettingsFilename_new);
  if (datafile)
  {
    LOG2("File open:", SettingsFilename_new);
	LOGD;
    string_count = 0;
    strcpy(buff, "");
    while (datafile.available())
    {

      ch = datafile.read();
      if (ch == '\n')
      {
        string_count++;
        if (!parse_string(buff))
        { 
  //Если произошла ошибка парсинга, закрываем файл, возвращаем номер строки с ошибкой (первая строка имеет индекс 1)
          datafile.close();
          return string_count;
        }
        buff[0] = 0; //после парсинга чистим буффер
      }
      else
      {
        buff_len = strlen(buff);
        if (buff_len < BUFF_LEN_CONFIG - 1)
        {
          buff[buff_len] = ch;
          buff[buff_len + 1] = 0;
        }
        else
        {
          LOG2(F("Error.Exceed maximum size of line:"), BUFF_LEN_CONFIG - 1);
          return string_count;
        }
      }
    } //end while
  }
  else
  {
    return -1; //не смогли открыть файл
  }

  datafile.close();
  LOG1(F("Configuration succesfully read from file"));
  LOGD;
	
  LOG1(F("New settings from file:")); 	
  print_settings(new_setting);
  LOGD;
  
  //Если все строки обработаны без ошибок, обновляем текущие параметры, записываем их ееprom
  //и возвращаем 0.
  
  save_settings_to_eeprom();
  
  LOG1(F("New settings from file:"));
  print_settings(work_setting);
  LOGD;
  return 0;
}


#define SD_alarm_led_blink_timeout 1501

/*Функция инициализирует SD карту. 
Делаем это перед каждым обращением к карточке, на случай если карта вытащена или сломалась*/
bool SD_check(char message[])
{

  if (SD.begin(CHIP_SELECT))
  {
    //LOG1(F("SD card initialized at step:"));
    //LOG1(message);
    //LOGD;
    
    SD_works_good=true;
    if (led_Alarm.timeout()==SD_alarm_led_blink_timeout){
      led_Alarm.off();
    }
    return true;
  }
  else
  {
    LOG1(F("Card failed, or not present. At step:"));
    LOG1(message);
    LOGD;
    led_Alarm.blink(SD_alarm_led_blink_timeout);
    SD_works_good=false;
    return false;
  }
}

//=========================== Чтение конфига из файла настроек ===========================

bool read_settings_from_file()
{
  int error_line;
  if (SD_check("Read conf"))
  {
    error_line = read_strings_from_file();
    if (error_line == 0)
    {
		
      return true;
    }
    else if (error_line == -1)
    {
      LOG2("Error open file:", SettingsFilename_new);
    }
    else
    {
      LOG2("Error in line:", error_line);
    }
  }

  //+++Show error on screen or return in func
  return false;
}



//=========================== Функция для сохранения текущих настроек на SD карту ===========================
void save_settings_to_file()
{
  if (SD_check("Save conf"))
  {
    //Удаляем существующий файл настроек
    SD.remove(SettingsFilename_current);
    File datafile2 = SD.open(SettingsFilename_current, FILE_WRITE);
    if (datafile2)
    {
      // LOGD;
      // LOG2("File created:", SettingsFilename_current);
      datafile2.println(F("# This file contains the current system settings."));
      datafile2.print(F("# Rename this file to '"));
      datafile2.print(SettingsFilename_new);
      datafile2.println(F("', change the required settings and upload it back to the system."));

      datafile2.print(F("# Note! Multiply 'calibration_factor' by "));
      datafile2.print(CALIBRATION_FACTOR_MULTIPLIER);
      datafile2.println(F(" and save as an integer to the file."));
      datafile2.println(F("# Example: 1234 means 0.1234, 44444 means 4.4444"));

      datafile2.print(F("calibration_factor = "));
      long cf_long = round(work_setting.calibration_factor * CALIBRATION_FACTOR_MULTIPLIER);
      datafile2.println(cf_long);

      datafile2.print(F("tara = "));
      datafile2.println(work_setting.tara);

      datafile2.print(F("max_naliv = "));
      datafile2.println(work_setting.max_naliv);

      datafile2.print(F("max_sliv = "));
      datafile2.println(work_setting.max_sliv);

      datafile2.print(F("zapas = "));
      datafile2.println(work_setting.zapas);

      datafile2.print(F("max_ves_utechki = "));
      datafile2.println(work_setting.max_ves_utechki);

      datafile2.print(F("kolvo_vzveshivanii = "));
      datafile2.println(work_setting.kolvo_vzveshivanii);

      datafile2.println(F("#End config file"));
      datafile2.println("");
      datafile2.close();
      LOG2("Setting succesfully store to: ", SettingsFilename_current);
      LOGD;
    }
    else
    {
      //+++Show error on screen or return in func
    }
  }
}
