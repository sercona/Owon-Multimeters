/*
   dmm_decode.h

   (c) 2024 linux-works
*/


#ifndef _DECODE_H_
#define _DECODE_H_

#include <Arduino.h>
#include <string.h>


#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)      \
  ((byte) & 0x80 ? '1' : '0'),      \
  ((byte) & 0x40 ? '1' : '0'),    \
  ((byte) & 0x20 ? '1' : '0'),    \
  ((byte) & 0x10 ? '1' : '0'),    \
  ((byte) & 0x08 ? '1' : '0'),    \
  ((byte) & 0x04 ? '1' : '0'),    \
  ((byte) & 0x02 ? '1' : '0'),    \
  ((byte) & 0x01 ? '1' : '0')


// tokens that mean non-numeric things
#define INF_OPEN   (-32767)  // numeric
#define INF_OPEN_S "-32767"  // string


// mode bit-masks
#define MODE_DC_VOLTS     0b00000000
#define MODE_AC_VOLTS     0b00000001
#define MODE_AC_AMPS      0b00000011
#define MODE_DC_AMPS      0b00000010
#define MODE_NCV          0b00001101
#define MODE_OHMS         0b00000100
#define MODE_CAPACITANCE  0b00000101
#define MODE_DIODE        0b00001010
#define MODE_CONTINUITY   0b00001011
#define MODE_FREQUENCY    0b00000110
#define MODE_PERCENT      0b00000111
#define MODE_TEMP_C       0b00001000
#define MODE_TEMP_F       0b00001001
#define MODE_HFE          0b00001100

// so far, 2 handle (hex) values were seen by rev-eng, so we give them names, here
//#define OWON_CM2100B_HANDLE  "0x001b"
#define OWON_CM2100B_LENGTH  6 //18

//#define OWON_OW18E_HANDLE    "0x001b"
#define OWON_OW18E_LENGTH    6 //18

//#define OWON_B35_B41_HANDLE  "0x002e"
#define OWON_B35_B41_LENGTH  6 //18


// externs
extern char *parse_ble_meter_bytes(uint8_t d[]);
extern int owon_length; // = OWON_CM2100B_LENGTH;
extern uint8_t d[14];   // 'd' array is the sequence of bytes


#endif // _DECODE_H_
