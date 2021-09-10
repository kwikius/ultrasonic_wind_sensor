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
Sheet 3 3
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L GND #PWR?
U 1 1 607437CF
P 3500 3700
F 0 "#PWR?" H 3500 3450 50  0001 C CNN
F 1 "GND" H 3500 3550 50  0000 C CNN
F 2 "" H 3500 3700 50  0001 C CNN
F 3 "" H 3500 3700 50  0001 C CNN
	1    3500 3700
	1    0    0    -1  
$EndComp
$Comp
L R R14
U 1 1 607437D5
P 3500 3475
F 0 "R14" H 3375 3475 50  0000 C CNN
F 1 "330R" V 3500 3475 50  0000 C CNN
F 2 "" V 3430 3475 50  0001 C CNN
F 3 "" H 3500 3475 50  0001 C CNN
	1    3500 3475
	1    0    0    -1  
$EndComp
$Comp
L C C9
U 1 1 607437DC
P 2925 3050
F 0 "C9" V 2825 2875 50  0000 L CNN
F 1 "C" H 2950 2950 50  0000 L CNN
F 2 "" H 2963 2900 50  0001 C CNN
F 3 "" H 2925 3050 50  0001 C CNN
	1    2925 3050
	0    1    1    0   
$EndComp
Wire Wire Line
	3500 3325 3500 3250
Wire Wire Line
	3500 3700 3500 3625
Wire Wire Line
	3075 3050 3200 3050
$Comp
L R R16
U 1 1 607437E7
P 3150 2825
F 0 "R16" H 3025 2850 50  0000 C CNN
F 1 "150K" V 3150 2825 50  0000 C CNN
F 2 "" V 3080 2825 50  0001 C CNN
F 3 "" H 3150 2825 50  0001 C CNN
	1    3150 2825
	1    0    0    -1  
$EndComp
$Comp
L R R15
U 1 1 607437EE
P 3500 2400
F 0 "R15" H 3350 2450 50  0000 C CNN
F 1 "3K9" V 3500 2400 50  0000 C CNN
F 2 "" V 3430 2400 50  0001 C CNN
F 3 "" H 3500 2400 50  0001 C CNN
	1    3500 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 2550 3500 2850
Wire Wire Line
	3150 2975 3150 3050
Connection ~ 3150 3050
Wire Wire Line
	3150 2675 3150 2625
Wire Wire Line
	3150 2625 3500 2625
Connection ~ 3500 2625
$Comp
L +5V #PWR?
U 1 1 607437FD
P 3500 2100
F 0 "#PWR?" H 3500 1950 50  0001 C CNN
F 1 "+5V" H 3500 2240 50  0000 C CNN
F 2 "" H 3500 2100 50  0001 C CNN
F 3 "" H 3500 2100 50  0001 C CNN
	1    3500 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 2250 3500 2100
$Comp
L R R17
U 1 1 60743804
P 2350 3050
F 0 "R17" V 2430 3050 50  0000 C CNN
F 1 "100R" V 2350 3050 50  0000 C CNN
F 2 "" V 2280 3050 50  0001 C CNN
F 3 "" H 2350 3050 50  0001 C CNN
	1    2350 3050
	0    1    1    0   
$EndComp
$Comp
L R R19
U 1 1 6074380B
P 4100 2700
F 0 "R19" V 4000 2675 50  0000 C CNN
F 1 "4K7" V 4100 2700 50  0000 C CNN
F 2 "" V 4030 2700 50  0001 C CNN
F 3 "" H 4100 2700 50  0001 C CNN
	1    4100 2700
	0    1    1    0   
$EndComp
$Comp
L C C8
U 1 1 60743812
P 3725 2700
F 0 "C8" V 3575 2700 50  0000 L CNN
F 1 "C" H 3750 2600 50  0000 L CNN
F 2 "" H 3763 2550 50  0001 C CNN
F 3 "" H 3725 2700 50  0001 C CNN
	1    3725 2700
	0    1    1    0   
