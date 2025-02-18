#include "mbed.h"

// Accelerometer pins (X=A0=PA0, Y=A1=PA1, Z=A2=PA4)
AnalogIn accel_x(A0), accel_y(A1), accel_z(A2);
// RGB LED pins (Red=D2=PA10, Blue=D3=PB3, Green=D4=PB5)
DigitalOut red(D2), blue(D3), green(D4);

// Convert accelerometer reading to g value，ADXL335 formula：（2.2.2）
float read_accel_g(AnalogIn &axis) {
    return (axis.read() - 0.5f) * 6.0f;  // (0-1) → (-3g ~ +3g)
}

int main() {
    while(1) {
        // Read acceleration in g
        float x = read_accel_g(accel_x);
        float y = read_accel_g(accel_y);
        float z = read_accel_g(accel_z);

        // Check for fall (if any axis exceeds threshold)
        bool is_fall = (x > 1.8f || x < -0.8f || 
                        y > 1.8f || y < -0.8f || 
                        z > 1.8f || z < -0.8f);

        // Trigger alarm: Cycle through Red→Blue→Green, each for 0.2 seconds
        if (is_fall) {
            red = 1; blue = 0; green = 0;  // Red
            wait_us(200000);
            red = 0; blue = 1; green = 0;  // Blue
            wait_us(200000);
            red = 0; blue = 0; green = 1;  // Green
            wait_us(200000);
            green = 0;  // Turn off
        } else {
            red = blue = green = 0;  // Normal state: LEDs off
        }

        // Debug output (serial monitor)
        printf("Acceleration: X=%.2fg, Y=%.2fg, Z=%.2fg | Alarm: %s\n", 
              x, y, z, is_fall ? "TRIGGERED" : "NONE");
    }
}
