#include <Wire.h>

// Define I2C addresses and commands
#define SHT45_I2C_ADDR 0x44
#define MEASURE_CMD 0xFD

// Define SHT45 class
class ArtronShop_SHT45 {
private:
    uint8_t addr;
    float t, h;

public:
    void init(uint8_t addr) {
        this->addr = addr;
    }

    bool begin() {
        Wire.begin();
        Wire.beginTransmission(this->addr);
        return Wire.endTransmission() == 0;
    }

    bool measure() {
        Wire.beginTransmission(this->addr);
        Wire.write(MEASURE_CMD);
        if (Wire.endTransmission() != 0) {
            this->t = -99;
            this->h = -99;
            return false;
        }

        delay(10); // wait measuring

        Wire.requestFrom(this->addr, (uint8_t)6);
        if (Wire.available() != 6) {
            this->t = -99;
            this->h = -99;
            return false;
        }

        uint8_t rx_bytes[6];
        for (int i = 0; i < 6; i++) {
            rx_bytes[i] = Wire.read();
        }

        uint16_t t_ticks = ((uint16_t)rx_bytes[0] << 8) | rx_bytes[1];
        uint16_t rh_ticks = ((uint16_t)rx_bytes[3] << 8) | rx_bytes[4];
        this->t = -45.0f + (175.0f * (t_ticks / 65535.0f));
        this->h = -6.0f + (125.0f * (rh_ticks / 65535.0f));
        if (this->h > 100) {
            this->h = 100;
        }
        if (this->h < 0) {
            this->h = 0;
        }

        return true;
    }

    float temperature() {
        return this->t;
    }

    float humidity() {
        return this->h;
    }
};

ArtronShop_SHT45 sht45;

void setup() {
    Serial.begin(115200);
    sht45.init(SHT45_I2C_ADDR);

    while (!sht45.begin()) {
        Serial.println("SHT45 not found !");
        delay(1000);
    }
}

void loop() {
    if (sht45.measure()) {
        Serial.print("Temperature: ");
        Serial.print(sht45.temperature(), 1);
        Serial.print(" *C\tHumidity: ");
        Serial.print(sht45.humidity(), 1);
        Serial.print(" %RH");
        Serial.println();

        if (sht45.temperature() >= 30) {
            // Do something
        }
    } else {
        Serial.println("SHT45 read error");
    }
    delay(1000);
}
