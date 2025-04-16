# 🌱 Sistem Irigasi Otomatis Berbasis Timer RTC DS3231

Proyek ini adalah sistem irigasi otomatis berbasis mikrokontroler STM32F401CCU6 yang menggunakan modul RTC DS3231 sebagai penentu waktu penyiraman tanaman secara otomatis. Output penyiraman dikontrol melalui relay.

---

## 🛠️ Tools yang Digunakan

- STM32CubeMX (konfigurasi awal mikrokontroler)
- Keil uVision5 (development dan kompilasi program)
- Visual Studio Code (unit testing kode C)
- STM32F401CCU6 (mikrokontroler utama)
- RTC DS3231 (modul waktu real-time via I2C)
- Relay (untuk kontrol aktuator pompa atau katup air)
- ST-Link (untuk flashing ke STM32)

---

## 🧱 Arsitektur Umum Sistem

```plaintext
[RTC DS3231] ---> [STM32F401CCU6] ---> [Relay] ---> [Pompa / Katup Air]
       (I2C)              (GPIO Output)

```
---

## 🔧 Langkah-Langkah Pengembangan

### 1. Konfigurasi di STM32CubeMX

1. Pilih MCU: STM32F401CCU6  
2. Aktifkan:
   - I2C1 (komunikasi dengan RTC DS3231)
   - GPIO Output (untuk relay)
3. Atur clock dan konfigurasi pin
4. Generate project dan pilih toolchain: **MDK-ARM (Keil)**

### 2. Pengembangan Firmware di Keil uVision5

1. Buka project hasil generate dari STM32CubeMX  
2. Tambahkan file `ds3231.c` dan `ds3231.h` ke dalam project  
3. Implementasikan:
   - Fungsi pembacaan waktu dari DS3231
   - Perbandingan waktu saat ini dengan jadwal penyiraman
   - Aktivasi GPIO output jika waktunya sesuai  
4. Build dan upload ke board STM32 menggunakan ST-Link

### 3. Unit Testing di Visual Studio Code

1. Buat folder `tests/` dan `src/` di project root  
2. Tambahkan logika program (non-hardware dependent) di `src/logic.c`  
3. Buat file test seperti `tests/test_logic.c`  
4. Gunakan Unity Framework untuk mengetes fungsi-fungsi logika  
5. Compile dan jalankan test

---

## 📁 Struktur Folder

```plaintext
STM32_Irigasi/
│
├── Core/                  # Kode utama dari STM32CubeMX (main.c, stm32f4xx_it.c, dll)
│
├── Drivers/               # Driver HAL dan user-defined
│   └── STM32F4xx_HAL_Driver/
│
├── include/               # Header files (.h) buatan sendiri
│   ├── logic.h
│   └── ds3231.h
│
├── src/                   # Logika penyiraman (tanpa akses langsung ke hardware)
│   ├── logic.c
│   └── ds3231.c
│
├── tests/                 # Unit testing dengan Unity
│   └── test_logic.c
│
├── unity/                 # Unity framework untuk unit test
│   ├── unity.c
│   └── unity.h
│
├── MDK-ARM/               # Folder project Keil uVision (.uvprojx, .uvoptx)
│
└── README.md              # Dokumentasi proyek
```


## ✅ Fitur

- Pembacaan waktu dari RTC DS3231 via I2C

- Penyiraman otomatis berdasarkan waktu yang dijadwalkan

- Kontrol pompa/katup irigasi melalui relay

- Unit testing untuk logika program menggunakan Unity C framework
