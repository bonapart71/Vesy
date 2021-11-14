
#define NUMBER_OF_MODBUS_REGISTERS 30

// с какого регистра начинаются ригистры для записи
#define INPUT_MODBUS_REGISTERS_START 20  

#define DIRECT_PIN 4
#define MODBUS_ADDRESS 1
#define MODBUS_SERIAL Serial1

unsigned int ModbusRegTable[NUMBER_OF_MODBUS_REGISTERS];  // таблица регистров
unsigned int ModbusTempTable[NUMBER_OF_MODBUS_REGISTERS]; // таблица где временно хранятся регистры

Modbus ModbusSlave(MODBUS_ADDRESS, MODBUS_SERIAL, DIRECT_PIN); // this is slave @1 and RS-232 or USB-FTDI

void Modbus_Timer_Interrupt()
{
    ModbusSlave.poll(ModbusRegTable, NUMBER_OF_MODBUS_REGISTERS);
}

void Modbus_Update_Registers()
{
    unsigned int *ptr;
    long temp;

    //Long to 2 word conversion
    ptr = (unsigned int *)&cycle;
   
    ModbusTempTable[1] = *ptr++;
    ModbusTempTable[0] = *ptr;

    temp = round(sred_wess_N_izmer);
    ptr = (unsigned int *)&temp;
    ModbusTempTable[3] = *ptr++;
    ModbusTempTable[2] = *ptr;

    ModbusTempTable[4] = led_Naliv.ledStatus();
    ModbusTempTable[5] = led_Sliv.ledStatus();
    ModbusTempTable[6] = led_Alarm.ledStatus();

    ModbusTempTable[7] = (int)valve_Naliv_open;
    ModbusTempTable[8] = (int)valve_Sliv_open;
    
    ModbusTempTable[9] = Sostoyanie_Systemy;

    ModbusTempTable[10] = Sostoyanie_Alarm;

    temp = Sliv_Time;
    ptr = (unsigned int *)&temp;
    ModbusTempTable[12] = *ptr++;
    ModbusTempTable[11] = *ptr;

    
    // Наверное нужно ввести новую переменную
    temp = work_setting.max_sliv;
    ptr = (unsigned int *)&temp;
    ModbusTempTable[14] = *ptr++;
    ModbusTempTable[13] = *ptr;

    temp = work_setting.max_naliv;
    ptr = (unsigned int *)&temp;
    ModbusTempTable[16] = *ptr++;
    ModbusTempTable[15] = *ptr;

    temp = ves_poslednego_sliva;
    ptr = (unsigned int *)&temp;
    ModbusTempTable[18] = *ptr++;
    ModbusTempTable[17] = *ptr;

    ModbusTempTable[19] = Sostoyanie_Avtonaliv;

    

    

    noInterrupts();
    //for (int i = 0; i < NUMBER_OF_MODBUS_REGISTERS; i++)
    for (int i = 0; i < INPUT_MODBUS_REGISTERS_START; i++)
     
        ModbusRegTable[i] = ModbusTempTable[i];
    interrupts();
}
