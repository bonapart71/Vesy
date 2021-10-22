bool menu_param_started = false; 
int set_param_step=0;

//=========================== Количество пунктов меню ===========================

#define SET_PARAM_MAX_NUMBER 3

long current_param[SET_PARAM_MAX_NUMBER];

long set_param_min_max[3][3]=
{
  //Минимальная граница, макс граница, шаг (x,y,z)
    {100,3500,10},
    {10,3000,10},
    {0,1,1}
};

//=========================== Названия пунктов меню ===========================

char* set_param_steps_names[]={"Max_Naliv","Max_Sliv","Avtonaliv"};

void set_one_part_of_param(int i){
        if (button_Naliv_pressed){
           button_Naliv_pressed = false;
           current_param[i]=current_param[i]+set_param_min_max[i][2];
           if (current_param[i]>set_param_min_max[i][1]){
               current_param[i]=set_param_min_max[i][0];
           }
           
           //+++ Исправить очистку экрана!!!

           lcd.setCursor(0, 1);
           lcd.print("      ");
           lcd.setCursor(0, 1);           
           
           if (i==2)
           {
            if (current_param[i]==0)
              lcd.print("OFF");
            else
              lcd.print("ON");             
           }
           else{
              lcd.print(current_param[i]);
           }
           
        } else if (button_Sliv_pressed){
           button_Sliv_pressed = false;
           current_param[i]=current_param[i]-set_param_min_max[i][2];
           if (current_param[i]<set_param_min_max[i][0]){
               current_param[i]=set_param_min_max[i][1];
           }
           
           //+++ Исправить очистку экрана!!!
           lcd.setCursor(0, 1);
           lcd.print("      ");
           lcd.setCursor(0, 1);
           
           if (i==2)
           {
            if (current_param[i]==0)
              lcd.print("OFF");
            else
              lcd.print("ON");             
           }
           else{
              lcd.print(current_param[i]);
           }
           
        };
}
        

void set_param(){
    set_one_part_of_param(set_param_step);
    if (button_Menu_long_pressed) {
        button_Menu_pressed=false;
        button_Menu_long_pressed=false;
        set_param_step++;
        
        //Защита. Что-бы вес налива не был меньше веса слива
        
        if (set_param_step==1){
           set_param_min_max[1][1] = current_param[0]-work_setting.zapas;
        }
        
        if (set_param_step==SET_PARAM_MAX_NUMBER){
            //LOG1("Нормальная работа");
            menu_param_started=false;
            lcd.clear();  
            
            work_setting.max_naliv=current_param[0];
            work_setting.max_sliv=current_param[1];
            Sostoyanie_Avtonaliv=current_param[2];

            store_to_eeprom_long(EEPROM_MAX_NALIV, work_setting.max_naliv);
            store_to_eeprom_long(EEPROM_MAX_SLIV, work_setting.max_sliv);
            
            SD_Log_All_Settings();
            print_settings(work_setting);
            //save_settings_to_file();
 
            return;
        }
        
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set ");
        lcd.setCursor(4, 0);        
        lcd.print(set_param_steps_names[set_param_step]);
        
        lcd.setCursor(0, 1);
        //lcd.print(current_param[set_param_step]);

        if (set_param_step==2)
           {
            if (current_param[set_param_step]==0)
              lcd.print("OFF");
            else
              lcd.print("ON");             
           }
           else{
              lcd.print(current_param[set_param_step]);
           }
        
    }
}
