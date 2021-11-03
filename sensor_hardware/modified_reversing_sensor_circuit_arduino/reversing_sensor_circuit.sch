EESchema Schematic File Version 2
LIBS:mcp6561
LIBS:reversing_sensor_circuit-rescue
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
LIBS:atmega328p
LIBS:esp-01
LIBS:reversing_sensor_circuit-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 5
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
F3 "Address0" I R 3975 3375 60 
F4 "Address1" I R 3975 3275 60 
F5 "RawSense" O R 3975 2600 60 
$EndSheet
$Sheet
S 4725 1925 1200 850 
U 60743163
F0 "Filter" 60
F1 "filter.sch" 60
F2 "RawSense" I L 4725 2600 60 
F3 "RxSignalGnd" O R 5925 2450 60 
F4 "RxSignal" O R 5925 2350 60 
$EndSheet
Wire Wire Line
	4725 2600 3975 2600
$Sheet
S 6525 2550 2225 1750
U 61407FAF
F0 "atmega328_mcu" 60
F1 "atmega328p_mcu.sch" 60
F2 "RxSignalGnd" I L 6525 3000 60 
F3 "RxSignal" I L 6525 2875 60 
F4 "Address0" O L 6525 3375 60 
F5 "Address1" O L 6525 3275 60 
F6 "SenseDrive" O L 6525 3600 60 
F7 "ArduinoRST" I L 6525 4100 60 
F8 "WifiTXO" O L 6525 3800 60 
F9 "RXI" I L 6525 3950 60 
$EndSheet
Wire Wire Line
	5925 2350 6325 2350
Wire Wire Line
	6325 2350 6325 2875
Wire Wire Line
	6325 2875 6525 2875
Wire Wire Line
	5925 2450 6225 2450
Wire Wire Line
	6225 2450 6225 3000
Wire Wire Line
	6225 3000 6525 3000
Wire Wire Line
	6525 3600 3975 3600
Wire Wire Line
	6525 3375 3975 3375
Wire Wire Line
	6525 3275 3975 3275
$Sheet
S 4750 4225 1175 575 
U 617533DD
F0 "wifi" 60
F1 "wifi.sch" 60
F2 "RXI" I R 5925 4325 60 
F3 "ArduinoRST" O R 5925 4600 60 
F4 "ArduinoTXO" O R 5925 4450 60 
$EndSheet
Wire Wire Line
	5925 4325 6075 4325
Wire Wire Line
	6075 4325 6075 3800
Wire Wire Line
	6075 3800 6525 3800
Wire Wire Line
	6525 3950 6175 3950
Wire Wire Line
	6175 3950 6175 4450
Wire Wire Line
	6175 4450 5925 4450
Wire Wire Line
	5925 4600 6275 4600
Wire Wire Line
	6275 4600 6275 4100
Wire Wire Line
	6275 4100 6525 4100
$EndSCHEMATC
