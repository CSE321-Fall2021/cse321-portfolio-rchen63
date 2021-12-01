#include "mbed.h"
#include "DHT.h"
#include "lcd1602.h"
#include <algorithm>
#include <cstdio>
#include <stdio.h>
#include <string> 


char const keypad[4][4] = {
        {'1','2','3','A'},      //row 0
        {'4','5','6','B'},      //row 1
        {'7','8','9','C'},      //row 2
        {'*','0','#','D'}       //row 3
//column: 0   1   2   3
    };

    char k[1] = {' '};

Mutex lock;         //Mutex is used for critical section protection.
Thread polling;     //This thread is used for spin_polling
Thread readTemp;    // This thread is used to read the temperature in Celsius
Thread buzzerIO;    // This thread monitors the temperature and make the buzzer alert when current temperature is out of range.


//Timer t;
InterruptIn col0(PD_3,PullDown);     // Column 0 is triggered by PD_3
InterruptIn col1(PD_2,PullDown);     // Column 1 is triggered by PD_2
InterruptIn col2(PD_1,PullDown);     // Column 2 is triggered by PD_1
InterruptIn col3(PD_0,PullDown);     // Column 3 is triggered by PD_0

CSE321_LCD LCD(16, 2, LCD_5x10DOTS, PB_9, PB_8); // LCD
DHT11 DHT(PB_10);           // DHT11
DigitalOut buzzer(PB_11);   // Buzzer recieves signal from PB_11



void isr_col0();        // ISR Handler of interrupt col0
void isr_col1();        // ISR Handler of interrupt col1
void isr_col2();        // ISR Handler of interrupt col2
void isr_col3();        // ISR Handler of interrupt col3
void spin_polling();    // Spin polling to output from the rows
void read_temperature();    // Used in Thread readTemp to read temperature, it reads the temperature in Celsius and display it on the LCD
void readBuzzer();      // Used in Thread buzzerIO to make the buzzer alert


int row = 0;     //row
int col = 0;     // column
int press = 0;   // If a key on the keypad is pressed, press = 1;
int pressC=0;      // If 'C' on the kaypad is pressed, pressC = 1; 
int lowerBound = 10;    // Default lower bound of temperature is 10 Celsius degree
int upperBound = 35;    // Default upper bound of temperature is 35 Celsius degree
int currentTemp;        // Global variable used for store the current temperature in Celsius
int errorTemp = 0;      // If an error happens, errorTemp = 1;


char set_range[7]={'1','0','C','-','3','5','C'};  // LCD print this string.



