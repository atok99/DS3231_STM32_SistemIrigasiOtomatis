#include "stm32f4xx.h"                  // Header device STM32F4
#include "DS3231.h"                     // Header untuk RTC DS3231

#define SLAVE_ADDR 		0x68           // Alamat I2C dari modul DS3231

void i2c_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	                    // Aktifkan clock untuk GPIOB
	GPIOB->MODER |= GPIO_MODER_MODE7_1 | GPIO_MODER_MODE6_1;	// Set pin PB6 (SCL) dan PB7 (SDA) ke mode Alternate Function
	GPIOB->AFR[0] |= GPIO_AFRL_AFRL7_2 | GPIO_AFRL_AFRL6_2;	// Konfigurasi Alternate Function AF4 untuk I2C1
	GPIOB->OTYPER |= GPIO_OTYPER_OT7 | GPIO_OTYPER_OT6;		// Set pin SCL dan SDA sebagai open-drain
	
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;	// Aktifkan clock untuk peripheral I2C1

	I2C1->CR1 |= I2C_CR1_SWRST;		    // Reset software I2C1
	I2C1->CR1 &= ~I2C_CR1_SWRST;		// Selesai reset

	I2C1->CR2 |= I2C_CR2_FREQ_4;		// Set frekuensi clock peripheral (FREQ = 16MHz)
	I2C1->CCR |= 80;		            // Set nilai clock control register (lihat NOTE(1))
	I2C1->TRISE = 17;		            // Set TRISE = 1000ns/(1/16MHz) + 1 (lihat NOTE(2))
	I2C1->CR1 |= I2C_CR1_PE;		    // Enable peripheral I2C1
}

static void i2c_writebyte(uint8_t byte)
{
	while (!(I2C1->SR1 & I2C_SR1_TXE)){}	// Tunggu hingga register transmisi kosong
	I2C1->DR = byte;			            // Tulis data ke data register
	while (!(I2C1->SR1 & I2C_SR1_TXE));	    // Tunggu hingga register transmisi kembali kosong
}

static uint8_t i2c_readbyte(ack_send ack)
{
	if (ack) I2C1->CR1 |= I2C_CR1_ACK;	    // Jika baca banyak byte, set bit ACK
	else I2C1->CR1 &= ~I2C_CR1_ACK;	        // Jika baca terakhir, clear bit ACK
	
	while (!(I2C1->SR1 & I2C_SR1_RXNE)){};	// Tunggu data tersedia di register RX
	
	return (I2C1->DR);		                // Return data yang diterima
}

void i2c_addr_r(uint8_t addr)
{
	volatile char res;
	
	I2C1->DR = addr << 1 | 1; 	            // Kirim alamat slave dengan bit R (read)
	while (!(I2C1->SR1 & I2C_SR1_ADDR)){}; 	// Tunggu flag ADDR set (lihat NOTE(3))

	I2C1->CR1 &= ~0x400;		            // Clear bit ACK
	res = I2C1->SR2;		                // Baca SR2 untuk clear flag ADDR
}

void i2c_addr_w(uint8_t addr)
{
	volatile char res;
	
	I2C1->DR = addr << 1;		            // Kirim alamat slave dengan bit W (write)
	while (!(I2C1->SR1 & I2C_SR1_ADDR));	// Tunggu flag ADDR set (lihat NOTE(3))
	
	res = I2C1->SR2;		                // Baca SR2 untuk clear flag ADDR
}

void i2c_stop(void)
{
	I2C1->CR1 |= I2C_CR1_STOP;		        // Kirim kondisi STOP
	while (I2C1->SR2 & I2C_SR2_BUSY){}	    // Tunggu hingga bus tidak sibuk
}

void i2c_start(void)
{
	I2C1->CR1 |= I2C_CR1_START;			    // Kirim kondisi START
	while (!(I2C1->SR1 & I2C_SR1_SB)){};	// Tunggu flag START terkirim (lihat NOTE(3))
}

// Konversi dari format BCD ke desimal
static int bcd2dec(uint8_t b) { return ((b/16)*10 + (b%16)); }
// Konversi dari format desimal ke BCD
static int decimal2bcd (uint8_t d) { return (((d/10) << 4) | (d % 10)); }

