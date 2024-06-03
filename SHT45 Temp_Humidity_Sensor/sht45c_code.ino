//The code is completely in C programming
//not Arduino compatabile

#include <stdio.h>
#include <stdint.h>
#include <unistd.h> // for usleep

// Define I2C addresses and commands
#define SHT45_I2C_ADDR 0x44
#define MEASURE_CMD 0xFD

// Define I2C functions
// These functions should be implemented according to your platform's I2C library
// For example, if you're using Linux, you can use ioctl with I2C_RDWR to communicate with I2C devices.

void i2c_begin() {
    // Initialize I2C communication
    // Implement according to your platform
}

int i2c_write(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len) {
    // Write data to I2C device
    // Implement according to your platform
    return 0; // Return 0 for success, -1 for failure
}

int i2c_read(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len) {
    // Read data from I2C device
    // Implement according to your platform
    return 0; // Return 0 for success, -1 for failure
}

// Define delay function
void delay_ms(int ms) {
    delay(ms); // Arduino delay function
}



// Define SHT45 class
typedef struct {
    uint8_t addr;
    float t, h;
} sht45;

// Function prototypes
void sht45_init(sht45 *sht45, uint8_t addr);
int sht45_begin(sht45 *sht45);
int sht45_measure(sht45 *sht45);
float sht45_temperature(sht45 *sht45);
float sht45_humidity(sht45 *sht45);

// Main function
int main() {
    sht45 sht45;
    sht45_init(&sht45, SHT45_I2C_ADDR);

    while (!sht45_begin(&sht45)) {
        printf("SHT45 not found !\n");
        delay_ms(1000);
    }

    while (1) {
        if (sht45_measure(&sht45)) {
            printf("Temperature: %.1f *C\tHumidity: %.1f %%RH\n", sht45_temperature(&sht45), sht45_humidity(&sht45));

            if (sht45_temperature(&sht45) >= 30) {
                // Do something
            }
        } else {
            printf("SHT45 read error\n");
        }
        delay_ms(1000);
    }

    return 0;
}

// Function definitions

void sht45_init(sht45 *sht45, uint8_t addr) {
    sht45->addr = addr;
}

int sht45_begin(sht45 *sht45) {
    i2c_begin();
    return i2c_write(sht45->addr, 0, NULL, 0); // Perform I2C connection test
}

int sht45_measure(sht45 *sht45) {
    uint8_t tx_data[1] = {MEASURE_CMD};
    if (i2c_write(sht45->addr, 0, tx_data, 1) != 0) {
        return 0; // Error in writing command
    }

    delay_ms(10); // Wait for measurement to complete

    uint8_t rx_data[6];
    if (i2c_read(sht45->addr, 0, rx_data, 6) != 0) {
        return 0; // Error in reading data
    }

    // Parse temperature and humidity data
    uint16_t t_ticks = (rx_data[0] << 8) | rx_data[1];
    uint16_t rh_ticks = (rx_data[3] << 8) | rx_data[4];
    sht45->t = -45.0f + (175.0f * (t_ticks / 65535.0f));
    sht45->h = -6.0f + (125.0f * (rh_ticks / 65535.0f));

    if (sht45->h > 100) {
        sht45->h = 100;
    }
    if (sht45->h < 0) {
        sht45->h = 0;
    }

    return 1; // Success
}

float sht45_temperature(sht45 *sht45) {
    return sht45->t;
}

float sht45_humidity(sht45 *sht45) {
    return sht45->h;
}
