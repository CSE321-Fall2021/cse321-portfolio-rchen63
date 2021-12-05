-------------------
Last Update date
-------------------
12/05/2021

-------------------
About
-------------------
This is Project 3 of CSE 321

A temperature alarm will be designed in this project. The user can input a temperature range between 0 and 50 degree Celsius. If the measured temperature is not in the set range, the buzzer outputs sound to alert people that the temperature is out of range.


--------------------
Features
--------------------
-	Fast response time of the keypad

-	Easy to setup.

-	0 ℃ to 50 ℃. Temperature range

-	Low power consumption

-	Watchdog is set up to reset the system in the case of system failures or malfunctions.

-	EventQueue is used for critical section protection

-	Mutex is used for synchronization

-	Multiple threads are used to allow the different peripherals to run at the same time.




--------------------
Required Materials
--------------------
-Nucleo L4R5ZI

-4x4 Matrix Membrane Keypad

-1602 LCD

-6 LEDs

-Jumper wires

-Resistors

-Breadboard


--------------------
Resources and References
--------------------

IntteruptIn: https://os.mbed.com/docs/mbed-os/v6.14/apis/interruptin.html

Thread_sleep_for: https://os.mbed.com/docs/mbed-os/v6.12/mbed-os-api-doxy/group__mbed__thread.html#gaaa7efb95f13c2f6055a930fd8ae06942
     
STMicroelectronics, UM2179 User Manual: https://www.st.com/resource/en/user_manual/dm00368330-stm32-nucleo-144-boards-mb1312-stmicroelectronics.pdf
    
STM32L4 Reference manual:  https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf


--------------------
Getting Started
--------------------
Keypad: Connect pin 8 to PD_7, pin 7 to PD_6, pin 6 to PD_5, pin 5 to PD_4, pin 4 to PD_3, pin 3 to PD_2, pin 2 to PD_1  and pin 1 to PD_0.

1602LCD: Connect GND to GND, VCC to +5V, SDA to PB_9 and SCL to PB_8.

Load the codes to Nucleo.

Press "A" on keypad to start the timer.

Press "B" on keypad to pause the timer.

Press "D" on keypad to start input the time.

Time is entered as m:ss.


--------------------
CSE321_project2_rchen63_main.cpp:
--------------------
The main program that implement the logic. Spin polling and interrupts are used to interact with the 4x4 matrix keypad. 

The keypad is used for user input, LCD will react to the user input of keypad.

spin_polling is used to output 1 from each of PD_4, PD_5, PD_6 and PD_7 periodically, so that the only one key is detected at a time.
spin_polling has a interval of 75 ms between the outputs, so that pressed key is not repeated twice or more. 
75 ms is used because pressing a key does not take more than 75 ms in normal conditions

Interrupts are rising edge triggered by the keys on the keypad.



----------
Things Declared
----------
1. "mbed.h"
2. "lcd1602.h"
3. char const keypad[4][4] = {{'1','2','3','A'},{'4','5','6','B'},{'7','8','9','C'},{'*','0','#','D'}}
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
16. int ready = 0
17. int pause = 0
18. int time_up = 0
19. int press = 0
20. char Time[5] = {'0',':','0','0',0}


----------
API and Built In Elements Used
----------
lcd1602.cpp and lcd1602.h is provided by Dr. Winikus. The two files contains API that utilized the 1602 LCD to perform different functions like print, setCursor.


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