$EndComp
$Comp
L R R31
U 1 1 60743819
P 6375 3275
F 0 "R31" H 6225 3275 50  0000 C CNN
F 1 "1K" V 6375 3275 50  0000 C CNN
F 2 "" V 6305 3275 50  0001 C CNN
F 3 "" H 6375 3275 50  0001 C CNN
	1    6375 3275
	1    0    0    -1  
$EndComp
$Comp
L R R20
U 1 1 60743820
P 6375 3850
F 0 "R20" H 6250 3875 50  0000 C CNN
F 1 "1K" V 6375 3850 50  0000 C CNN
F 2 "" V 6305 3850 50  0001 C CNN
F 3 "" H 6375 3850 50  0001 C CNN
	1    6375 3850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 60743827
P 6375 4100
F 0 "#PWR?" H 6375 3850 50  0001 C CNN
F 1 "GND" H 6375 3950 50  0000 C CNN
F 2 "" H 6375 4100 50  0001 C CNN
F 3 "" H 6375 4100 50  0001 C CNN
	1    6375 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6375 4000 6375 4100
Wire Wire Line
	6375 3425 6375 3700
$Comp
L +5V #PWR?
U 1 1 60743831
P 6375 3025
F 0 "#PWR?" H 6375 2875 50  0001 C CNN
F 1 "+5V" H 6375 3165 50  0000 C CNN
F 2 "" H 6375 3025 50  0001 C CNN
F 3 "" H 6375 3025 50  0001 C CNN
	1    6375 3025
	1    0    0    -1  
$EndComp
Wire Wire Line
	6375 3125 6375 3025
$Comp
L LM358 U2
U 2 1 60743838
P 5475 2600
F 0 "U2" H 5475 2800 50  0000 L CNN
F 1 "NE5532" H 5475 2400 50  0000 L CNN
F 2 "" H 5475 2600 50  0001 C CNN
F 3 "" H 5475 2600 50  0001 C CNN
	2    5475 2600
	1    0    0    -1  
$EndComp
$Comp
L R R22
U 1 1 6074383F
P 4425 2975
F 0 "R22" H 4250 3000 50  0000 C CNN
F 1 "750R" V 4425 2975 50  0000 C CNN
F 2 "" V 4355 2975 50  0001 C CNN
F 3 "" H 4425 2975 50  0001 C CNN
	1    4425 2975
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 60743846
P 4425 3250
F 0 "#PWR?" H 4425 3000 50  0001 C CNN
F 1 "GND" H 4425 3100 50  0000 C CNN
F 2 "" H 4425 3250 50  0001 C CNN
F 3 "" H 4425 3250 50  0001 C CNN
	1    4425 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 2700 3875 2700
Wire Wire Line
	4425 2700 4425 2825
Wire Wire Line
	4425 3250 4425 3125
$Comp
L R R26
U 1 1 6074384F
P 5600 2125
F 0 "R26" V 5680 2125 50  0000 C CNN
F 1 "180K" V 5600 2125 50  0000 C CNN
F 2 "" V 5530 2125 50  0001 C CNN
F 3 "" H 5600 2125 50  0001 C CNN
	1    5600 2125
	0    1    1    0   
$EndComp
$Comp
L C C17
U 1 1 60743856
P 4975 1925
F 0 "C17" V 4800 1900 50  0000 L CNN
F 1 "C" H 5000 1825 50  0000 L CNN
F 2 "" H 5013 1775 50  0001 C CNN
F 3 "" H 4975 1925 50  0001 C CNN
	1    4975 1925
	0    1    1    0   
$EndComp
$Comp
L C C10
U 1 1 6074385D
P 4750 2700
F 0 "C10" V 4550 2600 50  0000 L CNN
F 1 "C" H 4775 2600 50  0000 L CNN
F 2 "" H 4788 2550 50  0001 C CNN
F 3 "" H 4750 2700 50  0001 C CNN
	1    4750 2700
	0    1    1    0   
$EndComp
Wire Wire Line
	4900 2700 5175 2700
Wire Wire Line
	4250 2700 4600 2700
Connection ~ 4425 2700
Wire Wire Line
	5000 2700 5000 2125
Wire Wire Line
	5000 2125 5450 2125
