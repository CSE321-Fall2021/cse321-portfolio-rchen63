/**
 * Author: Ray Chen (rchen63@buffalo.edu)
 
 * Purpose: 
    Implementing an all-in-one count_down alarm/timer system using the Nucleo embedded platform.
 
 * Modules/Subroutines: 
    "lcd1602.cpp"
    "lcd1602.h"
    InterruptIn col0(PD_3);     
    InterruptIn col1(PD_2);
    InterruptIn col2(PD_1);
    InterruptIn col3(PD_0);
    isr_col0();
    isr_col1();
    isr_col2();
    isr_col3(); 
    spin_polling();

 * The Input: 
    4x4 membrane keypad

 * The Output:
    1602 LCD
    LEDs
 
 * Constraints/Specification:
    Time is entered as m:ss.
    Valid times can go up to 9 min and 59 sec.
    Press D to input time
    Press A to start the timer after entering time
    Press B to pause the timer while counting down
 
 * Sourses/References used:
    IntteruptIn: https://os.mbed.com/docs/mbed-os/v6.14/apis/interruptin.html
    Thread_sleep_for: https://os.mbed.com/docs/mbed-os/v6.12/mbed-os-api-doxy/group__mbed__thread.html#gaaa7efb95f13c2f6055a930fd8ae06942
    STMicroelectronics, UM2179 User Manual: https://www.st.com/resource/en/user_manual/dm00368330-stm32-nucleo-144-boards-mb1312-stmicroelectronics.pdf
    STM32L4 Reference manual:  https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
 
**/

#include "mbed.h"
#include "lcd1602.h"
#include <cstdio>


char const keypad[4][4] = {
        {'1','2','3','A'},      //row 0
        {'4','5','6','B'},      //row 1
        {'7','8','9','C'},      //row 2
        {'*','0','#','D'}       //row 3
//column: 0   1   2   3
    };



InterruptIn col0(PD_3,PullDown);     // Column 0 is triggered by PD_3
InterruptIn col1(PD_2,PullDown);     // Column 1 is triggered by PD_2
InterruptIn col2(PD_1,PullDown);     // Column 2 is triggered by PD_1
InterruptIn col3(PD_0,PullDown);     // Column 3 is triggered by PD_0

void isr_col0();        // ISR Handler of interrupt col0
void isr_col1();        // ISR Handler of interrupt col1
void isr_col2();        // ISR Handler of interrupt col2
void isr_col3();        // ISR Handler of interrupt col3
void spin_polling();    // Spin polling to output from the rows

CSE321_LCD LCD(16, 2, LCD_5x10DOTS, PB_9, PB_8);   // Construct the LCD object, the object name is called LCD
int row = 0;     //row
int col = 0;     // column
int ready = 0;   // If ready = 1, the timer is ready to run
int running = 0; // If running = 1, the timer is counting down
int pause = 0;   // If pause = 1, the timer is stopped
int time_up = 0; // If time_up = 1, the timer is up

int press = 0;   // If a key on the keypad is pressed, press = 1;

char Time[5] = {'0',':','0','0',0};    // The input time from usesr, initially 0:00


