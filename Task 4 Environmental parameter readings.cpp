#include "mbed.h"

// DS1621 Temperature Sensor Reading Function
I2C i2c(I2C_SDA, I2C_SCL); // I²C Initialization (SDA=D14/PB9, SCL=D15/PB8)
const int addr = 0x9E;       // DS1621 8-bit I²C Address (7-bit address 0x4F << 1)

int* ReadDS1621() {
    char cmd[2];
    static int data[2] = {0, 0};
    
    // Configure Sensor for Continuous Conversion Mode
    cmd[0] = 0xAC;                    // Access Configuration Register
    i2c.write(addr, cmd, 1, false);
    cmd[0] = 0x02;                    // Continuous Conversion, POL=1 (Active High)
    i2c.write(addr, cmd, 1, false);
    wait_us(20000);                   // Wait for Configuration to Complete
    
    // Start Temperature Conversion and Wait for Result
    cmd[0] = 0xEE;                    // Start Conversion Command
    i2c.write(addr, cmd, 1, false);
    wait_us(20000);                   // Wait 20ms (Datasheet Requires ≥10ms)
    
    // Read Temperature Data
    cmd[0] = 0xAA;                    // Read Temperature Command
    i2c.write(addr, cmd, 1, false);
    i2c.read(addr, cmd, 2, false);    // Read Two Bytes of Data
    data[0] = (int)cmd[0];            // High Byte (Two's Complement)
    data[1] = (int)cmd[1];            // Low Byte (0.5°C Flag)
    return data;
}

// Input devices initialization
DigitalIn blue_button(PC_13);        // Blue button (PC13)
BusIn switches(D6, D7, D8, D9);      // 4-bit switch (D6-D9)

// Sensor initialization
extern int* ReadDS1621(void);        // Temperature sensor
AnalogIn ldr(A5);                    // Light sensor (A5=PC0)
AnalogIn accel_x(A0), accel_y(A1), accel_z(A2);  // Accelerometer
AnalogIn pot(A4);                    // Potentiometer (A4=PC1)

int main() {
    while(1) {
        // Check if blue button is pressed
        if (blue_button == 0) {  // Button is active low
            int switch_val = switches.read();  // Read 4-bit switch value (0-15)

            // Display content based on switch value
            switch (switch_val) {
                case 0b0001: {  // Display temperature
                    int* temp_data = ReadDS1621();
                    int8_t high_byte = (int8_t)temp_data[0];
                    uint8_t low_byte = (uint8_t)temp_data[1];
                    float temp = high_byte + ((low_byte & 0x80) ? 0.5f : 0.0f);
                    printf("Temperature: %.1f°C\n", temp);
                    break;
                }
                case 0b0010: {  // Display light level (* count)
                    int star_count = (int)(ldr.read() * 20);  // 0-20 stars
                    printf("Light Level: ");
                    for (int i=0; i<star_count; i++) printf("*");
                    printf("\n");
                    break;
                }
                case 0b0100: {  // Display 3-axis acceleration
                    float x = (accel_x.read() - 0.5f) * 6.0f;
                    float y = (accel_y.read() - 0.5f) * 6.0f;
                    float z = (accel_z.read() - 0.5f) * 6.0f;
                    printf("Acceleration: X=%.2fg, Y=%.2fg, Z=%.2fg\n", x, y, z);
                    break;
                }
                case 0b1000: {  // Display potentiometer voltage
                    float voltage = pot.read() * 3.3f;
                    printf("Potentiometer Voltage: %.2fV\n", voltage);
                    break;
                }
                default:  // Invalid switch value
                    printf("Error: Invalid Switch Value");
            }
        }
        wait_us(100000);  // Debounce delay for 0.1s.
    }
}
