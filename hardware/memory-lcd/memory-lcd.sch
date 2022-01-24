EESchema Schematic File Version 4
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
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
L riscv-serial:NHD-0220FZ J1
U 1 1 5EE4A207
P 9200 2100
F 0 "J1" H 10250 1350 50  0000 L CNN
F 1 "NHD-0220FZ" H 9200 2100 50  0000 C CNN
F 2 "riscv-serial:NHD-0220FZ" H 9200 2100 50  0001 C CNN
F 3 "" H 9200 2100 50  0001 C CNN
	1    9200 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 6200 1400 6200
Wire Wire Line
	1900 6300 1400 6300
Wire Wire Line
	1900 6400 1400 6400
Wire Wire Line
	7350 2450 7850 2450
Text Label 7350 2450 0    50   ~ 0
A0
Text Label 1900 6400 2    50   ~ 0
A1
Text Label 1900 6300 2    50   ~ 0
A2
Text Label 1900 6200 2    50   ~ 0
A3
Wire Wire Line
	1900 6100 1400 6100
Wire Wire Line
	1900 6000 1400 6000
Text Label 1900 6100 2    50   ~ 0
A4
Text Label 1900 6000 2    50   ~ 0
A5
Text Label 1900 5900 2    50   ~ 0
A6
Text Label 1900 5800 2    50   ~ 0
A7
Text Label 1900 5700 2    50   ~ 0
A8
Text Label 1900 5600 2    50   ~ 0
A9
Text Label 1900 5500 2    50   ~ 0
A10
Text Label 1900 5400 2    50   ~ 0
A11
Text Label 1900 5300 2    50   ~ 0
A12
Text Label 1900 5200 2    50   ~ 0
A13
Text Label 1900 5100 2    50   ~ 0
A14
Text Label 1900 5000 2    50   ~ 0
A15
Wire Wire Line
	1900 4600 1400 4600
Text Label 1900 4900 2    50   ~ 0
A16
Text Label 1900 4800 2    50   ~ 0
A17
Text Label 1900 4700 2    50   ~ 0
A18
Text Label 1900 4600 2    50   ~ 0
A19
Text Label 1900 4500 2    50   ~ 0
A20
Text Label 1900 4400 2    50   ~ 0
A21
Text Label 1900 4300 2    50   ~ 0
A22
Text Label 1900 4200 2    50   ~ 0
A23
Text Label 1900 3800 2    50   ~ 0
~WAIT
Text Label 1900 3700 2    50   ~ 0
~MEM_OE
Text Label 1900 3600 2    50   ~ 0
~MEM_WE
Wire Wire Line
	1900 3600 1400 3600
Wire Wire Line
	1900 3700 1400 3700
Text Label 1900 3500 2    50   ~ 0
D0
Text Label 1900 3400 2    50   ~ 0
D1
Text Label 1900 3300 2    50   ~ 0
D2
Text Label 1900 3200 2    50   ~ 0
D3
Text Label 1900 3100 2    50   ~ 0
D4
Text Label 1900 3000 2    50   ~ 0
D5
Text Label 1900 2900 2    50   ~ 0
D6
Text Label 1900 2800 2    50   ~ 0
D7
Wire Wire Line
	1400 3500 1900 3500
Wire Wire Line
	1900 3400 1400 3400
Wire Wire Line
	1400 3300 1900 3300
Wire Wire Line
	1900 3200 1400 3200
Wire Wire Line
	1400 3100 1900 3100
Wire Wire Line
	1900 3000 1400 3000
Wire Wire Line
	1400 2900 1900 2900
Wire Wire Line
	1900 2800 1400 2800
$Comp
L power:GND #PWR?
U 1 1 5EE549DA
P 1550 6700
AR Path="/5ED0998A/5EE549DA" Ref="#PWR?"  Part="1" 
AR Path="/5EE549DA" Ref="#PWR06"  Part="1" 
AR Path="/5EE4B953/5EE549DA" Ref="#PWR?"  Part="1" 
F 0 "#PWR06" H 1550 6450 50  0001 C CNN
F 1 "GND" H 1555 6527 50  0000 C CNN
F 2 "" H 1550 6700 50  0001 C CNN
F 3 "" H 1550 6700 50  0001 C CNN
	1    1550 6700
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1900 5900 1400 5900
Wire Wire Line
	1900 5500 1400 5500
Wire Wire Line
	1900 5600 1400 5600
Wire Wire Line
	1900 5700 1400 5700
Wire Wire Line
	1900 5800 1400 5800
Wire Wire Line
	1900 5100 1400 5100
Wire Wire Line
	1900 5200 1400 5200
Wire Wire Line
	1900 5300 1400 5300
Wire Wire Line
	1900 5400 1400 5400
Wire Wire Line
	1900 4900 1400 4900
Wire Wire Line
	1900 5000 1400 5000
Wire Wire Line
	1900 4700 1400 4700
Wire Wire Line
	1900 4800 1400 4800
