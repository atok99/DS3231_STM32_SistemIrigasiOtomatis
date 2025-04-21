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
#include "main.h"       // Header utama proyek, menyertakan definisi dan prototipe dasar
                        // SEI CERT C: DCL31-C — Identifier harus dideklarasikan sebelum digunakan
#include "cmsis_os.h"   // Header RTOS CMSIS
#include "i2c-lcd.h"    // Header untuk konfigurasi dan fungsi-fungsi kontrol LCD berbasis I2C

/* Private includes ----------------------------------------------------------*/
#include "DS3231.h"     // Header untuk pengendalian modul RTC DS3231
#include <stdio.h>      // Untuk fungsi printf() dan output standar
                        // SEI CERT C: FIO30-C — Hindari format string dari input yang tidak tepercaya
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
ds3231_data_struct currentTime;  // Variable untuk menyimpan waktu RTC
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;          // Handle ADC1 untuk pembacaan analog
DMA_HandleTypeDef hdma_adc1;      // Handle DMA untuk ADC1

I2C_HandleTypeDef hi2c1;          // Handle komunikasi I2C

UART_HandleTypeDef huart2;        // Handle komunikasi UART2

osThreadId Tugas1Handle;          // Handle untuk Thread 1
osThreadId Tugas2Handle;          // Handle untuk Thread 2
osThreadId Tugas3Handle;          // Handle untuk Thread 3
osThreadId Tugas4Handle;          // Handle untuk Thread 4
/* USER CODE BEGIN PV */
volatile uint8_t pompaStatus = 0;      // Status Pompa (0: OFF, 1: ON)
volatile uint8_t countdown = 0;        // Timer countdown untuk pompa aktif
                                    //(CON34-C: Volatile untuk shared variable)
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
                                                         // (DCL31-C: Deklarasi sebelum digunakan)
