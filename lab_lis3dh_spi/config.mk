# Copyright 2020 Charles Daniels, Jason Bakos, Philip Conrad

# CPU clock source speed
# F_CPU = 1000000UL  # un-comment for 1MHz
F_CPU = 16000000UL  # un-comment for 16MHz

# gcc MCU name, "attyiny84" for attiny84
MCUNAME = atmega328p

# avrdude part number, "t84" for attiny84
PARTNO = m328p

# You should almost always use -Os when targeting AVR, otherwise the binary
# will probably be too large to fit.
OPTIMIZATION = -Os

# see avrdude -c ?
# AVRDUDE_PROGRAMMER = usbtiny # uncomment for USBTinyISP or compatible
AVRDUDE_PROGRAMMER = atmelice_isp

# target baudrate for serial
# UART_BAUDRATE = 9600
UART_BAUDRATE = 57600

