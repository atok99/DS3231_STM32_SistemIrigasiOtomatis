# 🌱 Sistem Irigasi Otomatis Berbasis RTC DS3231 dengan FreeRTOS

Proyek ini adalah sistem irigasi otomatis berbasis STM32 Nucleo-F446RE yang memanfaatkan RTC DS3231 untuk menjadwalkan waktu penyiraman tanaman secara otomatis. Pompa dikendalikan menggunakan relay, dan status sistem ditampilkan melalui LCD I2C 16x2. Sistem ini dijalankan menggunakan FreeRTOS untuk mengatur multitasking agar lebih efisien dalam penggunaan memori dan sumber daya STM32.

---

## Authors
1. Rizal Khoirul Atok (2042231013)
2. Egga Terbyd Fabryan (2042231029)
3. Valencia Christina Setiowardhani (2042231055)
4. Ahmad Radhy (Supervisor)

Teknik Instrumentasi - Institut Teknologi Sepuluh Nopember

---

## 🛠️ Tools yang Digunakan

- STM32CubeIDE (konfigurasi awal mikrokontroler)
- Keil uVision5 (development dan kompilasi program)
- Visual Studio Code (unit testing kode C)
- STM32 Nucleo-F446RE (mikrokontroler utama)
- RTC DS3231 (modul waktu real-time via I2C)
- LCD I2C 16x2 (output status waktu & pompa)
- Relay (untuk kontrol aktuator pompa atau katup air)
- ST-Link (untuk flashing ke STM32)
- Unity Framework (untuk unit testing logika program)

---

## ⏱️ Tentang FreeRTOS dalam Sistem Ini
FreeRTOS adalah sistem operasi real-time yang digunakan untuk menjalankan beberapa fungsi (task) secara bersamaan secara terjadwal. Dengan menggunakan FreeRTOS, sistem menjadi:

- Lebih modular
- Lebih efisien dalam penggunaan memori
- Lebih mudah untuk dikembangkan dan di-debug

Sistem ini menjalankan empat task utama secara paralel:

1. Task Pembacaan Waktu RTC
Membaca waktu saat ini dari modul DS3231 secara berkala.
2. Task Kontrol Pompa
Menyalakan atau mematikan pompa berdasarkan jadwal waktu yang ditentukan (06:00 dan 16:00 selama 30 menit).
3. Task Tampilan LCD
Menampilkan waktu, status pompa (ON/OFF), dan sisa waktu countdown ketika pompa sedang aktif.
4. Task Countdown
Menghitung mundur selama 30 menit saat pompa aktif, dan otomatis mematikan pompa saat countdown selesai.

---

## 🧱 Arsitektur Sistem

```plaintext
[RTC DS3231] --(I2C)--> [STM32 Nucleo-F446RE] --(GPIO)--> [Relay] --> [Pompa]
                                     |
                                (I2C)
                                     |
                                [LCD 16x2]

```
---

## 🔁 Flowchart Sistem Irigasi Otomatis
```plaintext
           +-------------------------+
           |   Mulai Program (Init)  |
           +-----------+-------------+
                       |
                       v
         +-------------+--------------+
         | Inisialisasi RTC, LCD, dll |
         +-------------+--------------+
                       |
                       v
           +-----------+-----------+
           |   Buat dan Jalankan   |
           |      Empat Task       |
           +-----------+-----------+
                       |
             +---------+----------+----------+----------+
             |                    |                     |
             v                    v                     v
  +----------------+   +-------------------+   +------------------+
  | Task 1: Baca   |   | Task 2: Kontrol    |   | Task 3: Tampilkan|
  | RTC DS3231     |   | Relay berdasarkan  |   | waktu & status   |
  | per detik      |   | jadwal             |   | ke LCD I2C       |
  +----------------+   +-------------------+   +------------------+
             |                    |                     |
             |                    v                     |
             |          +-------------------+           |
             |          | Task 4: Countdown |<----------+
             |          | saat pompa ON     |
             |          +-------------------+
             |                    |
             +--------------------> [Matikan Pompa]
                                  jika countdown habis

```
---

## 🔧 Langkah-Langkah Pengembangan

### 1. Konfigurasi di STM32CubeIDE

1. Pilih Board: STM32 Nucleo-F446RE  
2. Aktifkan:
   - I2C1 : komunikasi RTC DS3231 & LCD I2C
   - GPIO Output (untuk relay)
   - FreeRTOS: aktifkan dan konfigurasi 4 task
3. Atur clock dan konfigurasi pin
4. Generate project

### 2. Pengembangan Firmware Code C

- Task 1: Baca waktu dari DS3231 dan simpan ke variabel global
- Task 2: Cek apakah waktu sekarang == 06:00 atau 16:00 → Aktifkan relay
- Task 3: Tampilkan waktu, status relay, dan countdown di LCD
- Task 4: Hitung mundur 30 menit jika pompa ON, lalu matikan relay

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
STM32_Irigasi_FreeRTOS/
│
├── Core/                  # Kode utama dari STM32CubeIDE (main.c, task.c, dll)
│
├── Drivers/               # Driver HAL dan user-defined
│   └── STM32F4xx_HAL_Driver/
│
├── include/               # Header files buatan sendiri
│   ├── main.h
│   ├── ds3231.h
│   └── lcd_i2c.h
│
├── src/                   # Implementasi logika (tanpa akses hardware langsung)
│   ├── main.c
│   ├── ds3231.c
│   └── lcd_i2c.c
│
├── tests/                 # Unit testing dengan Unity
│   └── RTOS-Testing.c
│
├── unity/                 # Framework Unity C
│   ├── unity.c
│   └── unity.h
│
│
└── README.md              # Dokumentasi proyek

```


## ✅ Fitur

- ⏰ Pembacaan waktu real dari RTC DS3231

- 🚿 Penyiraman otomatis berdasarkan waktu (06:00 dan 16:00 selama 30 menit)

- 🔁 Countdown timer saat pompa aktif

- 📺 Tampilan waktu dan status sistem pada LCD I2C

- 🧪 Unit testing logika program untuk meningkatkan keandalan kode

- ⚙️ Multitasking efisien menggunakan FreeRTOS
---

## ✅ PENGUJIAN
### 1. PUMP-ON
![alt text](https://github.com/atok99/DS3231_STM32_SistemIrigasiOtomatis/blob/main/PUMPON.png?raw=true)

### 2. PUMP-OFF
![alt text](https://github.com/atok99/DS3231_STM32_SistemIrigasiOtomatis/blob/main/PUMPOFF.png?raw=true)


---