void SystemClock_Config(void);                       // Fungsi konfigurasi clock sistem
static void MX_GPIO_Init(void);                      // Inisialisasi pin GPIO
static void MX_DMA_Init(void);                       // Inisialisasi DMA
static void MX_USART2_UART_Init(void);               // Inisialisasi USART2
static void MX_ADC1_Init(void);                      // Inisialisasi ADC1
static void MX_I2C1_Init(void);                      // Inisialisasi I2C1
void Pertama(void const * argument);                 // Thread 1: baca RTC
void Kedua(void const * argument);                   // Thread 2: kontrol pompa otomatis
void Ketiga(void const * argument);                  // Thread 3: update LCD
void Keempat(void const * argument);                 // Thread 4: countdown pompa

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

	  /* USER CODE BEGIN 1 */
	  /* USER CODE END 1 */

	  /* MCU Configuration--------------------------------------------------------*/

	  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	  HAL_Init(); // SEI CERT C: MEM34-C — Hanya membebaskan memori yang dialokasikan secara dinamis. Ini adalah inisialisasi untuk semua perangkat keras, termasuk pembebasan memori jika diperlukan.

	  /* USER CODE BEGIN Init */
	  /* USER CODE END Init */

	  /* Configure the system clock */
	  SystemClock_Config(); // SEI CERT C: EXP35-C — Jangan mengubah objek dengan umur sementara. Fungsi ini mengonfigurasi sistem clock, yang penting untuk kestabilan perangkat keras.

	  /* USER CODE BEGIN SysInit */
	  /* USER CODE END SysInit */

	  /* Initialize all configured peripherals */
	  MX_GPIO_Init(); // SEI CERT C: DCL31-C — Identifier harus dideklarasikan sebelum digunakan. Menginisialisasi GPIO (General Purpose Input Output) yang dibutuhkan.
	  MX_DMA_Init();  // SEI CERT C: DCL31-C — Identifier harus dideklarasikan sebelum digunakan. Menginisialisasi DMA (Direct Memory Access) untuk efisiensi transfer data.
	  MX_USART2_UART_Init(); // SEI CERT C: DCL31-C — Identifier harus dideklarasikan sebelum digunakan. Inisialisasi UART untuk komunikasi serial.
	  MX_ADC1_Init(); // SEI CERT C: DCL31-C — Identifier harus dideklarasikan sebelum digunakan. Inisialisasi ADC (Analog-to-Digital Converter) untuk pembacaan data analog.
	  MX_I2C1_Init(); // SEI CERT C: DCL31-C — Identifier harus dideklarasikan sebelum digunakan. Menginisialisasi I2C (Inter-Integrated Circuit) untuk komunikasi data.

	  /* USER CODE BEGIN 2 */
	  /* USER CODE END 2 */

	  /* USER CODE BEGIN RTOS_MUTEX */
	  /* add mutexes, ... */  // SEI CERT C: CON34-C — Gunakan mutex yang tepat untuk melindungi data bersama di lingkungan multitasking.
	  /* USER CODE END RTOS_MUTEX */

	  /* USER CODE BEGIN RTOS_SEMAPHORES */
	  /* add semaphores, ... */  // SEI CERT C: CON34-C — Gunakan semaphores untuk sinkronisasi thread di sistem operasi waktu nyata.
	  /* USER CODE END RTOS_SEMAPHORES */

	  /* USER CODE BEGIN RTOS_TIMERS */
	  /* start timers, add new ones, ... */  // SEI CERT C: CON34-C — Menambahkan timer dengan penjadwalan yang sesuai dalam sistem multitasking.
	  /* USER CODE END RTOS_TIMERS */

	  /* USER CODE BEGIN RTOS_QUEUES */
	  /* add queues, ... */ // SEI CERT C: CON34-C — Menambahkan antrian untuk pengelolaan komunikasi antar thread yang aman.
	  /* USER CODE END RTOS_QUEUES */

	  /* Create the thread(s) */
	  /* definition and creation of Tugas1 */
	  osThreadDef(Tugas1, Pertama, osPriorityNormal, 0, 128);  // SEI CERT C: CON34-C — Menggunakan penjadwalan thread dengan prioritas yang sesuai untuk alokasi waktu prosesor.
	  Tugas1Handle = osThreadCreate(osThread(Tugas1), NULL); // SEI CERT C: CON34-C — Penggunaan thread untuk operasi yang dijadwalkan dengan baik.

	  /* definition and creation of Tugas2 */
	  osThreadDef(Tugas2, Kedua, osPriorityIdle, 0, 128);  // SEI CERT C: CON34-C — Penggunaan thread dengan prioritas rendah untuk operasi latar belakang.
	  Tugas2Handle = osThreadCreate(osThread(Tugas2), NULL); // SEI CERT C: CON34-C — Membuat thread kedua untuk manajemen pompa otomatis.

	  /* definition and creation of Tugas3 */
	  osThreadDef(Tugas3, Ketiga, osPriorityIdle, 0, 128);  // SEI CERT C: CON34-C — Menjadwalkan thread untuk operasi dengan prioritas rendah.
	  Tugas3Handle = osThreadCreate(osThread(Tugas3), NULL); // SEI CERT C: CON34-C — Thread ketiga untuk pembaruan LCD.

	  /* definition and creation of Tugas4 */
	  osThreadDef(Tugas4, Keempat, osPriorityIdle, 0, 128);  // SEI CERT C: CON34-C — Membuat thread untuk countdown pompa.
	  Tugas4Handle = osThreadCreate(osThread(Tugas4), NULL); // SEI CERT C: CON34-C — Menggunakan thread keempat untuk countdown dan pengelolaan status pompa.

	  /* USER CODE BEGIN RTOS_THREADS */
	  /* add threads, ... */
	  /* USER CODE END RTOS_THREADS */

	  /* Start scheduler */
	  osKernelStart();  // SEI CERT C: CON34-C — Memulai kernel RTOS, mengelola scheduler untuk pengelolaan thread secara efektif.

	  /* We should never get here as control is now taken by the scheduler */

	  /* Infinite loop */
	  /* USER CODE BEGIN WHILE */
	  while (1)  // SEI CERT C: CON34-C — Thread utama yang tetap berjalan tanpa henti setelah scheduler dimulai.
	  {
	    /* USER CODE END WHILE */

	    /* USER CODE BEGIN 3 */
	  }
	  /* USER CODE END 3 */
	}

	/**
	  * @brief System Clock Configuration
	  * @retval None
	  */
	void SystemClock_Config(void)
	{
	  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	  /** Configure the main internal regulator output voltage
	  */
	  __HAL_RCC_PWR_CLK_ENABLE();  // SEI CERT C: MEM34-C — Mengonfigurasi regulator internal yang aman untuk sistem daya yang lebih stabil.
	  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	  /** Initializes the RCC Oscillators according to the specified parameters
	  * in the RCC_OscInitTypeDef structure.
	  */
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	  RCC_OscInitStruct.PLL.PLLM = 4;
	  RCC_OscInitStruct.PLL.PLLN = 180;
	  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	  RCC_OscInitStruct.PLL.PLLQ = 2;
	  RCC_OscInitStruct.PLL.PLLR = 2;
	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)  // SEI CERT C: ERR30-C — Jika konfigurasi gagal, proses harus menghentikan eksekusi atau menangani kesalahan.
	  {
	    Error_Handler();  // SEI CERT C: ERR33-C — Menangani kesalahan dengan cara yang sesuai dengan protokol.
	  }
	  /** Activate the Over-Drive mode
	    */
	    if (HAL_PWREx_EnableOverDrive() != HAL_OK)  // SEI CERT C: ERR30-C — Setel errno ke nol sebelum memanggil fungsi pustaka yang diketahui akan mengatur errno, dan periksa errno hanya setelah fungsi mengembalikan nilai yang menunjukkan kegagalan. Dalam hal ini, jika overdrive mode gagal diaktifkan, fungsi `Error_Handler()` dipanggil.
	    {
	      Error_Handler();  // SEI CERT C: ERR33-C — Penanganan kesalahan dengan cara yang tepat saat kegagalan terjadi, menghindari sistem melanjutkan dengan konfigurasi yang tidak valid.
	    }

	    /** Initializes the CPU, AHB and APB buses clocks
	    */
	    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;  // SEI CERT C: EXP30-C — Tidak bergantung pada urutan evaluasi untuk efek samping. Menginisialisasi jenis clock yang diperlukan.
	    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;  // SEI CERT C: EXP30-C — Mengonfigurasi sumber clock sistem (dari PLL).
	    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;  // SEI CERT C: EXP30-C — Mengatur pembagi clock AHB.
	    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  // SEI CERT C: EXP30-C — Mengatur pembagi clock APB1.
	    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  // SEI CERT C: EXP30-C — Mengatur pembagi clock APB2.

	    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)  // SEI CERT C: ERR30-C — Jika konfigurasi gagal, proses harus dihentikan atau kesalahan harus ditangani.
	    {
	      Error_Handler();  // SEI CERT C: ERR33-C — Menangani kesalahan jika konfigurasi clock gagal.
	    }
	  }

	  /**
	    * @brief ADC1 Initialization Function
	    * @param None
	    * @retval None
	    */
	  static void MX_ADC1_Init(void)
	  {

	    /* USER CODE BEGIN ADC1_Init 0 */

	    /* USER CODE END ADC1_Init 0 */

	    ADC_ChannelConfTypeDef sConfig = {0};

	    /* USER CODE BEGIN ADC1_Init 1 */

	    /* USER CODE END ADC1_Init 1 */

	    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	    */
	    hadc1.Instance = ADC1;  // SEI CERT C: DCL31-C — Identifier harus dideklarasikan sebelum digunakan. Menetapkan instance ADC1.
	    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;  // SEI CERT C: EXP30-C — Tidak bergantung pada urutan evaluasi untuk efek samping. Menetapkan prescaler clock untuk ADC.
	    hadc1.Init.Resolution = ADC_RESOLUTION_12B;  // SEI CERT C: EXP30-C — Menetapkan resolusi ADC menjadi 12 bit untuk akurasi yang lebih tinggi.
	    hadc1.Init.ScanConvMode = DISABLE;  // SEI CERT C: EXP30-C — Menonaktifkan mode konversi pemindaian karena hanya satu saluran ADC yang digunakan.
	    hadc1.Init.ContinuousConvMode = DISABLE;  // SEI CERT C: EXP30-C — Mode konversi kontinu dimatikan, hanya konversi satu kali.
	    hadc1.Init.DiscontinuousConvMode = DISABLE;  // SEI CERT C: EXP30-C — Mode konversi terputus dimatikan.
	    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;  // SEI CERT C: EXP30-C — Tidak ada pemicu eksternal untuk konversi ADC.
	    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;  // SEI CERT C: EXP30-C — Konversi ADC dimulai dengan perintah perangkat lunak.
	    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;  // SEI CERT C: EXP30-C — Menyelaraskan data hasil konversi ke kanan.
	    hadc1.Init.NbrOfConversion = 1;  // SEI CERT C: EXP30-C — Hanya satu konversi yang diperlukan.
	    hadc1.Init.DMAContinuousRequests = DISABLE;  // SEI CERT C: EXP30-C — Menonaktifkan permintaan DMA secara kontinu.
	    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;  // SEI CERT C: EXP30-C — Mengonfigurasi pemilihan akhir konversi untuk mode satu konversi.

	    if (HAL_ADC_Init(&hadc1) != HAL_OK)  // SEI CERT C: ERR30-C — Menangani kesalahan jika inisialisasi ADC gagal.
	    {
	      Error_Handler();  // SEI CERT C: ERR33-C — Menangani kesalahan jika inisialisasi ADC gagal.
	    }

	    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	    */
	    sConfig.Channel = ADC_CHANNEL_0;  // SEI CERT C: DCL31-C — Identifier harus dideklarasikan sebelum digunakan. Menetapkan saluran ADC yang akan digunakan.
	    sConfig.Rank = 1;  // SEI CERT C: DCL31-C — Menetapkan peringkat saluran ADC dalam urutan konversi.
	    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;  // SEI CERT C: EXP30-C — Menetapkan waktu sampling untuk saluran ADC.
	    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)  // SEI CERT C: ERR30-C — Menangani kesalahan jika konfigurasi saluran ADC gagal.
	    {
	      Error_Handler();  // SEI CERT C: ERR33-C — Menangani kesalahan jika konfigurasi saluran gagal.
	    }

	    /* USER CODE BEGIN ADC1_Init 2 */

	    /* USER CODE END ADC1_Init 2 */

	  }


	  /**
	    * @brief I2C1 Initialization Function
	    * @param None
	    * @retval None
	    */
	  static void MX_I2C1_Init(void)
	  {

	    /* USER CODE BEGIN I2C1_Init 0 */

	    /* USER CODE END I2C1_Init 0 */

	    /* USER CODE BEGIN I2C1_Init 1 */

	    /* USER CODE END I2C1_Init 1 */
	    hi2c1.Instance = I2C1;  // SEI CERT C: DCL31-C — Identifier harus dideklarasikan sebelum digunakan. Inisialisasi instance I2C1.
	    hi2c1.Init.ClockSpeed = 400000;  // SEI CERT C: EXP30-C — Mengatur kecepatan clock untuk I2C sesuai kebutuhan aplikasi.
	    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;  // SEI CERT C: EXP30-C — Menetapkan siklus tugas untuk I2C, memastikan pengoperasian yang efisien.
	    hi2c1.Init.OwnAddress1 = 0;  // SEI CERT C: DCL31-C — Mengatur alamat perangkat I2C pertama.
	    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;  // SEI CERT C: EXP30-C — Menetapkan mode pengalamatan 7-bit untuk I2C.
	    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;  // SEI CERT C: EXP30-C — Nonaktifkan mode dual-address untuk pengaturan I2C.
	    hi2c1.Init.OwnAddress2 = 0;  // SEI CERT C: DCL31-C — Mengatur alamat kedua (jika digunakan) pada perangkat I2C.
	    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;  // SEI CERT C: EXP30-C — Nonaktifkan mode panggilan umum untuk I2C.
	    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;  // SEI CERT C: EXP30-C — Nonaktifkan mode tanpa penundaan (no-stretching) pada I2C.

	    if (HAL_I2C_Init(&hi2c1) != HAL_OK)  // SEI CERT C: ERR30-C — Menangani kesalahan jika inisialisasi I2C gagal.
	    {
	      Error_Handler();  // SEI CERT C: ERR33-C — Memanggil fungsi penanganan kesalahan jika inisialisasi gagal.
	    }
	    /* USER CODE BEGIN I2C1_Init 2 */

	    /* USER CODE END I2C1_Init 2 */

	  }

	  /**
	    * @brief USART2 Initialization Function
	    * @param None
	    * @retval None
	    */
	  static void MX_USART2_UART_Init(void)
	  {

	    /* USER CODE BEGIN USART2_Init 0 */

	    /* USER CODE END USART2_Init 0 */

	    /* USER CODE BEGIN USART2_Init 1 */

	    /* USER CODE END USART2_Init 1 */
	    huart2.Instance = USART2;  // SEI CERT C: DCL31-C — Identifier harus dideklarasikan sebelum digunakan. Menetapkan instance USART2 untuk komunikasi serial.
	    huart2.Init.BaudRate = 115200;  // SEI CERT C: EXP30-C — Mengatur kecepatan baud untuk komunikasi UART.
	    huart2.Init.WordLength = UART_WORDLENGTH_8B;  // SEI CERT C: EXP30-C — Mengatur panjang kata data UART menjadi 8 bit.
	    huart2.Init.StopBits = UART_STOPBITS_1;  // SEI CERT C: EXP30-C — Menetapkan satu bit stop untuk komunikasi UART.
	    huart2.Init.Parity = UART_PARITY_NONE;  // SEI CERT C: EXP30-C — Menonaktifkan pemeriksaan paritas untuk komunikasi UART.
	    huart2.Init.Mode = UART_MODE_TX_RX;  // SEI CERT C: EXP30-C — Mengaktifkan mode pengiriman dan penerimaan UART.
	    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;  // SEI CERT C: EXP30-C — Menonaktifkan kontrol aliran perangkat keras untuk UART.
	    huart2.Init.OverSampling = UART_OVERSAMPLING_16;  // SEI CERT C: EXP30-C — Mengaktifkan oversampling untuk komunikasi UART.

	    if (HAL_UART_Init(&huart2) != HAL_OK)  // SEI CERT C: ERR30-C — Menangani kesalahan jika inisialisasi UART gagal.
	    {
	      Error_Handler();  // SEI CERT C: ERR33-C — Memanggil fungsi penanganan kesalahan jika inisialisasi gagal.
	    }
	    /* USER CODE BEGIN USART2_Init 2 */

	    /* USER CODE END USART2_Init 2 */

	  }

	  /**
	    * Enable DMA controller clock
	    */
	  static void MX_DMA_Init(void)
	  {

	    /* DMA controller clock enable */
	    __HAL_RCC_DMA2_CLK_ENABLE();  // SEI CERT C: MEM34-C — Mengaktifkan clock DMA2 untuk pengelolaan transfer data.

	    /* DMA interrupt init */
	    /* DMA2_Stream0_IRQn interrupt configuration */
	    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);  // SEI CERT C: CON34-C — Mengonfigurasi prioritas interrupt DMA untuk memastikan pengelolaan interrupt yang efisien.
	    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);  // SEI CERT C: CON34-C — Mengaktifkan interrupt DMA untuk pemrosesan data.

	  }

	  /**
	    * @brief GPIO Initialization Function
	    * @param None
	    * @retval None
	    */
	  static void MX_GPIO_Init(void)
	  {
	    GPIO_InitTypeDef GPIO_InitStruct = {0};
	    /* USER CODE BEGIN MX_GPIO_Init_1 */
	    /* USER CODE END MX_GPIO_Init_1 */

	    /* GPIO Ports Clock Enable */
	    __HAL_RCC_GPIOC_CLK_ENABLE();  // SEI CERT C: MEM34-C — Mengaktifkan clock untuk port GPIOC untuk memastikan port dapat digunakan.
	    __HAL_RCC_GPIOH_CLK_ENABLE();  // SEI CERT C: MEM34-C — Mengaktifkan clock untuk port GPIOH.
	    __HAL_RCC_GPIOA_CLK_ENABLE();  // SEI CERT C: MEM34-C — Mengaktifkan clock untuk port GPIOA.
	    __HAL_RCC_GPIOB_CLK_ENABLE();  // SEI CERT C: MEM34-C — Mengaktifkan clock untuk port GPIOB.

	    /*Configure GPIO pin Output Level */
	    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);  // SEI CERT C: EXP30-C — Menulis level output pin GPIO untuk mengatur status LED (off).

	    /*Configure GPIO pin Output Level */
	    HAL_GPIO_WritePin(GPIOB, RELAY_PUMP_Pin|GPIO_PIN_5, GPIO_PIN_RESET);  // SEI CERT C: EXP30-C — Menulis level output pin GPIO untuk mengatur status relay pompa (off).

	    /*Configure GPIO pin : B1_Pin */
	    GPIO_InitStruct.Pin = B1_Pin;  // SEI CERT C: DCL31-C — Menetapkan pin B1 untuk input eksternal.
	    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;  // SEI CERT C: EXP30-C — Mengonfigurasi pin B1 sebagai input dengan interrupt pada sisi jatuh (falling edge).
	    GPIO_InitStruct.Pull = GPIO_NOPULL;  // SEI CERT C: EXP30-C — Menonaktifkan pull-up/pull-down pada pin B1.
	    HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);  // SEI CERT C: DCL31-C — Inisialisasi pin GPIO dengan pengaturan yang ditetapkan.

	    /*Configure GPIO pin : LD2_Pin */
	    GPIO_InitStruct.Pin = LD2_Pin;  // SEI CERT C: DCL31-C — Mengonfigurasi pin LD2 untuk output.
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // SEI CERT C: EXP30-C — Mengonfigurasi pin LD2 sebagai output push-pull.
	    GPIO_InitStruct.Pull = GPIO_NOPULL;  // SEI CERT C: EXP30-C — Menonaktifkan pull-up/pull-down pada pin LD2.
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;  // SEI CERT C: EXP30-C — Menetapkan kecepatan rendah untuk pin LD2.
	    HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);  // SEI CERT C: DCL31-C — Inisialisasi pin GPIO LD2.

	    /*Configure GPIO pins : RELAY_PUMP_Pin PB5 */
	    GPIO_InitStruct.Pin = RELAY_PUMP_Pin|GPIO_PIN_5;  // SEI CERT C: DCL31-C — Menetapkan pin untuk relay pompa dan pin GPIO lainnya.
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // SEI CERT C: EXP30-C — Mengonfigurasi pin sebagai output push-pull.
	    GPIO_InitStruct.Pull = GPIO_NOPULL;  // SEI CERT C: EXP30-C — Menonaktifkan pull-up/pull-down pada pin GPIO.
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;  // SEI CERT C: EXP30-C — Menetapkan kecepatan rendah untuk pin GPIO.
	    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);  // SEI CERT C: DCL31-C — Inisialisasi pin GPIO untuk relay pompa.

	    /* USER CODE BEGIN MX_GPIO_Init_2 */
	    /* USER CODE END MX_GPIO_Init_2 */
	    }

	    /* USER CODE BEGIN 4 */

	    /* USER CODE END 4 */

	    /* USER CODE BEGIN Header_Pertama */
	    /**
	      * @brief  Function implementing the Tugas1 thread.
	      * @param  argument: Not used
	      * @retval None
	      */
	    /* USER CODE END Header_Pertama */
	    void Pertama(void const * argument)
	    {
	      /* USER CODE BEGIN 5 */
	      /* Infinite loop */
	      for(;;)  // SEI CERT C: CON34-C — Gunakan pengendalian alur yang jelas dalam loop tak terbatas untuk menghindari kebingungannya alur eksekusi. Ini adalah thread yang memproses waktu setiap detik.
	      {
	        ds3231_get_time(&currentTime);  // Membaca waktu RTC
	        osDelay(1000);  // SEI CERT C: CON34-C — Menggunakan penjadwalan thread dengan delay yang sesuai agar sistem tidak terkunci dalam operasi panjang tanpa respons.
	      }
	      /* USER CODE END 5 */
	    }

	    /* USER CODE BEGIN Header_Kedua */
	    /**
	    * @brief Function implementing the Tugas2 thread.
	    * @param argument: Not used
	    * @retval None
	    */
	    /* USER CODE END Header_Kedua */
	    void Kedua(void const * argument)
	    {
	      /* USER CODE BEGIN Kedua */
	      /* Infinite loop */
	      for(;;)  // SEI CERT C: CON34-C — Menggunakan loop tak terbatas untuk memeriksa waktu dan mengontrol status pompa.
	      {
	        // Cek waktu untuk menyalakan/mematikan pompa secara otomatis
	        if ((currentTime.hours == 6 && currentTime.minutes < 30) ||
	            (currentTime.hours == 16 && currentTime.minutes < 30))  // SEI CERT C: EXP30-C — Menggunakan perbandingan waktu dengan akurat.
	        {
	          if (pompaStatus == 0) // Pompa masih mati
	          {
	            HAL_GPIO_WritePin(RELAY_PUMP_GPIO_Port, RELAY_PUMP_Pin, GPIO_PIN_SET); // Nyalakan pompa
	            pompaStatus = 1;  // SEI CERT C: MEM30-C — Mengubah status pompa dengan tepat untuk menjaga konsistensi status.
	            countdown = 60;  // SEI CERT C: MEM30-C — Menetapkan countdown ketika pompa dinyalakan.
	            printf("Pompa ON: %02d:%02d:%02d\r\n", currentTime.hours, currentTime.minutes, currentTime.seconds);
	          }
	        }
	        else
	        {
	          if (pompaStatus == 1) // Pompa sedang menyala
	          {
	            HAL_GPIO_WritePin(RELAY_PUMP_GPIO_Port, RELAY_PUMP_Pin, GPIO_PIN_RESET); // Matikan pompa
	            pompaStatus = 0;  // SEI CERT C: MEM30-C — Mengubah status pompa dengan tepat ketika dimatikan.
	            printf("Pompa OFF: %02d:%02d:%02d\r\n", currentTime.hours, currentTime.minutes, currentTime.seconds);
	          }
	        }
	        if (countdown == 0 && pompaStatus == 1) {  // SEI CERT C: EXP33-C — Pastikan untuk memeriksa kondisi valid sebelum menggunakan variabel dalam operasi.
	          HAL_GPIO_WritePin(RELAY_PUMP_GPIO_Port, RELAY_PUMP_Pin, GPIO_PIN_RESET);
	          pompaStatus = 0;
	        }
	        osDelay(1000);  // SEI CERT C: CON34-C — Menggunakan delay untuk interval 1 detik agar sistem tetap responsif.
	      }
	      /* USER CODE END Kedua */
	    }

	    /* USER CODE BEGIN Header_Ketiga */
	    /**
	    * @brief Function implementing the Tugas3 thread.
	    * @param argument: Not used
	    * @retval None
	    */
	    /* USER CODE END Header_Ketiga */
	    void Ketiga(void const * argument)
	    {
	      /* USER CODE BEGIN Ketiga */
	      char buffer[20]; // SEI CERT C: MEM30-C — Pastikan bahwa buffer memiliki ukuran yang cukup untuk menyimpan string yang diproses.
	      for(;;) {
	        lcd_clear();  // SEI CERT C: EXP30-C — Menggunakan fungsi untuk menghapus layar, memastikan tampilan yang konsisten.
	        lcd_put_cur(0, 0);
	        sprintf(buffer, "Time: %02d:%02d:%02d", currentTime.hours, currentTime.minutes, currentTime.seconds);  // SEI CERT C: FIO30-C — Hindari format string dari input yang tidak tepercaya.
	        lcd_send_string(buffer); // Kirim buffer ke LCD

	        lcd_put_cur(1, 0);
	        if (pompaStatus == 1) { // Pastikan variabel konsisten (pompaStatus, bukan pumpOn)
	          sprintf(buffer, "Pompa ON - %02d:%02d", countdown / 60, countdown % 60);  // SEI CERT C: FIO30-C — Hindari format string yang tidak tepercaya.
	          lcd_send_string(buffer);
	        } else {
	          lcd_send_string("Pompa OFF");
	        }
	        if (pompaStatus == 1 && countdown > 0)  // SEI CERT C: EXP30-C — Menggunakan logika kondisi untuk menghitung countdown.
	        {
	          countdown--;  // SEI CERT C: MEM30-C — Mengurangi countdown setelah setiap detik.
	        }
	        osDelay(1000);  // SEI CERT C: CON34-C — Menunda eksekusi agar thread tetap responsif dan tidak terlalu sering memperbarui LCD.
	      }
	      /* USER CODE END Ketiga */
	    }

	    /* USER CODE BEGIN Header_Keempat */
	    /**
	    * @brief Function implementing the Tugas4 thread.
	    * @param argument: Not used
	    * @retval None
	    */
	    /* USER CODE END Header_Keempat */
	    void Keempat(void const * argument)
	    {
	      /* USER CODE BEGIN Keempat */
	      /* Infinite loop */
	      for(;;)
	      {
	        // Countdown jika pompa aktif
	        if (pompaStatus && countdown > 0)  // SEI CERT C: EXP33-C — Pastikan kondisi valid sebelum menggunakan variabel dalam operasi.
	        {
	          countdown--;  // SEI CERT C: MEM30-C — Mengurangi countdown setiap detik.
	        }

	        osDelay(1000);  // SEI CERT C: CON34-C — Menunda eksekusi agar sistem tetap responsif dan menghitung countdown secara berkelanjutan.
	      }
	      /* USER CODE END Keempat */
	    }

	    /**
	      * @brief  Period elapsed callback in non blocking mode
	      * @note   This function is called  when TIM2 interrupt took place, inside
	      * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
	      * a global variable "uwTick" used as application time base.
	      * @param  htim : TIM handle
	      * @retval None
	      */
	    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
	    {
	      /* USER CODE BEGIN Callback 0 */

	      /* USER CODE END Callback 0 */
	      if (htim->Instance == TIM2) {  // SEI CERT C: EXP30-C — Memastikan bahwa interupsi berasal dari timer yang tepat untuk menghindari kesalahan pemrosesan.
	        HAL_IncTick();  // SEI CERT C: EXP30-C — Memperbarui timer global yang digunakan untuk dasar waktu aplikasi.
	      }
	      /* USER CODE BEGIN Callback 1 */

	      /* USER CODE END Callback 1 */
	    }

	    /**
	      * @brief  This function is executed in case of error occurrence.
	      * @retval None
	      */
	    void Error_Handler(void)
	    {
	      /* USER CODE BEGIN Error_Handler_Debug */
	      /* User can add his own implementation to report the HAL error return state */
	      __disable_irq();  // SEI CERT C: ERR33-C — Menonaktifkan interupsi untuk mencegah eksekusi lebih lanjut jika terjadi kesalahan.
	      while (1)
	      {
	      }
	      /* USER CODE END Error_Handler_Debug */
	    }

	    #ifdef  USE_FULL_ASSERT
	    /**
	      * @brief  Reports the name of the source file and the source line number
	      *         where the assert_param error has occurred.
	      * @param  file: pointer to the source file name
	      * @param  line: assert_param error line source number
	      * @retval None
	      */
	    void assert_failed(uint8_t *file, uint32_t line)
	    {
	      /* USER CODE BEGIN 6 */
	      /* User can add his own implementation to report the file name and line number,
	         ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	      /* USER CODE END 6 */
	    }
	    #endif /* USE_FULL_ASSERT */
