/**
 * Author: Ray Chen (rchen63@buffalo.edu)
 
 * Purpose: 
    * This is the porject 3 of CSE321, fall 2021, University at Buffalo.

    * A temperature alarm system is designed and implemeented using Nucleo L4R5ZI, 
      1602 LCD screen, 4x4 Membrane Matrix keypad, DHT11 temperature humidity sensor,
      and a low level trigger buzzer. 
 
 * Modules/Subroutines: 
    "lcd1602.cpp"
    "lcd1602.h"
    "DHT.cpp"
    "DHT.h"
    InterruptIn col0(PD_3);     
    InterruptIn col1(PD_2);
    InterruptIn col2(PD_1);
    InterruptIn col3(PD_0);
    isr_col0();
    isr_col1();
    isr_col2();
    isr_col3(); 
    spin_polling();
    void readBuzzer();
    void read_temperature();

 * The Input: 
    * 4x4 membrane matrix keypad
    * DHT11 temperature humidity sensor

 * The Output:
    * 1602 LCD Screen
    * buzzer
 
 * Constraints/Specification:
    * The DHT11 can only measure temperature between 0 to 50 degree Celsius
    * Buzzer outputs high pitch sound, do not put the buzzer too close to the ears
    * Temperature measurement error is ±2 ℃.
    * It can only be used to measure the air temperature.
    * It takes time to measure the current temperature.
    * User can use matrix keypad to input the temperature range.
	* Current temperature will be displayed on the LCD screen.
	* The default temperature range is 10 ℃ to 35 ℃.
	* The valid temperature range is between 0 ℃ and 50 ℃.
	* The lower bound of the temperature range should be lower than the upper bound of the temperature range. 
	* If the temperature is not in the set range, the buzzer outputs sound.
	* If the buzzer alarms, user can press C on the matrix keypad to stop the buzzer for 1 minute. After 1 minute, if the current temperature is still out of rang, the buzzer keeps alerting.
	* The LCD displays Range: followed by the set temperature range on the top row and displays Current Temp: followed by the current temperature in Celsius on the bottom row.
	* When inputting time, Enter new range: is displayed on the top row of LCD and the user can input the range on the second row of the LCD.
	* The alarm runs “forever”

 
 * Sourses/References used:
    * IntteruptIn: https://os.mbed.com/docs/mbed-os/v6.14/apis/interruptin.html
    * Thread_sleep_for: https://os.mbed.com/docs/mbed-os/v6.12/mbed-os-api-doxy/group__mbed__thread.html#gaaa7efb95f13c2f6055a930fd8ae06942
    * STMicroelectronics, UM2179 User Manual: https://www.st.com/resource/en/user_manual/dm00368330-stm32-nucleo-144-boards-mb1312-stmicroelectronics.pdf
    * STM32L4 Reference manual:  https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
    * DigitalOut: https://os.mbed.com/docs/mbed-os/v6.8/apis/digitalout.html
    * Watchdog: https://os.mbed.com/docs/mbed-os/v6.15/apis/watchdog.html
    * Mutex: https://os.mbed.com/docs/mbed-os/v6.15/apis/mutex.html
    * EventQueue: https://os.mbed.com/docs/mbed-os/v6.15/apis/eventqueue.html
    * Thread: https://os.mbed.com/docs/mbed-os/v6.15/apis/thread.html
 
**/




#include "mbed.h"
#include "DHT.h"
#include "lcd1602.h"

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
Thread interrupts;  // This thread is used for interrupts triggers

EventQueue interruptQueue(32 * EVENTS_QUEUE_SIZE);  // EvenetQueue is used for synchronization so that isrs execute in orders.

//Timer t;
InterruptIn col0(PD_3,PullDown);     // Column 0 is triggered by PD_3
InterruptIn col1(PD_2,PullDown);     // Column 1 is triggered by PD_2
InterruptIn col2(PD_1,PullDown);     // Column 2 is triggered by PD_1
InterruptIn col3(PD_0,PullDown);     // Column 3 is triggered by PD_0

