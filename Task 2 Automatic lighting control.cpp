#include "mbed.h"

// LDR sensor pin (A5 = PC0)
AnalogIn LDR(A5);
// 4 LED control pins (D10-D13 = PB6, PA7, PA6, PA5)
BusOut LEDs(D10, D11, D12, D13);

int main() {
    while(1) {
        float brightness = 1.0f - LDR.read();  // Brightness is high when dark, low when bright
        int level = (int)(brightness * 4);      // Map to 5 levels (0-4, corresponding to 0-4 LEDs)

        // Control number of LEDs lit (level=0: all off, level=4: all on)
        LEDs = (1 << level) - 1;  /* Generate binary mask for LED combination 
        e.g., level=2 → 0b0011 (D10, D11 on) */

        // Debug output (serial monitor)
        printf("Light Level: %.2f | LED Level: %d/4\n", brightness, level);

        wait_us(200000);  // Update every 0.2 seconds
    }
}
