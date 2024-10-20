/*
   dmm_decode_only.c

   (c) 2024 linux-works
*/


#include <Arduino.h>
#include <string.h>

#include "dmm_decode.h"


unsigned int function, scale, decimal;
int measurement;
uint16_t reading[3];  // 3 groups of 16bit words (6 bytes total)
char tmp_val_buf[16];
char fixed_val_buf[16];

struct glb {
  uint16_t flags;
  char *owon_multi_cli_address;
};



void decode_init (struct glb *g)
{
  g->flags = 0;
  g->owon_multi_cli_address = NULL;
}



//
// function_code, values and flags to string
//

char *funct_to_string (char *fixed_val_buf, int function, int scale, int measurement)
{
  static char full_value_s[64];
  char funct_s[32];

  memset(full_value_s, 0, 64);
  memset(funct_s, 0, 32);


  if (function == MODE_DC_VOLTS) {
    if (scale == 3) {
      strcpy(funct_s, "DC mV");

    } else if (scale == 4) {
      strcpy(funct_s, "DC V");

    } else {
      strcpy(funct_s, "??DC V");
    }

  } else if (function == MODE_AC_VOLTS) {
    if (scale == 4) {
      strcpy(funct_s, "AC V");
    } else if (scale == 3) {
      strcpy(funct_s, "AC mV");
    } else {
      strcpy(funct_s, "??AC V");
    }

  } else if (function == MODE_AC_AMPS) {
    if (scale == 2) {
      strcpy(funct_s, "AC µA");
    } else if (scale == 3) {
      strcpy(funct_s, "AC mA");
    } else if (scale == 4) {
      strcpy(funct_s, "AC A");
    } else {
      strcpy(funct_s, "??AC A");
    }

  } else if (function == MODE_DC_AMPS ) {
    if (scale == 2) {
      strcpy(funct_s, "DC µA");
    } else if (scale == 3) {
      strcpy(funct_s, "DC mA");
    } else if (scale == 4) {
      strcpy(funct_s, "DC A");
    } else {
      strcpy(funct_s, "??DC A");
    }
  } else if (function == MODE_NCV) {
    strcpy(funct_s, "NCV");

  } else if (function == MODE_OHMS) {
    if (measurement == INF_OPEN) {
      strcpy(funct_s, "Ohms Open");

    } else {
      if (scale == 4) {
        strcpy(funct_s, "Ohms");

      } else if (scale == 5) {
        strcpy(funct_s, "K Ohms");

      } else if (scale == 6) {
        strcpy(funct_s, "M Ohms");

      } else {
        strcpy(funct_s, "??Ohms");
      }
    }

  } else if (function == MODE_CAPACITANCE) {
    if (scale == 1) {
      strcpy(funct_s, "nF");

    } else if (scale == 2) {
      strcpy(funct_s, "uF");

    } else {
      strcpy(funct_s, "??Farads");
    }

  } else if (function == MODE_DIODE) {
    if (measurement == INF_OPEN) {
      strcpy(funct_s, "Diode Open");

    } else {
      strcpy(funct_s, "Diode");
    }

  } else if (function == MODE_CONTINUITY) {
    if (measurement == INF_OPEN) {
      strcpy(funct_s, "Continuity Open");

    } else {
      strcpy(funct_s, "Continuity Closed");
    }

  } else if (function == MODE_FREQUENCY) {
    strcpy(funct_s, "Hz"); // frequency mode

  } else if (function == MODE_PERCENT) {
    strcpy(funct_s, "%");  // percent mode

  } else if (function == MODE_TEMP_C) {
    strcpy(funct_s, "Deg C");

  } else if (function == MODE_TEMP_F) {
    strcpy(funct_s, "Deg F");

  } else if (function == MODE_HFE) {
    strcpy(funct_s, "HFE");

  } else {
    strcpy(funct_s, "???");
  }


  // form the full string
  strcpy(full_value_s, fixed_val_buf);
  strcat(full_value_s, " ");
  strcat(full_value_s, funct_s);

  return (full_value_s);
}





//
// Owon meter logic
//

char *parse_ble_meter_bytes (uint8_t d[])
{
  static char buf[256];


  // convert bytes to 'reading' array
  reading[0] = d[1] << 8 | d[0];
  reading[1] = d[3] << 8 | d[2];
  reading[2] = d[5] << 8 | d[4];

  // Extract data items from first field
  function = (reading[0] >> 6) & 0x0f;
  scale = (reading[0] >> 3) & 0x07;
  decimal = reading[0] & 0x07;

  // Extract and convert measurement value (sign)
  if (reading[2] < 0x7fff) {
    measurement = reading[2];
  } else {
    measurement = -1 * (reading[2] & 0x7fff);
  }


  // debug
  //sprintf(buf, "\"Function\": \"%c%c%c%c%c%c%c%c\", \"Scale\": \"%02d\", \"Decimal\": \"%02d\", ", BYTE_TO_BINARY(function), scale, decimal);
  //Serial.println(buf);

  //sprintf(buf, "\"Measurement\": \"%d\", ", measurement);
  //Serial.println(buf);


  // convert a string of numerals to 'floating point' via dot insertion
  //  copy bytes over from src to dest until we get to where the 'dot' should go

  // string (integer) version of meter value
  sprintf(tmp_val_buf, "%04d", measurement);

  int tv = 0; // temp value
  int fv = 0; // fixed value
  memset(fixed_val_buf, 0, 16);

  for (tv = 0; tv < strlen(tmp_val_buf); tv++) {
    if ( tv == (strlen(tmp_val_buf) - decimal) ) {
      fixed_val_buf[fv++] = '.';
    }
    fixed_val_buf[fv++] = tmp_val_buf[tv];
  }


  // get the non-number part of the measurement
  char *funct_s = funct_to_string(fixed_val_buf, function, scale, measurement);

  // finally, print the value and any flags (if not in quiet-mode)
  //if (!strcmp(fixed_val_buf, INF_OPEN_S)) {
  sprintf(buf, "%s", funct_s);  // no value, just a 'flag'
  //Serial.println(buf);
  //}

  return buf;
}




#ifdef TEST_MAIN
int main (int argc, char **argv)
{
  uint8_t i = 0;

  // default meter type
  //strcpy(owon_handle, OWON_B35_B41_HANDLE);
  //owon_length = OWON_B35_B41_LENGTH;
  //strcpy(owon_handle, OWON_CM2100B_HANDLE);
  owon_length = OWON_CM2100B_LENGTH;



  // ble test data: [22, f1, 04, 00, 9f, 11] = "45.11 Ohms"
  d[0] = 0x22;
  d[1] = 0xf1;
  d[2] = 0x04;
  d[3] = 0x00;
  d[4] = 0x9f;
  d[5] = 0x11;

  // debug
  Serial.print("{ \"BLE_bytes\" : \"[");
  for (i = 0; i < owon_length; i++) {
    Serial.print("%02x", d[i]);
  }
  Serial.print("]\", ");



  (void)parse_ble_meter_bytes(d);
}

#endif // TEST_MAIN
