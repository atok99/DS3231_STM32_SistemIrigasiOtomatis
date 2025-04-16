/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"        // Memasukkan header file utama yang berisi definisi dan deklarasi untuk konfigurasi dan pengaturan hardware
#include "i2c.h"         // Memasukkan header file untuk komunikasi I2C (untuk komunikasi dengan DS3231)
#include "usart.h"       // Memasukkan header file untuk komunikasi USART (untuk komunikasi serial)
#include "gpio.h"        // Memasukkan header file untuk pengaturan GPIO (misalnya, untuk menghidupkan/mematikan pompa)

/* Private includes ----------------------------------------------------------*/
#include "DS3231.h"      // Memasukkan header file untuk pengaturan RTC DS3231
#include <stdio.h>       // Memasukkan header untuk fungsi input-output standar (seperti printf)
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
RTC_Time currentTime; // Mendeklarasikan variabel untuk menyimpan waktu saat ini dari RTC (jam, menit, detik)
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define PUMP_GPIO_PORT GPIOA  // Mendefinisikan port GPIO tempat pompa terhubung (GPIOA)
#define PUMP_GPIO_PIN GPIO_PIN_0 // Mendefinisikan pin GPIO yang digunakan untuk mengendalikan pompa (pin 0)
/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
static uint8_t pumpState = 0;  // Variabel untuk menyimpan status pompa (0: OFF, 1: ON)
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);  // Deklarasi fungsi untuk mengonfigurasi clock sistem
void Error_Handler(void);       // Deklarasi fungsi untuk menangani error
void CheckAndControlPump(void); // Deklarasi fungsi untuk memeriksa waktu dan mengendalikan pompa
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void CheckAndControlPump(void)
{
  // Memeriksa apakah jam saat ini berada dalam rentang waktu yang diinginkan untuk menyalakan pompa
  if ((currentTime.hours == 6 && currentTime.minutes < 30) ||
      (currentTime.hours == 16 && currentTime.minutes < 30))
  {
    // Jika pompa belum menyala, nyalakan pompa
    if (pumpState == 0)
    {
      HAL_GPIO_WritePin(PUMP_GPIO_PORT, PUMP_GPIO_PIN, GPIO_PIN_SET); // Menghidupkan pompa dengan menulis HIGH pada pin GPIO
      pumpState = 1; // Mengubah status pompa menjadi ON
      printf("Pompa ON: %02d:%02d:%02d\r\n", currentTime.hours, currentTime.minutes, currentTime.seconds); // Menampilkan waktu pompa hidup
    }
  }
  else
  {
    // Jika waktu di luar rentang waktu yang diinginkan, matikan pompa
    if (pumpState == 1)
    {
      HAL_GPIO_WritePin(PUMP_GPIO_PORT, PUMP_GPIO_PIN, GPIO_PIN_RESET); // Mematikan pompa dengan menulis LOW pada pin GPIO
      pumpState = 0; // Mengubah status pompa menjadi OFF
      printf("Pompa OFF: %02d:%02d:%02d\r\n", currentTime.hours, currentTime.minutes, currentTime.seconds); // Menampilkan waktu pompa mati
    }
  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();  // Menginisialisasi hardware abstraksi layer (HAL), yang mengatur periferal dasar dan sistem interrupt

  /* Configure the system clock */
  SystemClock_Config();  // Memanggil fungsi untuk mengonfigurasi sistem clock

  /* Initialize all configured peripherals */
  MX_GPIO_Init();  // Menginisialisasi konfigurasi GPIO (misalnya, untuk pompa)
  MX_I2C1_Init();  // Menginisialisasi konfigurasi I2C (untuk komunikasi dengan RTC DS3231)
  MX_USART1_UART_Init();  // Menginisialisasi konfigurasi USART (untuk komunikasi serial)

  /* Inisialisasi I2C dan RTC DS3231 */
  i2c_init();  // Menginisialisasi I2C untuk komunikasi dengan DS3231

  // Inisialisasi awal GPIO pompa (Awalnya mati)
  HAL_GPIO_WritePin(PUMP_GPIO_PORT, PUMP_GPIO_PIN, GPIO_PIN_RESET);  // Menjaga pompa mati saat inisialisasi

  /* Infinite loop */
  while (1)
  {
    // Membaca waktu saat ini dari RTC DS3231
    ds3231_data_struct rtc;
    ds3231_get_time(&rtc);  // Mendapatkan waktu dari RTC DS3231 dan menyimpannya dalam struktur rtc

    // Mengupdate waktu saat ini
    currentTime.hours = rtc.hours;
    currentTime.minutes = rtc.minutes;
    currentTime.seconds = rtc.seconds;

    // Cek dan kontrol pompa berdasarkan waktu yang diterima dari RTC
    CheckAndControlPump();  // Memanggil fungsi untuk memeriksa waktu dan mengendalikan pompa

    // Delay selama 1 detik sebelum memeriksa waktu lagi
    HAL_Delay(1000);  // Memberikan jeda 1 detik agar kontrol pompa tidak dilakukan terlalu cepat
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @brief  This function configures the system clock.
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};  // Struktur untuk mengatur konfigurasi oscillator
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};  // Struktur untuk mengatur konfigurasi clock sistem

    /* Inisialisasi OSC HSI (High-Speed Internal Oscillator) */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;  // Menggunakan HSI sebagai sumber oscillator
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;  // Menyalakan HSI
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;  // Menggunakan nilai kalibrasi default untuk HSI
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;  // Tidak menggunakan PLL (Phase-Locked Loop)
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)  // Mencoba mengonfigurasi oscillator
    {
        Error_Handler();  // Jika gagal, panggil fungsi Error_Handler untuk menangani kesalahan
    }

    /* Pilih sumber clock sistem */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;  // Menentukan bahwa kita akan mengonfigurasi clock sistem
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;  // Menggunakan HSI sebagai sumber clock sistem
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)  // Mencoba mengonfigurasi clock sistem
    {
        Error_Handler();  // Jika gagal, panggil fungsi Error_Handler untuk menangani kesalahan
    }
}

/**
  * @brief  This function handles errors and enters an infinite loop.
  * @retval None
  */
void Error_Handler(void)
{
    __disable_irq();  // Menonaktifkan interrupt untuk mencegah eksekusi kode lebih lanjut
    while (1)  // Masuk ke dalam loop tak terhingga untuk menahan program di sini ketika terjadi kesalahan
    {
        // Loop ini tidak akan pernah keluar kecuali sistem di-reset
    }
}
