/**
 * Author: Unknown

 * Editor: Ray Chen (rchen63@buffalo.edu)
 
 * Purpose: Save the world.

 * Modules/Subroutines:
    saveTheWorld()
    Zombie_is_out()
    Unicorn_save_the_world()

 * The Input: Button1
 
 * The Output: Blue Led
 
 * Constraints: None
 
 * Sourses/References used: 
    Thread: https://os.mbed.com/docs/mbed-os/v6.14/apis/thread.html
    IntteruptIn: https://os.mbed.com/docs/mbed-os/v6.14/apis/interruptin.html
    DigitalOut: https://os.mbed.com/docs/mbed-os/v6.14/apis/digitalout.html
    Thread_sleep_for: https://os.mbed.com/docs/mbed-os/v6.12/mbed-os-api-doxy/group__mbed__thread.html#gaaa7efb95f13c2f6055a930fd8ae06942
 
**/

#include "mbed.h"

/*
    Thread allows things to be configured with scheduling and priority management.
    It is a block of code that will execute based on an event.

    There are methods to access information with this controller and to control
    it. Specifically in this project you will see: get_state and start
*/
Thread controller; 

InterruptIn Button(BUTTON1); //

DigitalOut fire(LED2); // establish blue led as an output
                        // When blue led is on, the world is on fire

int unicorn = 0;    // Unicorn is initially 0. Only 1 unicorn can exit at a time. Unicorn protect the world by eliminating zombie
int zombie = 0;     // Zombie is initially 0.

void saveTheWorld(); 
void Zombie_is_out(); 
void Unicorn_save_the_world();


int main() {
    // start the allowed execution of the thread 
    printf("----------------START----------------\n");
    printf("Starting state of thread: %d\n", controller.get_state());
    controller.start(saveTheWorld); 
    printf("State of thread right after start: %d\n", controller.get_state());
    Button.rise(&Zombie_is_out);  // One zombie is out when the botton is in rising edge
    Button.fall(&Unicorn_save_the_world);  // The unicorn kills the zombie if there is one zombie when the botton is in falling edge
    return 0;
}

// make the handler
void saveTheWorld() {
    while (true) {
            if(unicorn==0){   //If there is no unicorn, the fire keeps getting on and off
            fire = !fire;     //fire = 1
            printf("Set the fire\n"); 
            thread_sleep_for(2000); //Thread_sleep is a time delay function, causes a 2000 unit delay
            fire = !fire;     //fire = 0
            printf("The fire is extinguished\n");
            thread_sleep_for(500); //Thread_sleep is a time delay function, causes a 500 unit delay
        }
    }
}

// Make one zombie
void Zombie_is_out() {
    // togle the state of the thread 
    // set flag...this is where we end
    zombie=1;
}

// Make one unicorn if there is one zombie. 

void Unicorn_save_the_world() {
    if (zombie==1){
        unicorn++;      // Make one unicorn if there is one zombie. 
        unicorn %= 2; 	// If there are two unicorns, they will both disappear.				 	    
        zombie=0;       // Eliminate the zombie no matte what.
    }
}
