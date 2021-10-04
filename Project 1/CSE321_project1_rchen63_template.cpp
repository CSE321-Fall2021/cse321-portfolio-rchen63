/**
 * Author: Ray Chen (rchen63@buffalo.edu)
 
 * Purpose: 
 
 * Modules/Subroutines:
 
 * The Input: 
 
 * The Output:
 
 * Constraints:
 
 * Sourses/References used:
 
**/



#include <mbed.h>

int main_template() {
    /* code */
    //turn on things
    //button
    // BUTTON1 = PC_13
    //led connection
    // LED2    = PB_7,  // LD2 [Blue]

    //MODER and RCC
    //LED is out
    //MODER needs 00 in this control spot
    //pins 27 and 26 need to be 0
    GPIOC->MODER&=~(0x0C000000);
    GPIOB->MODER&=~(0x8000);
    GPIOB->MODER|=0x4000;

    //button is in

    //ports b and c are needed
    RCC->AHB2ENR |=0x6;
    //connecting button to LED
    while(1){
        if(((GPIOC->IDR)&0x2000) == 1){
            GPIOB->ODR|=0x80;
        }
        else{
            GPIOB->ODR&=~(0x80);    
        }
    }
    return 0;
}