// main() runs in its own thread in the OS
int main()
{
    buzzer = 1;             // The buzzer is low level triggered, so buzzer = 1 turns the buzzer off.
    RCC->AHB2ENR |= 0x8;    // enable clock for GPIOD
    GPIOD->MODER &= ~(0xAA00);  //Set PD_4, PD_5, PD_6, PD_7 as output mode, these are pins of rows.
    GPIOD->MODER |= 0x5500;         
    GPIOD->MODER &= ~(0xFF);     //Set PD_0, PD_1, PD_2, PD_3 as input mode, these are pins of columns
    LCD.begin();            // Initialize the LCD
    LCD.clear();            // Clear all the content on the LCD


    polling.start(callback(spin_polling));  // Start spin_polling in the Thread polling
    readTemp.start(callback(read_temperature)); // Start reading temperature

    // The interrupts are all rising edge triggered.
    col0.rise(&isr_col0);       
    col1.rise(&isr_col1);
    col2.rise(&isr_col2);
    col3.rise(&isr_col3);

    
    Watchdog &watchdog = Watchdog::get_instance(); // Create watchdog
    lock.lock();        // Lock mutex when printing on LCD
    LCD.home();
    LCD.print("Range:");    
    LCD.print(set_range);
    lock.unlock();      // Unlock mutex when finish printing on LCD

    buzzerIO.start(callback(readBuzzer)); // Start buzzer thread, buzzer alerts when current temperature is out of range.
    
    watchdog.start(15000);  // watchdog timeout value is 15 seconds.
    while (true) {
        watchdog.kick();    // Keep watchdog awake while running
        if(press == 1){     // If a key on keypad is pressed
            press = 0;      
            if(keypad[row][col]=='D'){
                lock.lock();
                // readTemp.terminate();
                // isr.release();
                LCD.clear();
                LCD.home();
                LCD.print("Enter new range:");
                LCD.setCursor(3,1);
                LCD.print("-");
                LCD.setCursor(8,1);
                LCD.print("C");
                LCD.setCursor(0, 1);
                LCD.blink();
                int lower = 2;
                int upper = 2;
                char new_Lower[3]={' ',' ',0};
                char new_Upper[3]={' ',' ',0};
                int new_lowerBound = 0;
                int new_upperBound = 0;
                while(lower>0){
                    if(press==1 && lower == 2){
                        press = 0;
                        if(keypad[row][col]>='0' &&keypad[row][col]<='9'){
                            watchdog.kick();
                            lower--;
                            new_Lower[0] = keypad[row][col];
                            k[0]=keypad[row][col];
                            LCD.print(k);
                        }
                    }
                    if(press == 1 && lower == 1){
                        press = 0;
                        if(keypad[row][col]>='0' &&keypad[row][col]<='9'){
                            watchdog.kick();
                            lower--;
                            new_Lower[1] = keypad[row][col];
                            k[0]=keypad[row][col];
                            LCD.print(k);
                        }
                    }
                }
                new_lowerBound = new_Lower[0]-'0';
                new_lowerBound = new_lowerBound*10 + new_Lower[1] - '0';
                LCD.print(" - ");
                while(upper>0){
                    if(press==1 && upper == 2){
                        press = 0;
                        if(keypad[row][col]>='0' &&keypad[row][col]<='9'){
                            watchdog.kick();
                            upper--;
                            new_Upper[0] = keypad[row][col];
                            k[0]=keypad[row][col];
                            LCD.print(k);
                        }
                    }
                    if(press == 1 && upper == 1){
                        press = 0;
                        if(keypad[row][col]>='0' &&keypad[row][col]<='9'){
                            watchdog.kick();
                            upper--;
                            new_Upper[1] = keypad[row][col];
                            k[0]=keypad[row][col];
                            LCD.print(k);
                        }
                    }
                }
                LCD.noBlink();
                new_upperBound = new_Upper[0]-'0';
                new_upperBound = new_upperBound*10 + new_Upper[1]-'0';
                printf("%d\n",new_lowerBound);
                printf("%d\n",new_upperBound);
                if(new_lowerBound>=new_upperBound){
                    LCD.clear();
                    LCD.home();
                    LCD.print("Error 0");
                    errorTemp = 1;

                }else if(new_upperBound>=50 || new_lowerBound<=0){
                    LCD.clear();
                    LCD.home();
                    LCD.print("Error 1");
                    errorTemp = 1;
                }else{
                    lowerBound = new_lowerBound;
                    upperBound = new_upperBound;
                    set_range[0] = new_Lower[0];
                    set_range[1] = new_Lower[1];
                    set_range[4] = new_Upper[0];
                    set_range[5] = new_Upper[1];
                    pressC=0;
                    LCD.clear();
                    LCD.print("Range:");
                    LCD.print(set_range);
                }
                lock.unlock();
            }else if(keypad[row][col]=='C'){
                lock.lock();
                pressC=1;
                lock.unlock();
            }
        }
    }
}