// main() runs in its own thread in the OS
int main()
{
    RCC->AHB2ENR |= 0x8;  //enable clock for GPIOD
    GPIOD->MODER &= ~(0x2AA00);  //Set PD_4, PD_5, PD_6, PD_7 as output mode, these are pins of rows. Set PD_8  as output mode, this is used for the LEDs when times up.
    GPIOD->MODER |= 0x15500;         
    GPIOD->MODER &= ~(0xFF);     //Set PD_0, PD_1, PD_2, PD_3 as input mode, these are pins of columns


    // The interrupts are all rising edge triggered.
    col0.rise(&isr_col0);       
    col1.rise(&isr_col1);
    col2.rise(&isr_col2);
    col3.rise(&isr_col3);



    LCD.begin();                        // Initialize LCD
    // 4x4 matrix Keypad


    // Display prompt
    LCD.print("Press D to");
    LCD.setCursor(0,1);
    LCD.print("Input Time");

    while(1){
        A:                                  // Label for jump
        spin_polling();                     // polling

        if(press==1){                       // If an keypad is pressed
            press--;                        // Reset the press
            if(keypad[row][col] == 'D'){  // If key D on the keypad is pressed
                D:                      // Label for jump
                GPIOD->ODR &= ~(0x100);
                int input_min = 1;      // One digit for minute
                int input_sec = 2;      // Two digit for second
                int input_time = 1;     // input_time is 1 until the time input is done
                LCD.clear();            // Clear any displayed characters on the LCD
                LCD.setCursor(0,0);     
                LCD.print("Input the time:");  // Display prompt
                LCD.setCursor(6, 1);           // Set cursor to the second and ready for input
                LCD.blink_on();                // Turn the cursor blink on when inputing time
                while(input_time){
                    spin_polling();             // Keep polling
                    if(press==1 && input_min==1){   // Input the digit for minute
                        if(keypad[row][col]=='D'){
                            press--;
                            goto D;
                        }
                        if(keypad[row][col]>='0' && keypad[row][col]<='9'){  // Only accept digit
                            press--;
                            input_min = 0;          // Indicate that min is inputted
                            LCD.print((char const*) keypad[row][col]);       // Print the digit on LCD
                            Time[0] = keypad[row][col];       // Change the min of time to the user input
                            LCD.print(":");                     // print ':' after min
                        }
                    }
                    if(press==1 && input_sec>0 && input_min==0){         // Input the digit for sec
                        if(keypad[row][col]=='D'){
                            press--;
                            goto D;
                        }
                        if(input_sec==2){                  // When input_sec == 2, it means the user is inputing the first digit of sec
                            if(keypad[row][col]>='0' && keypad[row][col]<='5'){ // First digit of sec can only be digit from 0 to 5
                                LCD.print((char const*) keypad[row][col]);     // Print the digit to LCD
                                press--;
                                input_sec--;                    // Indicate that the first digit of sec is inputted.
                                Time[2] = keypad[row][col];   // Change the first digit of sec of time to the user input
                            }else{
                                printf("%c\n",keypad[row][col]);
                                press--;
                            }
                        }else{                             // When input_sec == 1, it means the user in inputting the second idgit of sec
                            if(keypad[row][col]>='0' && keypad[row][col]<='9'){ //Second digit of sec can be any digit number.
                                LCD.print((char const*) keypad[row][col]);    // Print the digit to LCD
                                press--;
                                input_sec--;                   // Indicate that the second digit of sec is inputted.
                                input_time--;                   // Indicate that the time input is done
                                Time[3] = keypad[row][col];   // Change the second digit of sec of time to the user input
                            }else{
                                printf("%c\n",keypad[row][col]);
                                press--;
                            }
                        }
                    }
                }
                ready = 1;          // The timer is now ready to run
                LCD.noBlink();      // Turn the cursor blink off
            }
            if(keypad[row][col] == 'A' && ready){
                LCD.setCursor(0,0);
                LCD.print("Time Remaining:");        // Display prompt
                running=1;          // Start running
                pause=0;            // Timer is not paused now
                while(running && !pause){   

                    // Because each spin_polling takes 75*4 = 300 ms, spin_polling is needed to repeat 3 times to make up 900 ms
                    spin_polling();     //Keep polling
                    if(press==1 && keypad[row][col]=='B'){   //If key 'B' is pressed
                        running=0;              // Stop running
                        pause = 1;              // Paused the timer
                        press=0;
                        LCD.clear();            
                        LCD.setCursor(5, 0);
                        LCD.print("Paused");    // Display "Paused" on the LCD
                        LCD.setCursor(6,1);
                        LCD.print(Time);
                        goto A;                 // Jump out the running loop
                    }else if(press==1 && keypad[row][col]=='D'){  // If key 'D' is pressed reset everything and start time inputing phase
                        running = 0;            
                        pause = 0;             
                        press = 0;
                        ready = 0;
                        goto D;
                    }
                    spin_polling();
                    if(press==1 && keypad[row][col]=='B'){
                        running=0;
                        pause = 1;
                        press=0;
                        LCD.clear();
                        LCD.setCursor(5, 0);
                        LCD.print("Paused");
                        LCD.setCursor(6,1);
                        LCD.print(Time);
                        goto A;
                    }else if(press==1 && keypad[row][col]=='D'){
                        running = 0;
                        pause = 0;
                        press = 0;
                        ready = 0;
                        goto D;
                    }
                    spin_polling();
                    if(press==1 && keypad[row][col]=='B'){
                        running=0;
                        pause = 1;
                        press=0;
                        LCD.clear();
                        LCD.setCursor(5, 0);
                        LCD.print("Paused");
                        LCD.setCursor(6,1);
                        LCD.print(Time);
                        goto A;
                    }else if(press==1 && keypad[row][col]=='D'){
                        running = 0;
                        pause = 0;
                        press = 0;
                        ready = 0;
                        goto D;
                    }
                    thread_sleep_for(80);   // 80 ms is used instead of 100 ms because running codes takes some time, I tested it and 80 ms is an optimal wait time. 
                                            // The timer counts down every 1 second, but the timer is not 100% accurate. There is 0.02 second delay after the timer runs 1 minute.
                    LCD.setCursor(6,1);

                    // Count Down
                    if(Time[0]=='0' && Time[2]=='0'&& Time[3]=='0'){  // If timer = 0:00, time up and stop running
                        running=0;
                        time_up=1;
                    }else if(Time[3]>'0'){                    
                        Time[3]--;
                    }else{
                        Time[3]='9';
                        if(Time[2]>'0'){
                            Time[2]--;
                        }else{
                            Time[2]='5';
                            if(Time[0]>'0'){
                                Time[0]--;
                            }else{
                                time_up=1;
                                running=0;
                            }
                        }
                    }
                    LCD.print(Time);        // Update timer
                    
                }   
                if(time_up==1){            // Display "Times Up" when the timer reaches 0:00, and turn on multiple LEDs
                    ready=0;
                    press=0;
                    LCD.clear();
                    LCD.setCursor(4,0); 
                    LCD.print("Times Up");
                    GPIOD->ODR = 0x100;    // PD_8 Outputs 1 to turn on LEDs
                }
            }
        }
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


// Output 1 from each of PD_4, PD_5, PD_6 and PD_7 periodically
void spin_polling(){
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




