-------------------
About
-------------------
Project Description: This is the project 1 README file for CSE 321.

Contribitor List: Ray Chen (rchen63@buffalo.edu)


--------------------
Features
--------------------
None


--------------------
Required Materials
--------------------
-Nucleo L4R5ZI


--------------------
Resources and References
--------------------
University of Oklahoma README tips: https://libraries.ou.edu/content/how-make-readmetxt-file

README guide from Akash Nimare: https://gist.github.com/akashnimare/7b065c12d9750578de8e705fb4771d2f

Thread: https://os.mbed.com/docs/mbed-os/v6.14/apis/thread.html

IntteruptIn: https://os.mbed.com/docs/mbed-os/v6.14/apis/interruptin.html

DigitalOut: https://os.mbed.com/docs/mbed-os/v6.14/apis/digitalout.html

Thread_sleep_for: https://os.mbed.com/docs/mbed-os/v6.12/mbed-os-api-doxy/group__mbed__thread.html#gaaa7efb95f13c2f6055a930fd8ae06942


--------------------
Getting Started
--------------------
Nucleo L4R5ZI is required to run the program. 

Connect Nucleo L4R5ZI to the computer, click "Run program" button in the file explorer on the left sie of Mbed studio.

Initially the Blue Led will be Constantly turned on and off. If you press the button on the leftdown cornor, the Blue Led will turn off until you press the button again.


--------------------
CSE321_project_rchen63_corrected_code.cpp:
--------------------
There is an os type tool used to create periodic events with a periepherial. The name of the file and the contents in here will be entirely updated.

This is totally not bare metal since there are some tools used. Those tools instantiate with a finite reference but gots their unique object created. 


----------
Things Declared
----------
0. mbed.h
1. Thread controller;
2. InterruptIn Button(BUTTON1);
3. DigitalOut fire(LED2); 
4. int unicorn = 0; 
5. int zombie = 0;
6. void saveTheWorld(); 
7. void Zombie_is_out(); 
8. void Unicorn_save_the_world();


----------
API and Built In Elements Used
----------
Thread: 

    Allows things to be configured with scheduling and priority management. It
    is a block of code that will execute based on an event.
    There are methods to access information with this controller and to control
    it. Specifically in this project you will see: get_state and start
    https://os.mbed.com/docs/mbed-os/v6.14/apis/thread.html

IntteruptIn: 

    This creates a reference to an interrupt with a variable name to allow
    additional interaction. This interrupt is triggered by the button, with actions
    on both rise and fall of the signal.
    There are methods to configure the interrupt, specifically: rise and fall.
    https://os.mbed.com/docs/mbed-os/v6.14/apis/interruptin.html

DigitalOut:

    Allows you to set up a GPIO pin as an output, doing all that initialization
    stuff.  
    https://os.mbed.com/docs/mbed-os/v6.14/apis/digitalout.html

Thread_sleep_for:

    Thread_sleep_for is a time delay function, causes unit delay.
    https://os.mbed.com/docs/mbed-os/v6.12/mbed-os-api-doxy/group__mbed__thread.html#gaaa7efb95f13c2f6055a930fd8ae06942


----------
Custom Functions
----------
saveTheWorld:

    Description:
	    If there is no unicorn, the world will be set on fire.
        After 2000 time unit, the fire will be extinguished.
        After 500 time unit, the function will run again.
        Else If there is an unicorn, nothing happens to the world, because unicorn can protect the world.
	Inputs:
		None
	Globally referenced things used:
	    Unicorns, zombie, fire and printf

Zombie_is_out:

    Description:
        Make 1 zombie
    Inputs:
        None
    Globally referenced things used:
        Zombie

Unicorn_save_the_world:

    Description:
        If there is 1 zombie, an new unicorn will appear. 
        If there is already one unicorn, both the new and old unicorn disppear.
        The zombie is eliminated no matter what.
    Inputs:
        None
    Globally referenced things used:
        Unicorn, Zombie