void spin_polling(){
    while(true){
        // printf("PD_4\n");
        GPIOD->ODR |= 0x10;         
        GPIOD->ODR &= ~(0xE0);      // output 1 from PD_4, and 0 from other 3 pins 
        thread_sleep_for(75);       // Wait for 75 ms
        // printf("PD_5\n");
        GPIOD->ODR |= 0x20;
        GPIOD->ODR &= ~(0xD0);      // output 1 from PD_5, and 0 from other 3 pins 
        thread_sleep_for(75);       // Wait for 75 ms
        // printf("PD_6\n");
        GPIOD->ODR |= 0x40;
        GPIOD->ODR &= ~(0xB0);      // output 1 from PD_6, and 0 from other 3 pins
        thread_sleep_for(75);       // Wait for 75 ms
        // printf("PD_7\n");
        GPIOD->ODR |= 0x80;
        GPIOD->ODR &= ~(0x70);      // output 1 from PD_7, and 0 from other 3 pins
        thread_sleep_for(75);       // Wait for 75 ms
    }
}

void isr_col0(){     // A key on column 0 is pressed
    
    col=0;              
    press=1;
    if((GPIOD->ODR & 0x80) == 0x80){        // If PD_7 outputs 1
        row=0;           
    }else if((GPIOD->ODR & 0x40) == 0x40){  // If PD_6 outputs 1
        row=1;
    }else if((GPIOD->ODR & 0x20) == 0x20){  // If PD_5 outputs 1
        row=2;
    }else if((GPIOD->ODR & 0x10) == 0x10){  // If PD_4 outputs 1
        row=3;
    }
 
}
void isr_col1(){
   
    col=1;
    press=1;
    if((GPIOD->ODR & 0x80) == 0x80){        // If PD_7 outputs 1
        row=0;           
    }else if((GPIOD->ODR & 0x40) == 0x40){  // If PD_6 outputs 1
        row=1;
    }else if((GPIOD->ODR & 0x20) == 0x20){  // If PD_5 outputs 1
        row=2;
    }else if((GPIOD->ODR & 0x10) == 0x10){  // If PD_4 outputs 1
        row=3;
    }

}
void isr_col2(){
    
    col=2;
    press=1;
    if((GPIOD->ODR & 0x80) == 0x80){        // If PD_7 outputs 1
        row=0;           
    }else if((GPIOD->ODR & 0x40) == 0x40){  // If PD_6 outputs 1
        row=1;
    }else if((GPIOD->ODR & 0x20) == 0x20){  // If PD_5 outputs 1
        row=2;
    }else if((GPIOD->ODR & 0x10) == 0x10){  // If PD_4 outputs 1
        row=3;
    }
 
}
void isr_col3(){
   
    col=3;
    press=1;
    if((GPIOD->ODR & 0x80) == 0x80){        // If PD_7 outputs 1
        row=0;           
    }else if((GPIOD->ODR & 0x40) == 0x40){  // If PD_6 outputs 1
        row=1;
    }else if((GPIOD->ODR & 0x20) == 0x20){  // If PD_5 outputs 1
        row=2;
    }else if((GPIOD->ODR & 0x10) == 0x10){  // If PD_4 outputs 1
        row=3;
    }
 
}

void read_temperature(){
    while(true){
        lock.lock();
        if(errorTemp==1){
            thread_sleep_for(5000);
            errorTemp = 0;
            LCD.home();
            LCD.print("Range:");
            LCD.print(set_range);
        }
        DHT.read();
        LCD.setCursor(0, 1);
        LCD.print("Current Temp:");
        currentTemp = DHT.getCelsius();
        char tempChar[3];
        tempChar[2] = 'C';
        tempChar[1] = currentTemp % 10 + '0';
        tempChar[0] = (currentTemp - (currentTemp % 10))/10 + '0';
        LCD.print(tempChar);
        lock.unlock();
        thread_sleep_for(1000);
    }
    
}

//  #include "mbed.h"
// #include <cstdio>

void readBuzzer(){
    while(true){
        if(currentTemp<lowerBound || currentTemp>upperBound){
            if(pressC==1){
                thread_sleep_for(60000);
                pressC=0;
            }else{
                buzzer = 0;
                thread_sleep_for(500);
                buzzer = 1;
                thread_sleep_for(500);
            }
            
        }
    }
}