Text Label 1900 4000 2    50   ~ 0
A31
Text Label 1900 4100 2    50   ~ 0
A30
Wire Wire Line
	1900 4000 1400 4000
Wire Wire Line
	1400 4100 1900 4100
Wire Wire Line
	1400 6600 1550 6600
Wire Wire Line
	1550 6600 1550 6700
$Comp
L riscv-serial:Conn_Mem_Ext J?
U 1 1 5EE549F3
P 1150 4600
AR Path="/5EE4B953/5EE549F3" Ref="J?"  Part="1" 
AR Path="/5EE549F3" Ref="J2"  Part="1" 
F 0 "J2" H 1123 6725 50  0000 C CNN
F 1 "Conn_Mem_Ext" H 1123 6634 50  0000 C CNN
F 2 "Connector_IDC:IDC-Header_2x20_P2.54mm_Vertical" H 1150 4500 50  0001 C CNN
F 3 "~" H 1150 4500 50  0001 C CNN
	1    1150 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 4500 1400 4500
Wire Wire Line
	1400 4400 2050 4400
Wire Wire Line
	1400 4300 2050 4300
Wire Wire Line
	1400 4200 2050 4200
NoConn ~ 2050 4200
NoConn ~ 2050 4300
NoConn ~ 2050 4400
Wire Wire Line
	1550 6600 1550 2700
Wire Wire Line
	1550 2700 1400 2700
Connection ~ 1550 6600
Wire Wire Line
	1400 3800 2050 3800
NoConn ~ 2050 3800
Wire Wire Line
	1400 3900 2050 3900
NoConn ~ 2050 3900
Text Label 1900 3900 2    50   ~ 0
~RESET
$Comp
L riscv-serial:Conn_Power J?
U 1 1 5EE55C3C
P 3400 6500
AR Path="/5ED0998A/5EE55C3C" Ref="J?"  Part="1" 
AR Path="/5EE55C3C" Ref="J3"  Part="1" 
AR Path="/5EE4B953/5EE55C3C" Ref="J?"  Part="1" 
F 0 "J3" H 3373 6775 50  0000 C CNN
F 1 "Conn_Power" H 3373 6684 50  0000 C CNN
F 2 "Connector_JST:JST_PH_S2B-PH-SM4-TB_1x02-1MP_P2.00mm_Horizontal" H 3400 6600 50  0001 C CNN
F 3 "" H 3400 6600 50  0001 C CNN
	1    3400 6500
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR?
U 1 1 5EE55C43
P 5150 6400
AR Path="/5ED0998A/5EE55C43" Ref="#PWR?"  Part="1" 
AR Path="/5EE55C43" Ref="#PWR05"  Part="1" 
AR Path="/5EE4B953/5EE55C43" Ref="#PWR?"  Part="1" 
F 0 "#PWR05" H 5150 6250 50  0001 C CNN
F 1 "VCC" H 5167 6573 50  0000 C CNN
F 2 "" H 5150 6400 50  0001 C CNN
F 3 "" H 5150 6400 50  0001 C CNN
	1    5150 6400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 6400 5150 6450
Wire Wire Line
	5150 6450 5000 6450
$Comp
L power:GND #PWR?
U 1 1 5EE55C4B
P 3400 7050
AR Path="/5ED0998A/5EE55C4B" Ref="#PWR?"  Part="1" 
AR Path="/5EE55C4B" Ref="#PWR07"  Part="1" 
AR Path="/5EE4B953/5EE55C4B" Ref="#PWR?"  Part="1" 
F 0 "#PWR07" H 3400 6800 50  0001 C CNN
F 1 "GND" H 3405 6877 50  0000 C CNN
F 2 "" H 3400 7050 50  0001 C CNN
F 3 "" H 3400 7050 50  0001 C CNN
	1    3400 7050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 6550 3600 6550
Wire Wire Line
	3600 6550 3600 6900
Wire Wire Line
	3400 6750 3400 6900
Wire Wire Line
	3400 6900 3600 6900
Connection ~ 3600 6900
$Comp
L Regulator_Linear:MIC5504-3.3YM5 U?
U 1 1 5EE55C56
P 4600 6550
AR Path="/5ED0998A/5EE55C56" Ref="U?"  Part="1" 
AR Path="/5EE55C56" Ref="U3"  Part="1" 
AR Path="/5EE4B953/5EE55C56" Ref="U?"  Part="1" 
F 0 "U3" H 4600 6917 50  0000 C CNN
F 1 "MIC5504-3.3YM5" H 4600 6826 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5_HandSoldering" H 4600 6150 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/MIC550X.pdf" H 4350 6800 50  0001 C CNN
	1    4600 6550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 6450 4100 6450
Wire Wire Line
	4200 6650 4100 6650
Wire Wire Line
	4100 6650 4100 6450
Connection ~ 4100 6450
Wire Wire Line
	4100 6450 3800 6450