Connection ~ 5000 2700
Wire Wire Line
	5775 2600 6025 2600
Wire Wire Line
	5850 1925 5850 2600
Wire Wire Line
	5850 2125 5750 2125
Wire Wire Line
	4525 2700 4525 1925
Wire Wire Line
	4525 1925 4825 1925
Connection ~ 4525 2700
$Comp
L R R23
U 1 1 60743878
P 6550 2600
F 0 "R23" V 6630 2600 50  0000 C CNN
F 1 "470" V 6550 2600 50  0000 C CNN
F 2 "" V 6480 2600 50  0001 C CNN
F 3 "" H 6550 2600 50  0001 C CNN
	1    6550 2600
	0    1    1    0   
$EndComp
$Comp
L C C12
U 1 1 6074387F
P 6175 2600
F 0 "C12" V 5975 2550 50  0000 L CNN
F 1 "C" H 6200 2500 50  0000 L CNN
F 2 "" H 6213 2450 50  0001 C CNN
F 3 "" H 6175 2600 50  0001 C CNN
	1    6175 2600
	0    1    1    0   
$EndComp
$Comp
L LM358 U2
U 1 1 60743886
P 7850 2500
F 0 "U2" H 7850 2700 50  0000 L CNN
F 1 "NE5532" H 7850 2300 50  0000 L CNN
F 2 "" H 7850 2500 50  0001 C CNN
F 3 "" H 7850 2500 50  0001 C CNN
	1    7850 2500
	1    0    0    -1  
$EndComp
$Comp
L R R27
U 1 1 6074388D
P 6800 2875
F 0 "R27" V 6880 2875 50  0000 C CNN
F 1 "510R" V 6800 2875 50  0000 C CNN
F 2 "" V 6730 2875 50  0001 C CNN
F 3 "" H 6800 2875 50  0001 C CNN
	1    6800 2875
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 60743894
P 6800 3150
F 0 "#PWR?" H 6800 2900 50  0001 C CNN
F 1 "GND" H 6800 3000 50  0000 C CNN
F 2 "" H 6800 3150 50  0001 C CNN
F 3 "" H 6800 3150 50  0001 C CNN
	1    6800 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 2600 6325 2600
Wire Wire Line
	6800 2600 6800 2725
Wire Wire Line
	6800 3150 6800 3025
$Comp
L R R30
U 1 1 6074389D
P 7975 2025
F 0 "R30" V 8055 2025 50  0000 C CNN
F 1 "10K" V 7975 2025 50  0000 C CNN
F 2 "" V 7905 2025 50  0001 C CNN
F 3 "" H 7975 2025 50  0001 C CNN
	1    7975 2025
	0    1    1    0   
$EndComp
$Comp
L C C18
U 1 1 607438A4
P 7375 1825
F 0 "C18" V 7175 1725 50  0000 L CNN
F 1 "C" H 7400 1725 50  0000 L CNN
F 2 "" H 7413 1675 50  0001 C CNN
F 3 "" H 7375 1825 50  0001 C CNN
	1    7375 1825
	0    1    1    0   
$EndComp
$Comp
L C C15
U 1 1 607438AB
P 7125 2600
F 0 "C15" V 6925 2550 50  0000 L CNN
F 1 "C" H 7150 2500 50  0000 L CNN
F 2 "" H 7163 2450 50  0001 C CNN
F 3 "" H 7125 2600 50  0001 C CNN
	1    7125 2600
	0    1    1    0   
$EndComp
Wire Wire Line
	7275 2600 7550 2600
Wire Wire Line
	6700 2600 6975 2600
Connection ~ 6800 2600
Wire Wire Line
	7375 2600 7375 2025
Wire Wire Line
	7375 2025 7825 2025
Connection ~ 7375 2600
Wire Wire Line
	8225 2500 8150 2500
Wire Wire Line
	8225 1825 8225 4675
Wire Wire Line
	8225 2025 8125 2025
Wire Wire Line
	6900 2600 6900 1825
Wire Wire Line
	6900 1825 7225 1825
Connection ~ 6900 2600
Wire Wire Line
	7525 1825 8225 1825
