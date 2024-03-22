#include "mbed.h"
#include "stdint.h"

#define LM75_REG_TEMP (0x00) // Temperature Register
#define LM75_REG_CONF (0x01) // Configuration Register
#define LM75_ADDR     (0x90) // LM75 address

#define LM75_REG_TOS (0x03) // TOS Register
#define LM75_REG_THYST (0x02) // THYST Register

I2C i2c(I2C_SDA, I2C_SCL);
DigitalOut myled(LED1);
DigitalOut blue(LED2);
InterruptIn lm75_int(D7); // Make sure you have the OS line connected to D7
Serial pc(SERIAL_TX, SERIAL_RX);
int16_t i16; // This variable needs to be 16 bits wide for the TOS and THYST conversion to work - can you see why?
const float record_interval = 1.0;
Timeout record_temperature_ticker;
const int N = 60;
float temperature_reading[N];
int t = 0;
int j;
bool exceed = false;

void blue_flip() {
    blue = !blue;
    exceed = true;
}

void record_temperature(float temperature) {
    // Display result
    myled = !myled;
    if (t < N) {
        // If there's still room in the array, just add the temperature to the next available slot
        temperature_reading[t] = temperature;
        t++;
    } else {
        // If the array is full, remove the first element and shift all elements to the left by one position
        for (int i = 0; i < N - 1; i++) {
            temperature_reading[i] = temperature_reading[i + 1];
        }
        // Add the new temperature to the last slot
        temperature_reading[N - 1] = temperature;
    }
}

int main() {
    while (exceed == false){
    char data_write[3];
    char data_read[3];
    data_write[0] = LM75_REG_CONF;
    data_write[1] = 0x02;
    int status = i2c.write(LM75_ADDR, data_write, 2, 0);
    if (status != 0) { // Error
        while (1) {
            myled = !myled;
            wait(0.2);
        }
    }

    float tos = 28; // TOS temperature
    float thyst = 26; // THYST tempertuare

    // This section of code sets the TOS register
    data_write[0] = LM75_REG_TOS;
    i16 = (int16_t)(tos * 256) & 0xFF80;
    data_write[1] = (i16 >> 8) & 0xff;
    data_write[2] = i16 & 0xff;
    i2c.write(LM75_ADDR, data_write, 3, 0);

    // This section of codes set the THYST register
    data_write[0] = LM75_REG_THYST;
    i16 = (int16_t)(thyst * 256) & 0xFF80;
    data_write[1] = (i16 >> 8) & 0xff;
    data_write[2] = i16 & 0xff;
    i2c.write(LM75_ADDR, data_write, 3, 0);

    // This line attaches the interrupt.
    // The interrupt line is active low so we trigger on a falling edge
    lm75_int.fall(&blue_flip);

    // Read temperature register
    data_write[0] = LM75_REG_TEMP;
    i2c.write(LM75_ADDR, data_write, 1, 1); // no stop
    i2c.read(LM75_ADDR, data_read, 2, 0);

    // Calculate temperature value in Celcius
    int16_t i16 = (data_read[0] << 8) | data_read[1];
    // Read data as twos complement integer so sign is correct
    float temp = i16 / 256.0;
    pc.printf("Temperature = %.3f\r\n", temp);
    record_temperature_ticker.attach([&temp](){ record_temperature(temp); }, record_interval);   
    wait(1);
    }

    for (j = 0; j < N; j++) {
        pc.printf("Temperature3 = %.3f\r\n", temperature_reading[j]);
    }
    while (1) {
        // Do nothing until it reboots
    }

}