$Comp
L power:GND #PWR?
U 1 1 5EE55C62
P 4600 7050
AR Path="/5ED0998A/5EE55C62" Ref="#PWR?"  Part="1" 
AR Path="/5EE55C62" Ref="#PWR08"  Part="1" 
AR Path="/5EE4B953/5EE55C62" Ref="#PWR?"  Part="1" 
F 0 "#PWR08" H 4600 6800 50  0001 C CNN
F 1 "GND" H 4605 6877 50  0000 C CNN
F 2 "" H 4600 7050 50  0001 C CNN
F 3 "" H 4600 7050 50  0001 C CNN
	1    4600 7050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 6850 4600 6900
$Comp
L Device:C C?
U 1 1 5EE55C69
P 3800 6650
AR Path="/5ED0998A/5EE55C69" Ref="C?"  Part="1" 
AR Path="/5EE55C69" Ref="C1"  Part="1" 
AR Path="/5EE4B953/5EE55C69" Ref="C?"  Part="1" 
F 0 "C1" H 3850 6550 50  0000 L CNN
F 1 "1u" H 3850 6750 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 3838 6500 50  0001 C CNN
F 3 "~" H 3800 6650 50  0001 C CNN
	1    3800 6650
	1    0    0    1   
$EndComp
Wire Wire Line
	3800 6450 3800 6500
Connection ~ 3800 6450
Wire Wire Line
	3800 6450 3550 6450
Wire Wire Line
	3800 6800 3800 6900
Wire Wire Line
	3800 6900 3600 6900
$Comp
L Device:C C?
U 1 1 5EE55C75
P 5150 6650
AR Path="/5ED0998A/5EE55C75" Ref="C?"  Part="1" 
AR Path="/5EE55C75" Ref="C2"  Part="1" 
AR Path="/5EE4B953/5EE55C75" Ref="C?"  Part="1" 
F 0 "C2" H 5200 6550 50  0000 L CNN
F 1 "1u" H 5200 6750 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 5188 6500 50  0001 C CNN
F 3 "~" H 5150 6650 50  0001 C CNN
	1    5150 6650
	1    0    0    1   
$EndComp
Wire Wire Line
	5150 6450 5150 6500
Connection ~ 5150 6450
Wire Wire Line
	5150 6800 5150 6900
Wire Wire Line
	5150 6900 4600 6900
Connection ~ 4600 6900
Wire Wire Line
	4600 6900 4600 7050
Wire Wire Line
	5150 6450 5450 6450
Wire Wire Line
	5150 6900 5450 6900
Connection ~ 5150 6900
$Comp
L power:PWR_FLAG #FLG?
U 1 1 5EE55C85
P 3800 7050
AR Path="/5EE4B953/5EE55C85" Ref="#FLG?"  Part="1" 
AR Path="/5EE55C85" Ref="#FLG02"  Part="1" 
F 0 "#FLG02" H 3800 7125 50  0001 C CNN
F 1 "PWR_FLAG" H 3800 7223 50  0000 C CNN
F 2 "" H 3800 7050 50  0001 C CNN
F 3 "~" H 3800 7050 50  0001 C CNN
	1    3800 7050
	-1   0    0    1   
$EndComp
Wire Wire Line
	3400 7050 3400 6900
Connection ~ 3400 6900
Wire Wire Line
	3800 6350 3800 6450
$Comp
L power:PWR_FLAG #FLG?
U 1 1 5EE55C8E
P 3800 6350
AR Path="/5EE4B953/5EE55C8E" Ref="#FLG?"  Part="1" 
AR Path="/5EE55C8E" Ref="#FLG01"  Part="1" 
F 0 "#FLG01" H 3800 6425 50  0001 C CNN
F 1 "PWR_FLAG" H 3800 6524 50  0000 C CNN
F 2 "" H 3800 6350 50  0001 C CNN
F 3 "~" H 3800 6350 50  0001 C CNN
	1    3800 6350
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 6900 3800 7050
Connection ~ 3800 6900
Text Label 3600 6450 0    50   ~ 0
V_IN
$Comp
L Mechanical:MountingHole H?
U 1 1 5EE57652
P 10300 5650
AR Path="/5ED0998A/5EE57652" Ref="H?"  Part="1" 
AR Path="/5EE57652" Ref="H1"  Part="1" 
AR Path="/5EE4B953/5EE57652" Ref="H?"  Part="1" 
F 0 "H1" H 10400 5696 50  0000 L CNN
F 1 "MountingHole" H 10400 5605 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 10300 5650 50  0001 C CNN
F 3 "~" H 10300 5650 50  0001 C CNN
	1    10300 5650
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H?
U 1 1 5EE57659
P 10300 5850
AR Path="/5ED0998A/5EE57659" Ref="H?"  Part="1" 
AR Path="/5EE57659" Ref="H2"  Part="1" 
AR Path="/5EE4B953/5EE57659" Ref="H?"  Part="1" 
F 0 "H2" H 10400 5896 50  0000 L CNN
F 1 "MountingHole" H 10400 5805 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 10300 5850 50  0001 C CNN
F 3 "~" H 10300 5850 50  0001 C CNN
	1    10300 5850
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H?
U 1 1 5EE57660
P 10300 6050
AR Path="/5ED0998A/5EE57660" Ref="H?"  Part="1" 
AR Path="/5EE57660" Ref="H3"  Part="1" 
AR Path="/5EE4B953/5EE57660" Ref="H?"  Part="1" 
F 0 "H3" H 10400 6096 50  0000 L CNN
F 1 "MountingHole" H 10400 6005 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 10300 6050 50  0001 C CNN
F 3 "~" H 10300 6050 50  0001 C CNN
	1    10300 6050
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H?
U 1 1 5EE57667
P 10300 6250
AR Path="/5ED0998A/5EE57667" Ref="H?"  Part="1" 
AR Path="/5EE57667" Ref="H4"  Part="1" 
AR Path="/5EE4B953/5EE57667" Ref="H?"  Part="1" 
F 0 "H4" H 10400 6296 50  0000 L CNN
F 1 "MountingHole" H 10400 6205 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 10300 6250 50  0001 C CNN
F 3 "~" H 10300 6250 50  0001 C CNN
	1    10300 6250
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS688 U1
U 1 1 5EE58F8C
P 4400 3900
F 0 "U1" H 4600 2850 50  0000 L CNN
F 1 "74HC688" V 4400 3900 50  0000 C CNN
F 2 "Package_SO:SO-20_12.8x7.5mm_P1.27mm" H 4400 3900 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS688" H 4400 3900 50  0001 C CNN
	1    4400 3900
	1    0    0    -1  
