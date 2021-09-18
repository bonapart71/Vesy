//========Настройки отладки==========================

#define DEBUG

#ifdef DEBUG
#define PRN(x) Serial.print(x)
#else
#define PRN(x)
#endif

#ifdef DEBUG
#define PRNLN(x) Serial.println(x)
#else
#define PRNLN(x)
#endif

#ifdef DEBUG
#define PRN2(x, y) Serial.print(x);Serial.println(y)
#else
#define PRN2(x)
#endif






#define LOG1(x) Serial.println(x)
#define LOG1P(x) Serial.print(x);Serial.print(" ");
#define LOG2(x, y) \
  Serial.print(x); \
  Serial.println(y)
#define LOG2SP(x, y) \
  Serial.print(x);   \
  Serial.print(" "); \
  Serial.println(y)
#define LOG2EQ(x, y)      \
  Serial.print(x);        \
  Serial.print(F(" = ")); \
  Serial.println(y)
#define LOGD Serial.println(F("---"))
