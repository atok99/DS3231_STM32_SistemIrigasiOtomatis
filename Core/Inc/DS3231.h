#ifndef DS3231
#define DS3231

#include "stm32f4xx.h"                  // Device header
#include "stdint.h"

// I2C functions
void i2c_init(void);
static void i2c_start(void);
static void i2c_stop(void);
static void i2c_addr_r(uint8_t addr);
static void i2c_addr_w(uint8_t addr);
static void i2c_writebyte(uint8_t byte);

// Enable or disable multi-byte sending
typedef enum
{
	MULTI_BYTE_OFF = 0,
	MULTI_BYTE_ON	
}ack_send;

static uint8_t i2c_readbyte(ack_send ack);

// Convert the bcd value to decimal
static int bcd2dec(uint8_t b);

// Convert the decimal value to bcd
static int decimal2bcd (uint8_t d);

uint8_t ds3231_seconds(void);
uint8_t ds3231_minutes(void);
uint8_t ds3231_hours(void);

void ds3231_set_time(uint8_t hh, uint8_t mm, uint8_t ss);
void ds3231_set_date(uint8_t dd, uint8_t mm, uint8_t yy);

typedef struct
{
  uint8_t seconds;  
  uint8_t minutes;  
  uint8_t hours;

	uint8_t date;
	uint8_t month;
	uint8_t year;
}ds3231_data_struct;

void ds3231_get_time(ds3231_data_struct *ds3231_struct);
void ds3231_get_date(ds3231_data_struct *ds3231_struct);



#endif