Connection ~ 8225 2025
Connection ~ 5850 2600
Wire Wire Line
	7550 2400 7475 2400
Wire Wire Line
	7475 3575 7475 2400
Wire Wire Line
	6375 3575 7475 3575
Connection ~ 6375 3575
Wire Wire Line
	3575 2700 3500 2700
Connection ~ 3500 2700
$Comp
L D_x2_Serial_AKC D6
U 1 1 60743912
P 1775 2900
F 0 "D6" V 1625 2775 50  0000 C CNN
F 1 "BAV99" H 1750 3025 50  0000 C CNN
F 2 "" H 1775 2900 50  0001 C CNN
F 3 "" H 1775 2900 50  0001 C CNN
	1    1775 2900
	0    1    1    0   
$EndComp
$Comp
L GND #PWR?
U 1 1 60743919
P 1475 3000
F 0 "#PWR?" H 1475 2750 50  0001 C CNN
F 1 "GND" H 1475 2850 50  0000 C CNN
F 2 "" H 1475 3000 50  0001 C CNN
F 3 "" H 1475 3000 50  0001 C CNN
	1    1475 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	1475 3000 1475 2900
Wire Wire Line
	1475 2900 1575 2900
Wire Wire Line
	1775 2600 1775 2525
Wire Wire Line
	1775 2525 2025 2525
Wire Wire Line
	2025 2525 2025 3325
Wire Wire Line
	2025 3325 1775 3325
Wire Wire Line
	1775 3325 1775 3200
Wire Wire Line
	2200 3050 2025 3050
Connection ~ 2025 3050
Wire Wire Line
	2500 3050 2775 3050
Wire Wire Line
	1875 2400 1925 2400
Wire Wire Line
	1925 2400 1925 2525
Connection ~ 1925 2525
Wire Wire Line
	5175 2500 5100 2500
Wire Wire Line
	5100 2500 5100 3500
Wire Wire Line
	5100 3500 6375 3500
Connection ~ 6375 3500
$Comp
L Q_NPN_BEC Q6
U 1 1 60743939
P 3400 3050
F 0 "Q6" H 3600 3100 50  0000 L CNN
F 1 "2SC1623-L6" H 3600 3000 50  0000 L CNN
F 2 "" H 3600 3150 50  0001 C CNN
F 3 "" H 3400 3050 50  0001 C CNN
	1    3400 3050
	1    0    0    -1  
$EndComp
Text HLabel 1875 2400 0    60   Input ~ 0
RawSense
$Comp
L LM339 U1001
U 1 1 607337F3
P 9400 5150
F 0 "U1001" H 9400 5350 50  0000 L CNN
F 1 "LM6132" H 9400 4950 50  0000 L CNN
F 2 "" H 9350 5250 50  0001 C CNN
F 3 "" H 9450 5350 50  0001 C CNN
	1    9400 5150
	1    0    0    -1  
$EndComp
$Comp
L R R1002
U 1 1 60734958
P 9050 4675
F 0 "R1002" V 9130 4675 50  0000 C CNN
F 1 "100K" V 9050 4675 50  0000 C CNN
F 2 "" V 8980 4675 50  0001 C CNN
F 3 "" H 9050 4675 50  0001 C CNN
	1    9050 4675
	0    1    1    0   
$EndComp
Wire Wire Line
	9700 5150 9900 5150
Wire Wire Line
	9775 4675 9775 5150
Wire Wire Line
	9775 4675 9200 4675
Wire Wire Line
	8225 4675 8325 4675
Wire Wire Line
	8625 4675 8900 4675
Text HLabel 9900 5150 2    60   Output ~ 0
ZeroCrossing
Connection ~ 9775 5150
Wire Wire Line
	5125 1925 5850 1925
Connection ~ 5850 2125
$Comp
L C C11
U 1 1 60737A5C
P 5875 3825
F 0 "C11" H 5600 3900 50  0000 L CNN
F 1 "C" H 5900 3725 50  0000 L CNN
F 2 "" H 5913 3675 50  0001 C CNN
F 3 "" H 5875 3825 50  0001 C CNN
	1    5875 3825
	1    0    0    -1  