$EndComp
$Comp
L riscv-serial:74LVC1G08 U2
U 1 1 5EE5F490
P 3400 5300
F 0 "U2" H 3400 5300 50  0000 C CNN
F 1 "74LVC1G08" H 3400 5050 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5_HandSoldering" H 3400 5350 50  0001 C CNN
F 3 "" H 3400 5350 50  0001 C CNN
	1    3400 5300
	1    0    0    -1  
$EndComp
$Comp
L riscv-serial:74LVC1G08 U2
U 2 1 5EE5F516
P 8150 5700
F 0 "U2" H 8280 5746 50  0000 L CNN
F 1 "74LVC1G08" H 8280 5655 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5_HandSoldering" H 8150 5750 50  0001 C CNN
F 3 "" H 8150 5750 50  0001 C CNN
	2    8150 5700
	1    0    0    -1  
$EndComp
Text Label 2700 5400 0    50   ~ 0
~MEM_WE
Wire Wire Line
	2700 5200 3100 5200
Text Label 2700 5200 0    50   ~ 0
~MEM_OE
Wire Wire Line
	2700 5400 3100 5400
$Comp
L power:VCC #PWR?
U 1 1 5EE6408C
P 4400 2600
AR Path="/5ED0998A/5EE6408C" Ref="#PWR?"  Part="1" 
AR Path="/5EE6408C" Ref="#PWR01"  Part="1" 
AR Path="/5EE4B953/5EE6408C" Ref="#PWR?"  Part="1" 
F 0 "#PWR01" H 4400 2450 50  0001 C CNN
F 1 "VCC" H 4417 2773 50  0000 C CNN
F 2 "" H 4400 2600 50  0001 C CNN
F 3 "" H 4400 2600 50  0001 C CNN
	1    4400 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4400 2600 4400 2650
$Comp
L power:GND #PWR?
U 1 1 5EE65AA8
P 4400 5200
AR Path="/5ED0998A/5EE65AA8" Ref="#PWR?"  Part="1" 
AR Path="/5EE65AA8" Ref="#PWR04"  Part="1" 
AR Path="/5EE4B953/5EE65AA8" Ref="#PWR?"  Part="1" 
F 0 "#PWR04" H 4400 4950 50  0001 C CNN
F 1 "GND" H 4405 5027 50  0000 C CNN
F 2 "" H 4400 5200 50  0001 C CNN
F 3 "" H 4400 5200 50  0001 C CNN
	1    4400 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4400 5100 4400 5150
$Comp
L Device:C C?
U 1 1 5EE67493
P 5450 6650
AR Path="/5ED0998A/5EE67493" Ref="C?"  Part="1" 
AR Path="/5EE67493" Ref="C3"  Part="1" 
AR Path="/5EE4B953/5EE67493" Ref="C?"  Part="1" 
F 0 "C3" H 5500 6550 50  0000 L CNN
F 1 "100n" H 5500 6750 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 5488 6500 50  0001 C CNN
F 3 "~" H 5450 6650 50  0001 C CNN
	1    5450 6650
	1    0    0    1   
$EndComp
Wire Wire Line
	5450 6450 5450 6500
Wire Wire Line
	5450 6800 5450 6900
Wire Wire Line
	5450 6450 5750 6450
Wire Wire Line
	5450 6900 5750 6900
