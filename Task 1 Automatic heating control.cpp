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

// Servo Control (PWM Pin D13/PA5)
PwmOut servo(D13);

int main() {
    servo.period_ms(20);          // Set PWM Period to 20ms (Standard Servo Frequency)
    
    while(1) {
        // Read Temperature Data
        int* temp_data = ReadDS1621();
        int8_t high_byte = (int8_t)temp_data[0];
        uint8_t low_byte = (uint8_t)temp_data[1];
        
        // Parse Temperature Value
        float temperature = high_byte;
        if (low_byte & 0x80) temperature += 0.5f;
        
        // Control Logic: Turn Off Heating if Temperature ≥25°C, Else Turn On
        float pulse_width_ms = (temperature >= 25.0f) ? 1.0f : 1.75f;
        servo.pulsewidth_ms(pulse_width_ms);
        
        // Debug Output via Serial
        printf("Temperature: %.1f°C | Servo Pulse: %.2fms\n", 
              temperature, pulse_width_ms);
        
        wait_us(500000);             // Update Every 0.5 Seconds
    }
}
