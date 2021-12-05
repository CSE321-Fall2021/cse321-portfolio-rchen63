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
-	DHT11 temperature humidity sensor

-	4x4 membrane matrix keypad

-	1602 LCD screen

-	ARCELI Passive Low Level Trigger Buzzer

-	Nucleo L4R5ZI

-	Jumper wires
	-	At least 10 Male to Female jumper wires
	-	At least 20 Male to Male jumper wires

-	Breadboard.



--------------------
Resources and References
--------------------

-	IntteruptIn: https://os.mbed.com/docs/mbed-os/v6.14/apis/interruptin.html

-	Thread_sleep_for: https://os.mbed.com/docs/mbed-os/v6.12/mbed-os-api-doxy/group__mbed__thread.html#gaaa7efb95f13c2f6055a930fd8ae06942
     
-	STMicroelectronics, UM2179 User Manual: https://www.st.com/resource/en/user_manual/dm00368330-stm32-nucleo-144-boards-mb1312-stmicroelectronics.pdf
    
-	STM32L4 Reference manual:  https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf

-	DigitalOut: https://os.mbed.com/docs/mbed-os/v6.8/apis/digitalout.html

-	Watchdog: https://os.mbed.com/docs/mbed-os/v6.15/apis/watchdog.html

-	Mutex: https://os.mbed.com/docs/mbed-os/v6.15/apis/mutex.html

-	Thread: https://os.mbed.com/docs/mbed-os/v6.15/apis/thread.html

-	EventQueue: https://os.mbed.com/docs/mbed-os/v6.15/apis/eventqueue.html

--------------------
Getting Started
--------------------

1.	Mbed Studio is needed to compile the codes into Nucleo L4R5ZI.
2.	Create an empty Mbed OS program.
3.	Download the files listed below into the program folder
	-	CSE321_project3_rchen63_main.cpp
	-	DHT.cpp
	-	DHT.h
	-	lcd1602.cpp
	-	lcd1602.h
	-	mbed_app.json
4.	Connect Nucleo L4R5ZI to the computer.
5.	Click “Run program” to compile the code into Nucleo L4R5ZI.
6.	Connect the 4x4 membrane matrix keypad directly to Nucleo L4R5ZI
	-	Connect pin 8 to PB_7
	-	Connect pin 7 to PB_6
	-	Connect pin 6 to PB_5
	-	Connect pin 5 to PB_4
	-	Connect pin 4 to PB_3
	-	Connect pin 3 to PB_2
	-	Connect pin 2 to PB_1
	-	Connect pin 1 to PB_0
7.	Connect +5V to the power rail with ‘+’ sign on the breadboard, the rail is called VCC rail.
8.	Connect GND to the power rail with ‘-’ sign on the breadboard, the rail is called GND rail.
9.	Connect PB_8, PB_9, PB_10, PB 11 and +3V3 each to a different column
10.	Buzzer
	-	Connect GND to the GND rail
	-	Connect VCC to the column with +3V3
	-	Connect I/O to the column with PB_11
	11.	DHT11
	-	Connect GND to the GND rail
	-	Connect VCC to the VCC rail
	-	Connect OUT to the column with PB_10
12.	1602 LCD
	-	Connect GND to the GND rail
	-	Connect VCC to the VCC rail
	-	Connect SDA to the column with PB_9
	-	Connect SCL to the column with PB_8

- Schematic:
![schematic](https://user-images.githubusercontent.com/70613648/144765018-2d48cfcc-2f99-4044-b870-ee0b31ac7f7f.PNG)

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
