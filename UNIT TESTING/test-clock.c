#include <stdio.h>
#include <stdint.h>

// Simulasi implementasi konversi BCD ke desimal
static int bcd2dec(uint8_t b) {
    return ((b / 16) * 10 + (b % 16));
}

// Fungsi pengujian ds3231_seconds
uint8_t ds3231_seconds(void) {
    uint8_t bcd_seconds = 0x45; // Misalnya, detik BCD yang disimulasikan adalah 0x45 (45 detik)
    return bcd2dec(bcd_seconds);
}

// Fungsi pengujian ds3231_minutes
uint8_t ds3231_minutes(void) {
    uint8_t bcd_minutes = 0x30; // Misalnya, menit BCD yang disimulasikan adalah 0x30 (30 menit)
    return bcd2dec(bcd_minutes);
}

// Fungsi pengujian ds3231_hours
uint8_t ds3231_hours(void) {
    uint8_t bcd_hours = 0x12; // Misalnya, jam BCD yang disimulasikan adalah 0x12 (12 jam)
    return bcd2dec(bcd_hours);
}

// Fungsi untuk menguji ds3231_seconds
void test_ds3231_seconds(uint8_t bcd_seconds) {
    uint8_t expected_result = bcd2dec(bcd_seconds);
    uint8_t result = ds3231_seconds();

    printf("Input BCD detik: 0x%02X\n", bcd_seconds);
    printf("Hasil konversi (desimal): %d\n", expected_result);
    printf("Hasil dari fungsi ds3231_seconds: %d\n", result);

    if (result == expected_result) {
        printf("Test PASSED\n\n");
    } else {
        printf("Test FAILED: Expected %d but got %d\n\n", expected_result, result);
    }
}

// Fungsi untuk menguji ds3231_minutes
void test_ds3231_minutes(uint8_t bcd_minutes) {
    uint8_t expected_result = bcd2dec(bcd_minutes);
    uint8_t result = ds3231_minutes();

    printf("Input BCD menit: 0x%02X\n", bcd_minutes);
    printf("Hasil konversi (desimal): %d\n", expected_result);
    printf("Hasil dari fungsi ds3231_minutes: %d\n", result);

    if (result == expected_result) {
        printf("Test PASSED\n\n");
    } else {
        printf("Test FAILED: Expected %d but got %d\n\n", expected_result, result);
    }
}

// Fungsi untuk menguji ds3231_hours
void test_ds3231_hours(uint8_t bcd_hours) {
    uint8_t expected_result = bcd2dec(bcd_hours);
    uint8_t result = ds3231_hours();

    printf("Input BCD jam: 0x%02X\n", bcd_hours);
    printf("Hasil konversi (desimal): %d\n", expected_result);
    printf("Hasil dari fungsi ds3231_hours: %d\n", result);

    if (result == expected_result) {
        printf("Test PASSED\n\n");
    } else {
        printf("Test FAILED: Expected %d but got %d\n\n", expected_result, result);
    }
}

int main() {
    uint8_t bcd_seconds, bcd_minutes, bcd_hours;

    // Input detik BCD dari pengguna
    printf("Masukkan nilai detik dalam format BCD (misalnya 0x45 untuk detik 45): ");
    scanf("%hhx", &bcd_seconds);
    test_ds3231_seconds(bcd_seconds);

    // Input menit BCD dari pengguna
    printf("Masukkan nilai menit dalam format BCD (misalnya 0x30 untuk menit 30): ");
    scanf("%hhx", &bcd_minutes);
    test_ds3231_minutes(bcd_minutes);

    // Input jam BCD dari pengguna
    printf("Masukkan nilai jam dalam format BCD (misalnya 0x12 untuk jam 12): ");
    scanf("%hhx", &bcd_hours);
    test_ds3231_hours(bcd_hours);

    return 0;
}
