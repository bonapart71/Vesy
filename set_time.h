
bool menu_time_started = false; 
int set_time_step=0;

int current_time[5];

int set_time_min_max[5][2]=
{
    {2000,2100},
    {1,12},
    {1,31},
    {0,23},
    {0,59},
};

char* set_time_steps_names[]={"Year","Month","Day","Hour","Minute"};

enum  set_time_steps_enum{
    STEP_YEAR,
    STEP_MONTH,
    STEP_DAY,
    STEP_HOUR,
    STEP_MIN
};

void set_one_part_of_time(int i){
        if (button_Naliv_pressed){
           button_Naliv_pressed = false;
           current_time[i]++;
           if (current_time[i]>set_time_min_max[i][1]){
               current_time[i]=set_time_min_max[i][0];
           }
           
           //+++ Исправить очистку экрана!!!

           lcd.setCursor(0, 1);
           lcd.print("      ");
           lcd.setCursor(0, 1);           
           lcd.print(current_time[i]);
           
        } else if (button_Sliv_pressed){
           button_Sliv_pressed = false;
           current_time[i]--;
           if (current_time[i]<set_time_min_max[i][0]){
               current_time[i]=set_time_min_max[i][1];
           }
           
           //+++ Исправить очистку экрана!!!
           lcd.setCursor(0, 1);
           lcd.print("      ");
           lcd.setCursor(0, 1);
           lcd.print(current_time[i]);
        };
}
        

void set_time(){
    set_one_part_of_time(set_time_step);
    if (button_Menu_long_pressed) {
        button_Menu_pressed=false;
        button_Menu_long_pressed=false;
        set_time_step++;
        if (set_time_step==5){
            //LOG1("Нормальная работа");
            menu_time_started=false;
            lcd.clear();  
            rtc.setDate(current_time[2],current_time[1],current_time[0]);
            rtc.setTime(current_time[3],current_time[4],0);
            return;
        }
        
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set ");
        lcd.setCursor(4, 0);        
        lcd.print(set_time_steps_names[set_time_step]);
        
        lcd.setCursor(0, 1);
        lcd.print(current_time[set_time_step]);
    }
}
