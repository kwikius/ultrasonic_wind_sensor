EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:switches
LIBS:relays
LIBS:motors
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:reversing_sensor_circuit-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 3
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 2775 2325 1200 1600
U 6072429D
F0 "SensorDrivers" 60
F1 "sensor_drivers.sch" 60
F2 "SenseDrive" I R 3975 3600 60 
F3 "Address0" I R 3975 3500 60 
F4 "Address1" I R 3975 3400 60 
F5 "RawSense" O R 3975 2600 60 
$EndSheet
$Sheet
S 4725 1925 1200 850 
U 60743163
F0 "Filter" 60
F1 "filter.sch" 60
F2 "EnvelopeDetect" O R 5925 2450 60 
F3 "RawSense" I L 4725 2600 60 
F4 "ZeroCrossing" O R 5925 2300 60 
$EndSheet
$Comp
L GND #PWR?
U 1 1 60749F13
P 7700 3850
F 0 "#PWR?" H 7700 3600 50  0001 C CNN
F 1 "GND" H 7700 3700 50  0000 C CNN
F 2 "" H 7700 3850 50  0001 C CNN
F 3 "" H 7700 3850 50  0001 C CNN
	1    7700 3850
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR?
U 1 1 60749F4D
P 7700 2600
F 0 "#PWR?" H 7700 2450 50  0001 C CNN
F 1 "+5V" H 7700 2740 50  0000 C CNN
F 2 "" H 7700 2600 50  0001 C CNN
F 3 "" H 7700 2600 50  0001 C CNN
	1    7700 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	8225 3800 7700 3800
Wire Wire Line
	7700 3800 7700 3850
$Comp
L R R21
U 1 1 6074A404
P 4450 3600
F 0 "R21" V 4530 3600 50  0000 C CNN
F 1 "R" V 4450 3600 50  0000 C CNN
F 2 "" V 4380 3600 50  0001 C CNN
F 3 "" H 4450 3600 50  0001 C CNN
	1    4450 3600
	0    1    1    0   
$EndComp
Wire Wire Line
	4300 3600 3975 3600
Wire Wire Line
	7700 3700 8225 3700
Wire Wire Line
	7700 2600 7700 3700
Wire Wire Line
	8225 3600 4600 3600
Wire Wire Line
	4725 2600 3975 2600
Wire Wire Line
	3975 3500 8225 3500
Wire Wire Line
	3975 3400 8225 3400
Wire Wire Line
	8225 3300 6100 3300
Wire Wire Line
	6100 3300 6100 2450
Wire Wire Line
	6100 2450 5925 2450
Wire Wire Line
	5925 2300 6250 2300
Wire Wire Line
	6250 2300 6250 3200
Wire Wire Line
	6250 3200 8225 3200
$Comp
L R R?
U 1 1 607394D3
P 6525 2950
F 0 "R?" V 6605 2950 50  0000 C CNN
F 1 "R" V 6525 2950 50  0000 C CNN
F 2 "" V 6455 2950 50  0001 C CNN
F 3 "" H 6525 2950 50  0001 C CNN
	1    6525 2950
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 607395CF
P 6725 2950
F 0 "R?" V 6805 2950 50  0000 C CNN
F 1 "R" V 6725 2950 50  0000 C CNN
F 2 "" V 6655 2950 50  0001 C CNN
F 3 "" H 6725 2950 50  0001 C CNN
	1    6725 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	6525 2800 6525 2675
Wire Wire Line
	6525 2675 7700 2675
Connection ~ 7700 2675
Wire Wire Line
	6725 2800 6725 2675
Connection ~ 6725 2675
Wire Wire Line
	6525 3100 6525 3400
Connection ~ 6525 3400
Wire Wire Line
	6725 3100 6725 3500
Connection ~ 6725 3500
Text Notes 8700 3525 0    60   ~ 0
Original board PIC mcu connections\n\nSenseDrive connected to pic_mcu pin3\nEnvelopeDetect connected to pic_mcu pin 4\nAddress0 connected to pic_mcu pin 2\nAddress1 connected to pic_mcu pin 7\n
Text Label 6925 3200 0    60   ~ 0
ZeroCrossing
Text Label 6950 3300 0    60   ~ 0
EnvelopeDetect
Text Label 6975 3400 0    60   ~ 0
A1
Text Label 6975 3500 0    60   ~ 0
A0
Text Label 6975 3600 0    60   ~ 0
SenseDrive
Text Label 7700 3700 2    60   ~ 0
5V
Text Label 7700 3800 2    60   ~ 0
GND
$Comp
L Conn_01x09 J?
U 1 1 6082AEDD
P 8425 3400
F 0 "J?" H 8425 3900 50  0000 C CNN
F 1 "Conn_01x09" H 8425 2900 50  0000 C CNN
F 2 "" H 8425 3400 50  0001 C CNN
F 3 "" H 8425 3400 50  0001 C CNN
	1    8425 3400
	1    0    0    1   
$EndComp
NoConn ~ 8225 3000
NoConn ~ 8225 3100
$EndSCHEMATC