$Comp
L Device:C C?
U 1 1 5EE6AB36
P 5750 6650
AR Path="/5ED0998A/5EE6AB36" Ref="C?"  Part="1" 
AR Path="/5EE6AB36" Ref="C4"  Part="1" 
AR Path="/5EE4B953/5EE6AB36" Ref="C?"  Part="1" 
F 0 "C4" H 5800 6550 50  0000 L CNN
F 1 "100n" H 5800 6750 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 5788 6500 50  0001 C CNN
F 3 "~" H 5750 6650 50  0001 C CNN
	1    5750 6650
	1    0    0    1   
$EndComp
Wire Wire Line
	5750 6450 5750 6500
Wire Wire Line
	5750 6800 5750 6900
Connection ~ 5450 6450
Connection ~ 5450 6900
Wire Wire Line
	5750 6450 6050 6450
Wire Wire Line
	5750 6900 6050 6900
$Comp
L Device:C C?
U 1 1 5EE70137
P 6050 6650
AR Path="/5ED0998A/5EE70137" Ref="C?"  Part="1" 
AR Path="/5EE70137" Ref="C5"  Part="1" 
AR Path="/5EE4B953/5EE70137" Ref="C?"  Part="1" 
F 0 "C5" H 6100 6550 50  0000 L CNN
F 1 "100n" H 6100 6750 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 6088 6500 50  0001 C CNN
F 3 "~" H 6050 6650 50  0001 C CNN
	1    6050 6650
	1    0    0    1   
$EndComp
Wire Wire Line
	6050 6450 6050 6500
Wire Wire Line
	6050 6800 6050 6900
Connection ~ 5750 6450
Connection ~ 5750 6900
Text Label 7350 2550 0    50   ~ 0
~MEM_WE
Wire Wire Line
	7350 2550 7850 2550
Text Label 1900 6500 2    50   ~ 0
A0
Wire Wire Line
	1900 6500 1400 6500
Text Label 3400 3100 0    50   ~ 0
A31
Wire Wire Line
	3400 3100 3900 3100
Text Label 3400 3000 0    50   ~ 0
A30
Wire Wire Line
	3400 3000 3900 3000
Wire Wire Line
	3400 3400 3900 3400
Wire Wire Line
	3400 3300 3900 3300
Text Label 3400 3200 0    50   ~ 0
A1
Text Label 3400 3300 0    50   ~ 0
A2
Text Label 3400 3400 0    50   ~ 0
A3
Wire Wire Line
	3400 3500 3900 3500
Wire Wire Line
	3400 3600 3900 3600
Text Label 3400 3500 0    50   ~ 0
A4
Text Label 3400 3600 0    50   ~ 0
A5
Text Label 3400 3700 0    50   ~ 0
A6
Wire Wire Line
	3400 3700 3900 3700
Wire Wire Line
	3400 3200 3900 3200
Text Label 7350 2250 0    50   ~ 0
D0
Text Label 7350 2150 0    50   ~ 0
D1
Text Label 7350 2050 0    50   ~ 0
D2
Text Label 7350 1950 0    50   ~ 0
D3
Text Label 7350 1850 0    50   ~ 0
D4
Text Label 7350 1750 0    50   ~ 0
D5
Text Label 7350 1650 0    50   ~ 0
D6
Text Label 7350 1550 0    50   ~ 0
D7
Wire Wire Line
	7850 2250 7350 2250
Wire Wire Line
	7350 2150 7850 2150
Wire Wire Line
	7850 2050 7350 2050
Wire Wire Line
	7350 1950 7850 1950
Wire Wire Line
	7850 1850 7350 1850
Wire Wire Line
	7350 1750 7850 1750
Wire Wire Line
	7850 1650 7350 1650
Wire Wire Line
	7350 1550 7850 1550
Text Label 5450 3000 2    50   ~ 0
~LCD_E
Wire Wire Line
	5450 3000 4900 3000
Text Label 7350 2650 0    50   ~ 0
LCD_E
Wire Wire Line
	3900 4800 3800 4800
Wire Wire Line
	3800 4800 3800 5300
Wire Wire Line
	3800 5300 3700 5300
Wire Wire Line
	3800 2650 4400 2650
Connection ~ 4400 2650
Wire Wire Line
	4400 2650 4400 2700
Wire Wire Line
	3700 5150 4400 5150
Connection ~ 4400 5150
Wire Wire Line
	4400 5150 4400 5200
$Comp
L riscv-serial:SolderJumper_3_Bridged12 JP1
U 1 1 5EEBE9DE
P 5600 4000
F 0 "JP1" V 5600 4067 50  0000 L CNN
F 1 "SolderJumper_3_Bridged12" V 5645 4067 50  0001 L CNN
F 2 "riscv-serial:SolderJumper-3_P1.3mm_Bridged2Bar12_RoundedPad1.0x1.5mm" H 5600 4000 50  0001 C CNN
F 3 "~" H 5600 4000 50  0001 C CNN
	1    5600 4000
	0    1    -1   0   