CSE321_LCD LCD(16, 2, LCD_5x10DOTS, PB_9, PB_8); // LCD, SDA to PB_9 and SCL to PB_8
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
int currentTemp =25;        // Global variable used for store the current temperature in Celsius
int errorTemp = 0;      // If an error happens, errorTemp = 500;


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
    interrupts.start(callback(&interruptQueue, &EventQueue::dispatch_forever));

    // The interrupts are all rising edge triggered.
    col0.rise(interruptQueue.event(isr_col0));       
    col1.rise(interruptQueue.event(isr_col1));  
    col2.rise(interruptQueue.event(isr_col2));  
    col3.rise(interruptQueue.event(isr_col3));  


    
    Watchdog &watchdog = Watchdog::get_instance(); // Create watchdog, if the user does not input the range while entering the range for 15 seconds, the watchdog reset the program
    watchdog.start(15000);  // watchdog timeout value is 15 seconds.
    lock.lock();        // Lock mutex when printing on LCD
    LCD.home();
    LCD.print("Range:");    
    LCD.print(set_range);
    lock.unlock();      // Unlock mutex when finish printing on LCD

    buzzerIO.start(callback(readBuzzer)); // Start buzzer thread, buzzer alerts when current temperature is out of range.

    while (true) {
        watchdog.kick();    // Keep watchdog awake while running
        if(press == 1){     // If a key on keypad is pressed
            press = 0;      
            if(keypad[row][col]=='D'){  // When D is press, start inputing the temperature range
                lock.lock();        // Lock mutex when start inputing the temperature range
                errorTemp = 500;    // Set errorTemp = 500 mean to display the error message for 5 second when an invalide temperature range is entered

                // Print prompt
                LCD.clear();        
                LCD.home();
                LCD.print("Enter new range:");
                LCD.setCursor(3,1);
                LCD.print("-");
                LCD.setCursor(8,1);
                LCD.print("C");
                LCD.setCursor(0, 1);
                LCD.blink(); // Turn on cursor blinking while inputing

                // Lower bound and Upper bound of temperature range are both 2 digits
                int lower = 2;  
                int upper = 2;
                char new_Lower[3]={' ',' '};
                char new_Upper[3]={' ',' '};
                int new_lowerBound;
                int new_upperBound;

                // Entering lower bound
                while(lower>0){
                    // Entering first digit of lower bound
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
                    // Entering second digit of lower bound
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
                // Convert lower bound from string to integer
                new_lowerBound = new_Lower[0]-'0';
                new_lowerBound = new_lowerBound*10 + new_Lower[1] - '0';
                LCD.print(" - ");
                // Entering upper bound
                while(upper>0){
                    // Entering the first digit of upper bound
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
                    // Entering the second digit of upper bound
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

                LCD.noBlink(); //Turn off cursor blinking while inputing
                // Convert upper bound from string to integer
                new_upperBound = new_Upper[0]-'0';
                new_upperBound = new_upperBound*10 + new_Upper[1]-'0';

                // Error detecting
                if(new_upperBound>50 || new_lowerBound>50){ // Error 0: When lower bound is greater than upper bound
                    // Print Error
                    LCD.clear();
                    LCD.home();
                    LCD.print("Error 0");
                    errorTemp = 500;    
                }else if(new_lowerBound>=new_upperBound){ // Error 1: When upper bound or lower bound is greater than the maximum range of temperature measured by DHT11
                    // Print Error
                    LCD.clear();
                    LCD.home();
                    LCD.print("Error 1");
                    errorTemp = 500;
                }else{      // If no error happens, change the lower bound and upper abound to the new temperature range
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
                    errorTemp=0;
                }
                lock.unlock();
            }else if(keypad[row][col]=='C' && buzzer == 0){  // While the buzzer is alerting, and key 'C' on the keypad is pressed, stop the buzzer for 1 minute
                lock.lock();
                pressC=1;
                lock.unlock();
            }
        }
    }
}



void spin_polling(){
    while(true){
        GPIOD->ODR |= 0x10;         
        GPIOD->ODR &= ~(0xE0);      // output 1 from PD_4, and 0 from other 3 pins 
        thread_sleep_for(75);       // Wait for 75 ms
        GPIOD->ODR |= 0x20;
        GPIOD->ODR &= ~(0xD0);      // output 1 from PD_5, and 0 from other 3 pins 
        thread_sleep_for(75);       // Wait for 75 ms
        GPIOD->ODR |= 0x40;
        GPIOD->ODR &= ~(0xB0);      // output 1 from PD_6, and 0 from other 3 pins
        thread_sleep_for(75);       // Wait for 75 ms
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
void isr_col1(){    // A key on column 1 is pressed
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
void isr_col2(){    // A key on column 3 is pressed
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
void isr_col3(){   // A key on column 3 is pressed
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
        while(errorTemp>1){   // While errorTemp is greater than 1, the error message will be displayed for 5 seconds
            lock.lock();
            thread_sleep_for(10);
            errorTemp--;
            lock.unlock();
        }
        if(errorTemp==1){      // Return to display the temperature range after displaying error message for 5 seconds.
            lock.lock();
            LCD.home();
            LCD.print("Range:");
            LCD.print(set_range);
            errorTemp=0;
            lock.unlock();
        }
        // Display the current temperature
        lock.lock();
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



void readBuzzer(){
    while(true){
        if(currentTemp<lowerBound || currentTemp>upperBound){  // When the current temperature is out of range, buzzer start alerting.
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

