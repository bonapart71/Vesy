
#define NUMBER_OF_MODBUS_REGISTERS 16
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
    ModbusTempTable[6] = led_Sliv.ledStatus();

    ModbusTempTable[7] = (int)valve_Naliv_open;
    ModbusTempTable[8] = (int)valve_Sliv_open;

    noInterrupts();
    for (int i = 0; i < NUMBER_OF_MODBUS_REGISTERS; i++)
        ModbusRegTable[i] = ModbusTempTable[i];
    interrupts();
}