$EndComp
Wire Wire Line
	5875 3675 5875 3500
Connection ~ 5875 3500
Wire Wire Line
	5375 4075 6375 4075
Wire Wire Line
	5875 4075 5875 3975
Connection ~ 6375 4075
Wire Wire Line
	9100 5050 8750 5050
Wire Wire Line
	8750 4675 8750 5400
Connection ~ 8750 4675
Wire Wire Line
	9100 5250 6650 5250
Wire Wire Line
	6650 3575 6650 6525
Connection ~ 6650 3575
$Comp
L R R1001
U 1 1 607389C4
P 8475 4675
F 0 "R1001" V 8555 4675 50  0000 C CNN
F 1 "10K" V 8475 4675 50  0000 C CNN
F 2 "" V 8405 4675 50  0001 C CNN
F 3 "" H 8475 4675 50  0001 C CNN
	1    8475 4675
	0    1    1    0   
$EndComp
Wire Wire Line
	5375 4075 5375 2900
Connection ~ 5875 4075
Wire Wire Line
	6375 4025 7750 4025
Wire Wire Line
	7750 4025 7750 2800
Connection ~ 6375 4025
$Comp
L +5V #PWR?
U 1 1 6073A7F4
P 5375 1750
F 0 "#PWR?" H 5375 1600 50  0001 C CNN
F 1 "+5V" H 5375 1890 50  0000 C CNN
F 2 "" H 5375 1750 50  0001 C CNN
F 3 "" H 5375 1750 50  0001 C CNN
	1    5375 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5375 2300 5375 1750
$Comp
L +5V #PWR?
U 1 1 6073ABE0
P 7750 1650
F 0 "#PWR?" H 7750 1500 50  0001 C CNN
F 1 "+5V" H 7750 1790 50  0000 C CNN
F 2 "" H 7750 1650 50  0001 C CNN
F 3 "" H 7750 1650 50  0001 C CNN
	1    7750 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	7750 2200 7750 1650
$Comp
L +5V #PWR?
U 1 1 6073AEE7
P 9300 4575
F 0 "#PWR?" H 9300 4425 50  0001 C CNN
F 1 "+5V" H 9300 4715 50  0000 C CNN
F 2 "" H 9300 4575 50  0001 C CNN
F 3 "" H 9300 4575 50  0001 C CNN
	1    9300 4575
	1    0    0    -1  
$EndComp
Wire Wire Line
	9300 4850 9300 4575
$Comp
L GND #PWR?
U 1 1 6073B353
P 9300 5975
F 0 "#PWR?" H 9300 5725 50  0001 C CNN
F 1 "GND" H 9300 5825 50  0000 C CNN
F 2 "" H 9300 5975 50  0001 C CNN
F 3 "" H 9300 5975 50  0001 C CNN
	1    9300 5975
	1    0    0    -1  
$EndComp
Wire Wire Line
	9300 5450 9300 5975
$Comp
L R R1003
U 1 1 607551D6
P 8750 5550
F 0 "R1003" V 8830 5550 50  0000 C CNN
F 1 "100K" V 8750 5550 50  0000 C CNN
F 2 "" V 8680 5550 50  0001 C CNN
F 3 "" H 8750 5550 50  0001 C CNN
	1    8750 5550
	1    0    0    -1  
$EndComp
Connection ~ 8750 5050
Wire Wire Line
	8750 5700 8750 5900
Wire Wire Line
	8750 5900 9300 5900
Connection ~ 9300 5900
Text Notes 9900 4975 0    60   ~ 0
Zero crossing \non falling edge
$Comp
L R R?
U 1 1 613A4C0A
P 3825 5550
F 0 "R?" V 3905 5550 50  0000 C CNN
F 1 "100K" V 3825 5550 50  0000 C CNN
F 2 "" V 3755 5550 50  0001 C CNN
F 3 "" H 3825 5550 50  0001 C CNN
	1    3825 5550
	0    1    1    0   
$EndComp
Wire Wire Line
	4425 6025 4625 6025
