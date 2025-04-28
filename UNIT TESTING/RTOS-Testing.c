#include <stdio.h>
#include <assert.h>

// Mendefinisikan RTC_TimeTypeDef untuk simulasi waktu
typedef struct {
    int hours;
    int minutes;
    int seconds;
} RTC_TimeTypeDef;

// Variabel global yang digunakan
int pompaStatus = 0;
int countdown = 0;
RTC_TimeTypeDef currentTime;

// Fungsi Kedua yang mengatur logika pompa berdasarkan waktu
void Kedua(void const * argument) {
    // Reset countdown dan status pompa untuk setiap pengujian
    pompaStatus = 0;
    countdown = 0;

    // Logika pompa berdasarkan jam dan menit
    if ((currentTime.hours == 6 && currentTime.minutes < 30) || 
        (currentTime.hours == 16 && currentTime.minutes < 30)) {
        if (pompaStatus == 0) {
            pompaStatus = 1;
            countdown = 30 - currentTime.minutes; // Countdown dalam menit, mulai dari 30 menit
            printf("Pompa ON: %02d:%02d:%02d, Countdown: %d menit\n", currentTime.hours, currentTime.minutes, currentTime.seconds, countdown);
        }
    } else {
        if (pompaStatus == 1) {
            pompaStatus = 0;
            printf("Pompa OFF: %02d:%02d:%02d\n", currentTime.hours, currentTime.minutes, currentTime.seconds);
        }
    }

    if (countdown == 0 && pompaStatus == 1) {
        pompaStatus = 0;
    }
}

// Fungsi untuk interaktif: Memasukkan waktu dan menghitung countdown
void test_pompa() {
    char lanjut;
    do {
        // Input jam dan menit dari pengguna
        printf("Masukkan jam (0-23): ");
        scanf("%d", &currentTime.hours);

        printf("Masukkan menit (0-59): ");
        scanf("%d", &currentTime.minutes);

        // Validasi input jam dan menit
        if (currentTime.hours < 0 || currentTime.hours > 23 || currentTime.minutes < 0 || currentTime.minutes > 59) {
            printf("Input tidak valid! Silakan coba lagi.\n");
            continue;  // Kembali ke input jam dan menit jika input salah
        }

        currentTime.seconds = 0;  // Set detik ke 0 untuk kesederhanaan

        // Menjalankan fungsi Kedua untuk mengatur pompa
        Kedua(NULL);

        // Menampilkan status pompa dan countdown dalam menit
        if (pompaStatus == 1) {
            printf("Status POMPA: ON, Countdown: %d menit\n", countdown);
        } else {
            printf("Status POMPA: OFF\n");
        }

        // Tanyakan apakah ingin mencoba lagi
        printf("Coba lagi? (y/n): ");
        scanf(" %c", &lanjut);  // Spasi sebelum %c untuk menghindari masalah dengan karakter newline

    } while (lanjut == 'y' || lanjut == 'Y');

    printf("Tes selesai. Terima kasih!\n");
}

int main() {
    // Menjalankan tes interaktif
    test_pompa();
    return 0;
}