$EndComp
$Comp
L riscv-serial:SolderJumper_3_Bridged12 JP2
U 1 1 5EEBEC35
P 6200 4000
F 0 "JP2" V 6200 4067 50  0000 L CNN
F 1 "SolderJumper_3_Bridged12" V 6245 4067 50  0001 L CNN
F 2 "riscv-serial:SolderJumper-3_P1.3mm_Bridged2Bar12_RoundedPad1.0x1.5mm" H 6200 4000 50  0001 C CNN
F 3 "~" H 6200 4000 50  0001 C CNN
	1    6200 4000
	0    1    -1   0   
$EndComp
Wire Wire Line
	3900 4600 3400 4600
Text Label 3400 4600 0    50   ~ 0
B6
Text Label 3400 4500 0    50   ~ 0
B5
Text Label 3400 4400 0    50   ~ 0
B4
Text Label 3400 4300 0    50   ~ 0
B3
Text Label 3400 4200 0    50   ~ 0
B2
Text Label 3400 4100 0    50   ~ 0
B1
Wire Wire Line
	3400 4100 3900 4100
Wire Wire Line
	3900 4200 3400 4200
Wire Wire Line
	3400 4300 3900 4300
Wire Wire Line
	3900 4400 3400 4400
Wire Wire Line
	3400 4500 3900 4500
Text Label 8500 4000 0    50   ~ 0
B6
Wire Wire Line
	5300 4000 5450 4000
Text Label 5900 4000 0    50   ~ 0
B2
Wire Wire Line
	5900 4000 6050 4000
$Comp
L riscv-serial:SolderJumper_3_Bridged12 JP3
U 1 1 5EEEBEA2
P 6850 4000
F 0 "JP3" V 6850 4067 50  0000 L CNN
F 1 "SolderJumper_3_Bridged12" V 6895 4067 50  0001 L CNN
F 2 "riscv-serial:SolderJumper-3_P1.3mm_Bridged2Bar12_RoundedPad1.0x1.5mm" H 6850 4000 50  0001 C CNN
F 3 "~" H 6850 4000 50  0001 C CNN
	1    6850 4000
	0    1    -1   0   
$EndComp
Text Label 6550 4000 0    50   ~ 0
B3
Wire Wire Line
	6550 4000 6700 4000
$Comp
L riscv-serial:SolderJumper_3_Bridged12 JP4
U 1 1 5EEF045C
P 7500 4000
F 0 "JP4" V 7500 4067 50  0000 L CNN
F 1 "SolderJumper_3_Bridged12" V 7545 4067 50  0001 L CNN
F 2 "riscv-serial:SolderJumper-3_P1.3mm_Bridged2Bar12_RoundedPad1.0x1.5mm" H 7500 4000 50  0001 C CNN
F 3 "~" H 7500 4000 50  0001 C CNN
	1    7500 4000
	0    1    -1   0   
$EndComp
Text Label 7200 4000 0    50   ~ 0
B4
Wire Wire Line
	7200 4000 7350 4000
$Comp
L riscv-serial:SolderJumper_3_Bridged12 JP5
U 1 1 5EEF4B55
P 8150 4000
F 0 "JP5" V 8150 4067 50  0000 L CNN
F 1 "SolderJumper_3_Bridged12" V 8195 4067 50  0001 L CNN
F 2 "riscv-serial:SolderJumper-3_P1.3mm_Bridged2Bar12_RoundedPad1.0x1.5mm" H 8150 4000 50  0001 C CNN
F 3 "~" H 8150 4000 50  0001 C CNN
	1    8150 4000
	0    1    -1   0   
$EndComp
Text Label 7850 4000 0    50   ~ 0
B5
Wire Wire Line
	7850 4000 8000 4000
$Comp
L riscv-serial:SolderJumper_3_Bridged12 JP6
U 1 1 5EEF936D
P 8800 4000
F 0 "JP6" V 8800 4067 50  0000 L CNN
F 1 "SolderJumper_3_Bridged12" V 8845 4067 50  0001 L CNN
F 2 "riscv-serial:SolderJumper-3_P1.3mm_Bridged2Bar12_RoundedPad1.0x1.5mm" H 8800 4000 50  0001 C CNN
F 3 "~" H 8800 4000 50  0001 C CNN
	1    8800 4000
	0    1    -1   0   
$EndComp
Text Label 5300 4000 0    50   ~ 0
B1
Wire Wire Line
	8500 4000 8650 4000
Wire Wire Line
	5600 3800 5600 3700
Wire Wire Line
	5600 3700 6200 3700
Wire Wire Line
	8800 3700 8800 3800
Wire Wire Line
	8150 3800 8150 3700
Connection ~ 8150 3700
Wire Wire Line
	8150 3700 8800 3700
Wire Wire Line
	7500 3800 7500 3700
Connection ~ 7500 3700
Wire Wire Line
	7500 3700 8150 3700
Wire Wire Line
	6850 3800 6850 3700
Connection ~ 6850 3700
Wire Wire Line
	6850 3700 7500 3700
Wire Wire Line
	6200 3800 6200 3700
Connection ~ 6200 3700
Wire Wire Line
	6200 3700 6850 3700
Wire Wire Line
	5600 4200 5600 4350
Wire Wire Line
	5600 4350 6200 4350
Wire Wire Line
	8800 4350 8800 4200
