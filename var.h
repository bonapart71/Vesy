
//=========================== PIN ===========================

// Кнопка НАЛИВ  (D35)
#define BUTTON_NALIV 35

// Кнопка СЛИВ  (D39)
#define BUTTON_SLIV 39

// Кнопка МЕНЮ  (D37)
#define BUTTON_MENU 37

// Клапан НАЛИВА (D25)
#define VALVE_NALIV 25

// Клапан СЛИВ   (D27)
#define VALVE_SLIV 27

// SD pin-CS
#define CHIP_SELECT 10

//=========================== Лампочки ===========================

#define LED_NALIV 33
#define LED_SLIV 31
#define LED_ALARM 29

//=========================== Пин для звукойвой сигнализации ===========================

#define SOUND_ALARM 41

//=========================== АЦП HX711(TENZO) A0-DT, A1-SCK ===========================

HX711 scale(A0, A1);

//=========================== Модуль реального времени ===========================

DS3231 rtc(SDA, SCL);

//=========================== СОСТОЯНИЕ КНОПОК ===========================

bool button_Naliv_pressed = false;
bool button_Sliv_pressed = false;
bool button_Menu_pressed = false;
bool button_Menu_long_pressed = false;

//=========================== СОСТОЯНИЕ КЛАПАНОВ ===========================

bool valve_Naliv_open = false;
bool valve_Sliv_open = false;

//=========================== Переменная для долива ===========================
bool trebuetsy_doliv_ostatka = false;

//=========================== Множитель для параметра с плавающей запятой, что-бы хранить его как Long ===========================

#define CALIBRATION_FACTOR_MULTIPLIER 10000

const char SettingsFilename_new[] = "set.txt";
const char SettingsFilename_current[] = "config.txt";

//=========================== Количество настроек ===========================
#define SETTING_COUNT 7

//=========================== Названия параметров в файле настроек ===========================
char *setting_names[SETTING_COUNT]{
    "tara",
    "calibration_factor",
    "max_naliv",
    "max_sliv",
    "zapas",
    "max_ves_utechki",
    "kolvo_vzveshivanii"};

//=========================== Адреса параметров в EEPROM ===========================

#define EEPROM_STATUS 0
#define EEPROM_STATUS2 2
#define EEPROM_CYCLE 4
#define EEPROM_TARA 8
#define EEPROM_CALIBRATION_FACTOR 12
#define EEPROM_MAX_NALIV 16
#define EEPROM_MAX_SLIV 20
#define EEPROM_ZAPAS 24
#define EEPROM_MAX_VES_UTECHKI 28
#define EEPROM_KOLVO_VZVESHIVANIY 32
#define EEPROM_VES_PERED_SLIVOM_PRI_SBOE 36
// #define EEPROM_next_empty 36

enum settings_field
{
  TARA,
  CALIBRATION_FACTOR,
  MAX_NALIV,
  MAX_SLIV,
  ZAPAS,
  MAX_VES_UTECHKI,
  KOLVO_VZVESHIVANIY
};

struct setting_data
{
  long tara = 0;
  float calibration_factor = 0;
  long max_naliv = 0;
  long max_sliv = 0;
  long zapas = 0;
  long max_ves_utechki = 0;
  int kolvo_vzveshivanii = 0;
};

// struct setting_data
// {
// long tara = 330;
// float calibration_factor = -3.4564;
// long max_naliv = 120;
// long max_sliv = 330;
// long zapas = 50;
// long max_ves_utechki = 20;
// int kolvo_vzveshivanii = 10;
// };

//=========================== Структуры данных для хранения настроек ===========================

setting_data new_setting;
setting_data work_setting;

//=========================== СОСТОЯНИЯ СИСТЕМЫ ===========================

enum Sostoyaniya_Sistemy_
{
  OTKLUCHENO,
  INIZIALIZACIYA_DOLIV,
  NASTROYKA,
  RABOTA,
  ALARM,
};