Wire Wire Line
	4500 5550 4500 6025
Wire Wire Line
	4500 5550 3975 5550
Text HLabel 4625 6025 2    60   Output ~ 0
ZeroCrossing
Connection ~ 4500 6025
Wire Wire Line
	3475 5925 3825 5925
Wire Wire Line
	3475 5450 3475 6275
$Comp
L R R?
U 1 1 613A4C1F
P 3175 5875
F 0 "R?" V 3255 5875 50  0000 C CNN
F 1 "10K" V 3175 5875 50  0000 C CNN
F 2 "" V 3105 5875 50  0001 C CNN
F 3 "" H 3175 5875 50  0001 C CNN
	1    3175 5875
	0    1    1    0   
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 613A4C25
P 4025 4900
F 0 "#PWR?" H 4025 4750 50  0001 C CNN
F 1 "+3.3V" H 4025 5040 50  0000 C CNN
F 2 "" H 4025 4900 50  0001 C CNN
F 3 "" H 4025 4900 50  0001 C CNN
	1    4025 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4025 4900 4025 5725
$Comp
L GND #PWR?
U 1 1 613A4C2C
P 4025 6850
F 0 "#PWR?" H 4025 6600 50  0001 C CNN
F 1 "GND" H 4025 6700 50  0000 C CNN
F 2 "" H 4025 6850 50  0001 C CNN
F 3 "" H 4025 6850 50  0001 C CNN
	1    4025 6850
	1    0    0    -1  
$EndComp
Wire Wire Line
	4025 6325 4025 6850
$Comp
L R R?
U 1 1 613A4C34
P 3475 6425
F 0 "R?" V 3555 6425 50  0000 C CNN
F 1 "100K" V 3475 6425 50  0000 C CNN
F 2 "" V 3405 6425 50  0001 C CNN
F 3 "" H 3475 6425 50  0001 C CNN
	1    3475 6425
	1    0    0    -1  
$EndComp
Connection ~ 3475 5925
Wire Wire Line
	3475 6575 3475 6775
Wire Wire Line
	3475 6775 4025 6775
Connection ~ 4025 6775
Text Notes 4625 5850 0    60   ~ 0
Zero crossing \non falling edge
$Comp
L R R?
U 1 1 613A54A4
P 3475 5300
F 0 "R?" V 3555 5300 50  0000 C CNN
F 1 "R" V 3475 5300 50  0000 C CNN
F 2 "" V 3405 5300 50  0001 C CNN
F 3 "" H 3475 5300 50  0001 C CNN
	1    3475 5300
	1    0    0    -1  
$EndComp
$Comp
L LM339 U?
U 1 1 613A561A
P 4125 6025
F 0 "U?" H 4125 6225 50  0000 L CNN
F 1 "LM339" H 4125 5825 50  0000 L CNN
F 2 "" H 4075 6125 50  0001 C CNN
F 3 "" H 4175 6225 50  0001 C CNN
	1    4125 6025
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 5925 3600 5550
Wire Wire Line
	3600 5550 3675 5550
Connection ~ 3600 5925
Wire Wire Line
	3325 5875 3475 5875
Connection ~ 3475 5875
Text Notes 4225 5450 0    60   ~ 0
Open Collector output for 3.3v version
Connection ~ 8225 2500
Wire Wire Line
	3025 5875 2800 5875
Wire Wire Line
	2800 5875 2800 4550
Wire Wire Line
	2800 4550 8225 4550
Connection ~ 8225 4550
Wire Wire Line
	3825 6125 3725 6125
Wire Wire Line
	3725 6125 3725 6525
Wire Wire Line
	3725 6525 6650 6525
Connection ~ 6650 5250
$Comp
L +5V #PWR?
U 1 1 613A7443
P 3475 4450
F 0 "#PWR?" H 3475 4300 50  0001 C CNN
F 1 "+5V" H 3475 4590 50  0000 C CNN
F 2 "" H 3475 4450 50  0001 C CNN
F 3 "" H 3475 4450 50  0001 C CNN
	1    3475 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3475 5150 3475 4450
$EndSCHEMATC