Wire Wire Line
	8150 4200 8150 4350
Connection ~ 8150 4350
Wire Wire Line
	8150 4350 8800 4350
Wire Wire Line
	7500 4200 7500 4350
Connection ~ 7500 4350
Wire Wire Line
	7500 4350 8150 4350
Wire Wire Line
	6850 4200 6850 4350
Connection ~ 6850 4350
Wire Wire Line
	6850 4350 7500 4350
Wire Wire Line
	6200 4200 6200 4350
Connection ~ 6200 4350
Wire Wire Line
	6200 4350 6850 4350
$Comp
L power:VCC #PWR?
U 1 1 5EF3103C
P 5600 3600
AR Path="/5ED0998A/5EF3103C" Ref="#PWR?"  Part="1" 
AR Path="/5EF3103C" Ref="#PWR02"  Part="1" 
AR Path="/5EE4B953/5EF3103C" Ref="#PWR?"  Part="1" 
F 0 "#PWR02" H 5600 3450 50  0001 C CNN
F 1 "VCC" H 5617 3773 50  0000 C CNN
F 2 "" H 5600 3600 50  0001 C CNN
F 3 "" H 5600 3600 50  0001 C CNN
	1    5600 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 3600 5600 3700
Connection ~ 5600 3700
$Comp
L power:GND #PWR?
U 1 1 5EF36CD2
P 5600 4450
AR Path="/5ED0998A/5EF36CD2" Ref="#PWR?"  Part="1" 
AR Path="/5EF36CD2" Ref="#PWR03"  Part="1" 
AR Path="/5EE4B953/5EF36CD2" Ref="#PWR?"  Part="1" 
F 0 "#PWR03" H 5600 4200 50  0001 C CNN
F 1 "GND" H 5605 4277 50  0000 C CNN
F 2 "" H 5600 4450 50  0001 C CNN
F 3 "" H 5600 4450 50  0001 C CNN
	1    5600 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 4350 5600 4450
Connection ~ 5600 4350
$Comp
L power:VCC #PWR?
U 1 1 5EF800A8
P 8900 1300
AR Path="/5ED0998A/5EF800A8" Ref="#PWR?"  Part="1" 
AR Path="/5EF800A8" Ref="#PWR0101"  Part="1" 
AR Path="/5EE4B953/5EF800A8" Ref="#PWR?"  Part="1" 
F 0 "#PWR0101" H 8900 1150 50  0001 C CNN
F 1 "VCC" H 8917 1473 50  0000 C CNN
F 2 "" H 8900 1300 50  0001 C CNN
F 3 "" H 8900 1300 50  0001 C CNN
	1    8900 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 1300 8900 1350
$Comp
L power:VCC #PWR?
U 1 1 5EF85E08
P 9500 950
AR Path="/5ED0998A/5EF85E08" Ref="#PWR?"  Part="1" 
AR Path="/5EF85E08" Ref="#PWR0102"  Part="1" 
AR Path="/5EE4B953/5EF85E08" Ref="#PWR?"  Part="1" 
F 0 "#PWR0102" H 9500 800 50  0001 C CNN
F 1 "VCC" H 9517 1123 50  0000 C CNN
F 2 "" H 9500 950 50  0001 C CNN
F 3 "" H 9500 950 50  0001 C CNN
	1    9500 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 1300 9500 1350
$Comp
L power:GND #PWR?
U 1 1 5EF8BC83
P 8900 2900
AR Path="/5ED0998A/5EF8BC83" Ref="#PWR?"  Part="1" 
AR Path="/5EF8BC83" Ref="#PWR0103"  Part="1" 
AR Path="/5EE4B953/5EF8BC83" Ref="#PWR?"  Part="1" 
F 0 "#PWR0103" H 8900 2650 50  0001 C CNN
F 1 "GND" H 8905 2727 50  0000 C CNN
F 2 "" H 8900 2900 50  0001 C CNN
F 3 "" H 8900 2900 50  0001 C CNN
	1    8900 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 2850 8900 2900
$Comp
L power:GND #PWR?
U 1 1 5EF91B4D
P 9500 2900
AR Path="/5ED0998A/5EF91B4D" Ref="#PWR?"  Part="1" 
AR Path="/5EF91B4D" Ref="#PWR0104"  Part="1" 
AR Path="/5EE4B953/5EF91B4D" Ref="#PWR?"  Part="1" 
F 0 "#PWR0104" H 9500 2650 50  0001 C CNN
F 1 "GND" H 9505 2727 50  0000 C CNN
F 2 "" H 9500 2900 50  0001 C CNN
F 3 "" H 9500 2900 50  0001 C CNN
	1    9500 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 2850 9500 2900
$Comp
L Device:R_POT RV1
U 1 1 5EF97AB7
P 10800 2100
F 0 "RV1" H 10731 2146 50  0000 R CNN
F 1 "10k" H 10731 2055 50  0000 R CNN
F 2 "Potentiometer_THT:Potentiometer_Bourns_3386P_Vertical" H 10800 2100 50  0001 C CNN
F 3 "~" H 10800 2100 50  0001 C CNN
	1    10800 2100
	-1   0    0    -1  
