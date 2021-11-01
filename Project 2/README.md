-------------------
Last Update date
-------------------
10/14/2021

-------------------
About
-------------------
This is Project 2 of CSE 321

An alarm/timer system will be implemented using the Necleo embedded platform.


--------------------
Features
--------------------
None


--------------------
Required Materials
--------------------
-Nucleo L4R5ZI

-4x4 Matrix Membrane Keypad

-1602 LCD


--------------------
Resources and References
--------------------

IntteruptIn: https://os.mbed.com/docs/mbed-os/v6.14/apis/interruptin.html

Thread_sleep_for: https://os.mbed.com/docs/mbed-os/v6.12/mbed-os-api-doxy/group__mbed__thread.html#gaaa7efb95f13c2f6055a930fd8ae06942


--------------------
Getting Started
--------------------
Keypad: Connect pin 8 to PD_7, pin 7 to PD_6, pin 6 to PD_5, pin 5 to PD_4, pin 4 to PD_3, pin 3 to PD_2, pin 2 to PD_1  and pin 1 to PD_0.

1602LCD: Connect GND to GND, VCC to +5V, SDA to PB_9 and SCL to PB_8.

Load the codes to Nucleo.

Press "A" on keypad to start the timer.

Press "B" on keypad to stop/turn off.

Press "D" on keypad to start input the time.

Time is entered as m:ss.


--------------------
CSE321_project2_rchen63_main.cpp:
--------------------
The main program that implement the logic. Spin polling and interrupts are used to interact with the 4x4 matrix keypad. 

The keypad is used for user input, LCD will react to the user input of keypad.

Currently, only time inputing part is implemented. More function will be implemented lately.


----------
Things Declared
----------
1. "mbed.h"
2. "lcd1602.h"
3. keypad
4. InterruptIn col0(PD_3)
5. InterruptIn col1(PD_2)
6. InterruptIn col2(PD_1)
7. InterruptIn col3(PD_0)
8. void isr_col0()
9. void isr_col1()
10. void isr_col2()
11. void isr_col3()
12. void spin_polling()
13. CSE321_LCD LCD(16, 2, LCD_5x8DOTS, PB_9, PB_8)
14. int row = 0
15. int col = 0
16. ready = 0
17. pause = 0
18. time_up = 0
19. int press = 0
20. char Time[5] = {'0',':','0','0',0}


----------
API and Built In Elements Used
----------
IntteruptIn: 

    This creates a reference to an interrupt with a variable name to allow
    additional interaction. This interrupt is triggered by the button, with actions
    on both rise and fall of the signal.
    There are methods to configure the interrupt, specifically: rise and fall.
    https://os.mbed.com/docs/mbed-os/v6.14/apis/interruptin.html

Thread_sleep_for:

    Thread_sleep_for is a time delay function, causes unit delay.
    https://os.mbed.com/docs/mbed-os/v6.12/mbed-os-api-doxy/group__mbed__thread.html#gaaa7efb95f13c2f6055a930fd8ae06942


----------
Custom Functions
----------

isr_col0:

    Description:
        ISR handler of the intterupt col0, rise edge triggered by PD_3
    Inputs:
	None
    Globally referenced things used:
	row, col, press
      
isr_col1:

	Description:
		ISR handler of the intterupt col0, rise edge triggered by PD_2
	Inputs:
		None
	Globally referenced things used:
		row, col, press
      
isr_col2:

	Description:
		ISR handler of the intterupt col0, rise edge triggered by PD_1
	Inputs:
		None
	Globally referenced things used:
		row, col, press

isr_col3:

	Description:
		ISR handler of the intterupt col0, rise edge triggered by PD_0
	Inputs:
		None
	Globally referenced things used:
		row, col, press

spin_polling:

	Description:
		Spin polling to output 1 from each of PD_4, PD_5, PD_6 and PD_7 periodically
	Inputs:
		None
	Globally referenced things used:
		None
