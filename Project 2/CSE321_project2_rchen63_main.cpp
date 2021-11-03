/**
 * Author: Ray Chen (rchen63@buffalo.edu)
 
 * Purpose: 
    Implementing an all-in-one count_down alarm/timer system using the Nucleo embedded platform.
 
 * Modules/Subroutines: 
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
 
 * Constraints:
    Time is entered as m:ss
    Valid times can go up to 9 min and 59 sec
 
 * Sourses/References used:
 
**/

#include "mbed.h"
#include "lcd1602.h"
#include <cstdio>


char const keypad_c[4][4] = {
        {'1','2','3','A'},      //row 0
        {'4','5','6','B'},      //row 1
        {'7','8','9','C'},      //row 2
        {'*','0','#','D'}       //row 3
//column: 0   1   2   3
    };

    char const *keypad[4][4] = {
        {"1","2","3","A"},      //row 0
        {"4","5","6","B"},      //row 1
        {"7","8","9","C"},      //row 2
        {"*","0","#","D"}       //row 3
//column: 0   1   2   3
    };



InterruptIn col0(PD_3);     // Column 0 is triggered by PD_3
InterruptIn col1(PD_2);     // Column 1 is triggered by PD_2
InterruptIn col2(PD_1);     // Column 2 is triggered by PD_1
InterruptIn col3(PD_0);     // Column 3 is triggered by PD_0

void isr_col0();        // ISR Handler of interrupt col0
void isr_col1();        // ISR Handler of interrupt col1
void isr_col2();        // ISR Handler of interrupt col2
void isr_col3();        // ISR Handler of interrupt col3
void count_down();
void spin_polling();    // Spin polling to output from the rows

CSE321_LCD LCD(16, 2, LCD_5x10DOTS, PB_9, PB_8);   // Construct the LCD object, the object name is called LCD
int row = 0;     //row
int col = 0;     // column
int ready = 0;
int running = 0;
int pause = 0;
int time_up = 0;
int holding = 0;


int press = 0;   // If a key on the keypad is pressed, press = 1;

char Time[5] = {'0',':','0','0',0};    // The input time from usesr, initially 0:00