$EndComp
Wire Wire Line
	10650 2100 10550 2100
$Comp
L power:VCC #PWR?
U 1 1 5EF9DB99
P 10800 1850
AR Path="/5ED0998A/5EF9DB99" Ref="#PWR?"  Part="1" 
AR Path="/5EF9DB99" Ref="#PWR0105"  Part="1" 
AR Path="/5EE4B953/5EF9DB99" Ref="#PWR?"  Part="1" 
F 0 "#PWR0105" H 10800 1700 50  0001 C CNN
F 1 "VCC" H 10817 2023 50  0000 C CNN
F 2 "" H 10800 1850 50  0001 C CNN
F 3 "" H 10800 1850 50  0001 C CNN
	1    10800 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	10800 1850 10800 1950
$Comp
L power:GND #PWR?
U 1 1 5EFA3C53
P 10800 2350
AR Path="/5ED0998A/5EFA3C53" Ref="#PWR?"  Part="1" 
AR Path="/5EFA3C53" Ref="#PWR0106"  Part="1" 
AR Path="/5EE4B953/5EFA3C53" Ref="#PWR?"  Part="1" 
F 0 "#PWR0106" H 10800 2100 50  0001 C CNN
F 1 "GND" H 10805 2177 50  0000 C CNN
F 2 "" H 10800 2350 50  0001 C CNN
F 3 "" H 10800 2350 50  0001 C CNN
	1    10800 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	10800 2250 10800 2350
Text Label 3800 5100 1    50   ~ 0
~MEM_E
Wire Wire Line
	3700 5150 3700 4000
Wire Wire Line
	3700 4000 3900 4000
Wire Wire Line
	3900 3900 3800 3900
Wire Wire Line
	3800 3900 3800 2650
$Comp
L riscv-serial:74LVC1G04 U4
U 1 1 5EFED4D5
P 6750 2650
F 0 "U4" H 6750 2917 50  0000 C CNN
F 1 "74LVC1G04" H 6750 2826 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5_HandSoldering" H 6750 2650 50  0001 C CNN
F 3 "" H 6750 2650 50  0001 C CNN
	1    6750 2650
	1    0    0    -1  
$EndComp
Text Label 6050 2650 0    50   ~ 0
~LCD_E
Wire Wire Line
	6050 2650 6450 2650
Wire Wire Line
	7050 2650 7850 2650
$Comp
L riscv-serial:74LVC1G04 U4
U 2 1 5EFF9D65
P 8900 5700
F 0 "U4" H 9030 5746 50  0000 L CNN
F 1 "74LVC1G04" H 9030 5655 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5_HandSoldering" H 8900 5700 50  0001 C CNN
F 3 "" H 8900 5700 50  0001 C CNN
	2    8900 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	8150 5200 8150 5100
Wire Wire Line
	8150 5100 8900 5100
Wire Wire Line
	8900 5100 8900 5200
Wire Wire Line
	8150 6200 8150 6300
Wire Wire Line
	8150 6300 8900 6300
Wire Wire Line
	8900 6300 8900 6200
$Comp
L power:VCC #PWR?
U 1 1 5F0196B5
P 8150 5000
AR Path="/5ED0998A/5F0196B5" Ref="#PWR?"  Part="1" 
AR Path="/5F0196B5" Ref="#PWR0107"  Part="1" 
AR Path="/5EE4B953/5F0196B5" Ref="#PWR?"  Part="1" 
F 0 "#PWR0107" H 8150 4850 50  0001 C CNN
F 1 "VCC" H 8167 5173 50  0000 C CNN
F 2 "" H 8150 5000 50  0001 C CNN
F 3 "" H 8150 5000 50  0001 C CNN
	1    8150 5000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5F0196F2
P 8150 6400
AR Path="/5ED0998A/5F0196F2" Ref="#PWR?"  Part="1" 
AR Path="/5F0196F2" Ref="#PWR0108"  Part="1" 
AR Path="/5EE4B953/5F0196F2" Ref="#PWR?"  Part="1" 
F 0 "#PWR0108" H 8150 6150 50  0001 C CNN
F 1 "GND" H 8155 6227 50  0000 C CNN
F 2 "" H 8150 6400 50  0001 C CNN
F 3 "" H 8150 6400 50  0001 C CNN
	1    8150 6400
	1    0    0    -1  
$EndComp
Wire Wire Line
	8150 6300 8150 6400
Connection ~ 8150 6300
Connection ~ 8150 5100
Wire Wire Line
	8150 5000 8150 5100
$Comp
L Device:R R?
U 1 1 5F02D600
P 9500 1150
F 0 "R?" H 9570 1196 50  0000 L CNN
F 1 "R" H 9570 1105 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 9430 1150 50  0001 C CNN
F 3 "~" H 9500 1150 50  0001 C CNN
	1    9500 1150
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 950  9500 1000
$EndSCHEMATC