uint8_t ds3231_seconds(void)
{
	i2c_start();				            // Mulai komunikasi I2C
	i2c_addr_w(SLAVE_ADDR);	            // Kirim alamat slave dengan mode tulis
	i2c_writebyte(0);		            // Tulis register pointer ke 0 (detik)
	i2c_start();			            // Kirim start ulang (repeated start)
	i2c_addr_r(SLAVE_ADDR);	            // Kirim alamat slave dengan mode baca
	i2c_stop();			                // Kirim stop setelah pembacaan selesai
	return bcd2dec(i2c_readbyte(MULTI_BYTE_OFF)); // Baca data dan konversi ke desimal
}

uint8_t ds3231_minutes(void)
{
	i2c_start();
	i2c_addr_w(SLAVE_ADDR);
	i2c_writebyte(1);		            // Register menit
	i2c_start();
	i2c_addr_r(SLAVE_ADDR);
	i2c_stop();
	return bcd2dec(i2c_readbyte(MULTI_BYTE_OFF));
}

uint8_t ds3231_hours(void)
{
	i2c_start();
	i2c_addr_w(SLAVE_ADDR);
	i2c_writebyte(2);		            // Register jam
	i2c_start();
	i2c_addr_r(SLAVE_ADDR);
	i2c_stop();
	return bcd2dec(i2c_readbyte(MULTI_BYTE_OFF));
}

void ds3231_get_time(ds3231_data_struct *ds3231_struct)
{
	i2c_start();
	i2c_addr_w(SLAVE_ADDR);
	i2c_writebyte(0);	                // Set pointer ke register waktu awal
	i2c_start();
	i2c_addr_r(SLAVE_ADDR);
	ds3231_struct->seconds = bcd2dec(i2c_readbyte(MULTI_BYTE_ON)); // Baca detik
	ds3231_struct->minutes = bcd2dec(i2c_readbyte(MULTI_BYTE_ON)); // Baca menit
	ds3231_struct->hours = bcd2dec(i2c_readbyte(MULTI_BYTE_OFF));  // Baca jam
	i2c_stop();	
}

void ds3231_get_date(ds3231_data_struct *ds3231_struct)
{
	i2c_start();
	i2c_addr_w(SLAVE_ADDR);
	i2c_writebyte(4);	                // Set pointer ke register tanggal
	i2c_start();
	i2c_addr_r(SLAVE_ADDR);
	ds3231_struct->date = bcd2dec(i2c_readbyte(MULTI_BYTE_ON));  // Baca tanggal
	ds3231_struct->month = bcd2dec(i2c_readbyte(MULTI_BYTE_ON)); // Baca bulan
	ds3231_struct->year = bcd2dec(i2c_readbyte(MULTI_BYTE_OFF)); // Baca tahun
	i2c_stop();		
}

void ds3231_set_time(uint8_t hh, uint8_t mm, uint8_t ss)
{
	i2c_start();
	i2c_addr_w(SLAVE_ADDR);
	i2c_writebyte(0);		            // Set pointer ke register detik

	while (!(I2C1->SR1 & I2C_SR1_TXE)){};		
	I2C1->DR = decimal2bcd(ss);		    // Tulis detik dalam format BCD

	while (!(I2C1->SR1 & I2C_SR1_TXE)){};		
	I2C1->DR = decimal2bcd(mm);		    // Tulis menit dalam format BCD

	while (!(I2C1->SR1 & I2C_SR1_TXE)){};	
	I2C1->DR = decimal2bcd(hh);		    // Tulis jam dalam format BCD

	while (!(I2C1->SR1 & I2C_SR1_TXE)){};	
	i2c_stop();			                // Kirim STOP
}

void ds3231_set_date(uint8_t dd, uint8_t mm, uint8_t yy)
{
	i2c_start();
	i2c_addr_w(SLAVE_ADDR);
	i2c_writebyte(4);		            // Set pointer ke register tanggal

	while (!(I2C1->SR1 & I2C_SR1_TXE)){};		
	I2C1->DR = decimal2bcd(dd);		    // Tulis tanggal

	while (!(I2C1->SR1 & I2C_SR1_TXE)){};		
	I2C1->DR = decimal2bcd(mm);		    // Tulis bulan

	while (!(I2C1->SR1 & I2C_SR1_TXE)){};	
	I2C1->DR = decimal2bcd(yy);		    // Tulis tahun

	while (!(I2C1->SR1 & I2C_SR1_TXE)){};	
	i2c_stop();			                // Kirim STOP
}