enum Sostoyaniya_Alarm_
{
  NO_ALARM,
  ALARM_UTECHKA_NALIV,
  ALARM_UTECHKA_SLIV,
  ALARM_ZASOR_NALIV,
  ALARM_ZASOR_SLIV,
};

//=========================== АВТОНАЛИВ ===========================

enum Sostoyaniya_Avtonaliv_
{
  AVTONALIV_OFF,
  AVTONALIV_ON,
};

//int avtonaliv=AVTONALIV_ON;
int avtonaliv=AVTONALIV_OFF;

//=========================== Назначенные Задержки ===========================

#define vremya_na_stabilizaciya 3000
//++сделать больше потом!!!!
#define vremya_povtoreniya_soobsheniya_ob_alarme 10000
#define vremya_proverki_na_UTECHKU 26000
#define vremya_proverki_na_ZASOR 26000
#define vremya_obnovleniya_DISPLAY 2000
#define vremya_Zaderzhka_Na_Uspokoenie 10000
#define vremya_Zaderzhka_Nazhatiya_Knopki_Menu 3000

//======== Константы
/*
// коэффициент калибровки!
const float calibration_factor = -3.9552;
//float calibration_factor = -0.9452;


// вес максимального налива
const long max_Naliv = 200;

// вес максимального СЛИВ
const long max_Sliv = 95;

// Уровень разрешенного СЛИВ должен превышать вес максимального СЛИВ + некая доп величина. Вот это и есть эта величина.
const long zapas_max_Sliv = 10;

// Вес утечки в большую или меньшую сторону на который  будет срабатывать ТРЕВОГА (при закрытых клапанах)
const long max_ves_utechki = 20;

// (количество взвешиваний для усреднения)
const int max_kolichestvo_vzveshivanii = 3;
*/

// Минимальное количество жидкости, которое должно слиться в период времени заданный
// в timer_Proverki константой vremya_proverki_na_ZASOR
//++Может перевести в еепром?
long min_ves_sliva_v_period_vremeni = 20;
//long min_ves_sliva_v_period_vremeni = 2;

//=========================== ПЕРЕМЕННЫЕ ===========================

byte Sostoyanie_System;
byte ALARM_TYPE;
bool SD_works_good = true;

//=========================== для управления из ком-порта ===========================

uint8_t incomingByte;

//=========================== Средний вес за последние N измерений (N=max_kolichestvo_vzveshivanii) ===========================

float sred_wess_N_izmer = 0;

//=========================== Сумма весов за N последних измерений (перед делением на количество измерений) ===========================

float new_sred_wess_N_izmer = 0;

//=========================== Вес объекта перед сливом жидкости ===========================

float ves_pered_Slivom;

//=========================== Вес жидкости сохраненной на флешке перед сбоем эклектричества ===========================

long ves_pered_Slivom_pri_sboe;

//=========================== Вес проверки???????????podumat nad nazvaniem????????????????????????===========================

float ves_proverki = 0;

//=========================== Счетчик количества взвешиваний для усреднения веса ===========================

int schetchik_kolichestvo_vzveshivanii = 0;
int schetchik_proverki = 0;

//=========================== Счетчик количества сливов ===========================

long cycle;

//=========================== Таймеры-Лампочки ===========================

BonTimer timer_Zaderzhka_Na_Uspokoenie;
BonTimer timer_Proverki;
BonTimer timer_Display;
BonTimer timer_Menu;
BonTimer timer_Ruchnoy_Sliv;
BonLed led_Naliv(LED_NALIV), led_Sliv(LED_SLIV), led_Alarm(LED_ALARM), sound_Alarm(SOUND_ALARM);

bool Ruchnoy_Sliv = false;

const char space = ';';

//=====================Время слива===============

unsigned long Sliv_Start_Time;
unsigned long Sliv_Time;


//=========================== Статутс - "система стабильна". После закрытия клапанов даем системе некоторое время успокоиться, а потом включаем систему тревог.
bool sistema_stabilna;

LiquidCrystal_I2C lcd(0x27, 20, 4);