// main() runs in its own thread in the OS
int main()
{
    RCC->AHB2ENR |= 0x8;  //enable clock for GPIOD
    GPIOD->MODER &= ~(0x2AA00);  //Set PD_4, PD_5, PD_6, PD_7 as output mode, these are pins of rows
    GPIOD->MODER |= 0x15500;         
    GPIOD->MODER &= ~(0xFF);     //Set PD_0, PD_1, PD_2, PD_3 as input mode, these are pins of columns


    // The interrupts are all rising edge triggered.
    col0.rise(&isr_col0);       
    col1.rise(&isr_col1);
    col2.rise(&isr_col2);
    col3.rise(&isr_col3);



    LCD.begin();                        // Initialize LCD
    // 4x4 matrix Keypad

    LCD.print("Press D to");
    LCD.setCursor(0,1);
    LCD.print("Input Time");

    while(1){
        spin_polling();                     // polling

        if(press==1){                       // If an keypad is pressed
            press--;                        // Reset the press
            if(ready == 0){
                LCD.setCursor(0,0); 
                LCD.print(keypad[row][col]);
            }
            if(keypad_c[row][col] == 'D'){  // If key D on the keypad is pressed
                D:
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
                        if(keypad_c[row][col]=='D'){
                            press--;
                            goto D;
                        }
                        if(keypad_c[row][col]>='0' && keypad_c[row][col]<='9'){  // Only accept digit
                            press--;
                            input_min = 0;          // Indicate that min is inputted
                            LCD.print(keypad[row][col]);       // Print the digit on LCD
                            Time[0] = keypad_c[row][col];       // Change the min of time to the user input
                            LCD.print(":");                     // print ':' after min
                        }
                    }
                    if(press==1 && input_sec>0 && input_min==0){         // Input the digit for sec
                        if(keypad_c[row][col]=='D'){
                            press--;
                            goto D;
                        }
                        if(input_sec==2){                  // When input_sec == 2, it means the user is inputing the first digit of sec
                            if(keypad_c[row][col]>='0' && keypad_c[row][col]<='5'){ // First digit of sec can only be digit from 0 to 5
                                LCD.print(keypad[row][col]);     // Print the digit to LCD
                                press--;
                                input_sec--;                    // Indicate that the first digit of sec is inputted.
                                Time[2] = keypad_c[row][col];   // Change the first digit of sec of time to the user input
                            }else{
                                printf("%c\n",keypad_c[row][col]);
                                press--;
                            }
                        }else{                             // When input_sec == 1, it means the user in inputting the second idgit of sec
                            if(keypad_c[row][col]>='0' && keypad_c[row][col]<='9'){ //Second digit of sec can be any digit number.
                                LCD.print(keypad[row][col]);    // Print the digit to LCD
                                press--;
                                input_sec--;                   // Indicate that the second digit of sec is inputted.
                                input_time--;                   // Indicate that the time input is done
                                Time[3] = keypad_c[row][col];   // Change the second digit of sec of time to the user input
                            }else{
                                printf("%c\n",keypad_c[row][col]);
                                press--;
                            }
                        }
                    }
                }
                ready = 1;
                LCD.noBlink();
            }
            if(keypad_c[row][col] == 'A' && ready){
                LCD.noBlink();
                LCD.setCursor(0,0);
                LCD.print("Time Remaining:");
                running=1;
                pause=0;
                while(running && !pause){
                    spin_polling();
                    if(press==1 && keypad_c[row][col]=='B'){
                        running=0;
                        pause = 1;
                        press=0;
                    }else if(press==1 && keypad_c[row][col]=='D'){
                        running = 0;
                        pause = 0;
                        press = 0;
                        ready = 0;
                        goto D;
                    }
                    spin_polling();
                    if(press==1 && keypad_c[row][col]=='B'){
                        running=0;
                        pause = 1;
                        press=0;
                    }else if(press==1 && keypad_c[row][col]=='D'){
                        running = 0;
                        pause = 0;
                        press = 0;
                        ready = 0;
                        goto D;
                    }
                    spin_polling();
                    if(press==1 && keypad_c[row][col]=='B'){
                        running=0;
                        pause = 1;
                        press=0;
                    }else if(press==1 && keypad_c[row][col]=='D'){
                        running = 0;
                        pause = 0;
                        press = 0;
                        ready = 0;
                        goto D;
                    }
                    thread_sleep_for(80);
                    LCD.setCursor(6,1);
                    count_down();
                    LCD.print(Time);
                    printf("%s\n",Time);
                }
                if(time_up==1){
                    ready=0;
                    press=0;
                    LCD.clear();
                    LCD.setCursor(0,0); 
                    LCD.print("Times Up");
                    GPIOD->ODR = 0x100;
                }
            }
        }
    }
}




void isr_col0(){     // A key on column 0 is pressed
    col=0;              
    press=1;
    holding = 1;
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
    holding = 1;
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
    holding = 1;
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
    holding = 1;
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
    thread_sleep_for(75);       // Wait for 50 ms
    GPIOD->ODR |= 0x20;
    GPIOD->ODR &= ~(0xD0);      // output 1 from PD_5, and 0 from other 3 pins 
    thread_sleep_for(75);       // Wait for 50 ms
    GPIOD->ODR |= 0x40;
    GPIOD->ODR &= ~(0xB0);      // output 1 from PD_6, and 0 from other 3 pins
    thread_sleep_for(75);       // Wait for 50 ms
    GPIOD->ODR |= 0x80;
    GPIOD->ODR &= ~(0x70);      // output 1 from PD_7, and 0 from other 3 pins
    thread_sleep_for(75);       // Wait for 50 ms
}


void count_down(){
    if(Time[0]=='0' && Time[2]=='0'&& Time[3]=='0'){
        running=0;
        time_up=1;
        return;
    }
    if(Time[3]>'0'){
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
}


