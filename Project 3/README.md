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
3.	Download the files listed below into the program folder, and delete the original main.cpp
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
The main program that implement the logic. 
Multiple threads are created for parallel tasks. 
Watchdog is set up to reset the system in the case of system failures or malfunctions.
Mutex is set up for synchronization.
EventQueue is set up for critical section protection.
Four interrupts are set up for four columns of the matrix keypad.
Bitwise driver control is used to control the output of rows of the keypad.



----------
Things Declared
----------
1. "lcd1602.cpp"
2. "lcd1602.h"
3. "DHT.cpp"
4. "DHT.h"
5.  InterruptIn col0(PD_3);     
6.  InterruptIn col1(PD_2);
7.  InterruptIn col2(PD_1);
8.  InterruptIn col3(PD_0);
9.  isr_col0();
10.  isr_col1();
11.  isr_col2();
12.  isr_col3(); 
13.  spin_polling();
14.  void readBuzzer();
15.  void read_temperature();
16.  int row = 0;     
17.  int col = 0;     
18.  int press = 0;   
19.  int pressC=0;     
20.  int lowerBound = 10;    
21.  int upperBound = 35;    
22.  int currentTemp =25;        
23.  int errorTemp = 0;
24.  char const keypad[4][4] = {{'1','2','3','A'},{'4','5','6','B'},{'7','8','9','C'},{'*','0','#','D'}}
25.  char set_range[7]={'1','0','C','-','3','5','C'};
26.  Mutex lock;
27.  Thread polling;
28.  Thread readTemp;
29.  Thread buzzerIO;
30.  Thread interrupts;

----------
API and Built In Elements Used
----------
-	lcd1602.cpp and lcd1602.h is provided by Dr. Winikus. The two files contains API that utilized the 1602 LCD to perform different functions like print, setCursor.

-	DHT.cpp and DHT.h is provided by Dr. Winikus. The two files contains API that utilized the DHT11 temperature humidity sensor to perform different functions like 
	read, getCelsius.

-	IntteruptIn: https://os.mbed.com/docs/mbed-os/v6.14/apis/interruptin.html

-	Thread_sleep_for: https://os.mbed.com/docs/mbed-os/v6.12/mbed-os-api-doxy/group__mbed__thread.html#gaaa7efb95f13c2f6055a930fd8ae06942
     
-	STMicroelectronics, UM2179 User Manual: https://www.st.com/resource/en/user_manual/dm00368330-stm32-nucleo-144-boards-mb1312-stmicroelectronics.pdf
    
-	STM32L4 Reference manual:  https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf

-	DigitalOut: https://os.mbed.com/docs/mbed-os/v6.8/apis/digitalout.html

-	Watchdog: https://os.mbed.com/docs/mbed-os/v6.15/apis/watchdog.html

-	Mutex: https://os.mbed.com/docs/mbed-os/v6.15/apis/mutex.html

-	Thread: https://os.mbed.com/docs/mbed-os/v6.15/apis/thread.html

-	EventQueue: https://os.mbed.com/docs/mbed-os/v6.15/apis/eventqueue.html

----------
Custom Functions
----------

isr_col1:

	Description:
		ISR handler of the intterupt col0, rise edge triggered by PD_3
	Inputs:
		None
	Globally referenced things used:
		row, col, press
      
isr_col1:

	Description:
		ISR handler of the intterupt col1, rise edge triggered by PD_2
	Inputs:
		None
	Globally referenced things used:
		row, col, press
      
isr_col2:

	Description:
		ISR handler of the intterupt col2, rise edge triggered by PD_1
	Inputs:
		None
	Globally referenced things used:
		row, col, press

isr_col3:

	Description:
		ISR handler of the intterupt col3, rise edge triggered by PD_0
	Inputs:
		None
	Globally referenced things used:
		row, col, press

spin_polling:

	Description:
		Output 1 from each of PD_4, PD_5, PD_6 and PD_7 periodically at a 75 ms intervals
	Inputs:
		None
	Globally referenced things used:
		None
		
read_temperature:

	Description:
		Read the current temperautre while the user is not inputting new temperature range.
		And display the measured temperature on the LCD screen.
	Inputs:
		None
	Globally referenced things used:
		currentTemp, errorTemp, LCD, lock, DHT
		
readBuzzer:

	Description:
		Spin polling to check if the current temperature is out of the set temperature range.
		If the temperature is out of the range, the buzzer start alerting
	Inputs:
		None
	Globally referenced things used:
		currentTemp, lowerBound, upperBound, pressC, buzzer
