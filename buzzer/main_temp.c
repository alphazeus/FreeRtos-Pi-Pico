#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define BUZZER_PIN 15
#define CLK_DIV 64.0f

// Calculate the PWM wrap value for a given frequency
void play_tone(uint pin, uint frequency) {
    // Get the slice number of the pin
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Set wrap based on the clock
    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t wrap = (clock_freq / ((uint32_t)CLK_DIV * frequency)) - 1;

    // Set the frequency and 50% duty cycle
    pwm_set_wrap(slice_num, wrap);
    pwm_set_gpio_level(pin, wrap / 2);

}

void stop_tone(uint pin) {
    // Turn off output by setting level to 0
    pwm_set_gpio_level(pin, 0);
}

int main() {
    stdio_init_all();

    // Tell the GPIO pin that it is allocated to the PWM
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    
    // Set up PWM divider (no division)
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, CLK_DIV);
    pwm_init(slice_num, &config, true);

    // Frequencies for musical notes (C4, D4, E4, F4, G4, A4, B4, C5)
    uint notes_four[] = {261, 293, 329, 349, 392, 440, 494, 523};
    uint notes_five[] = {523, 587, 659, 698, 783, 880, 987, 1046};
    uint notes_six[] = {1046, 1174, 1318, 1396, 1567, 1760, 1975, 2093};
    uint notes[] = {2093, 2349, 2637, 2793, 3135, 3520, 3951, 4186};

    while (true) {
        // Play each note
        for (int i = 0; i < 8; i++) {
            play_tone(BUZZER_PIN, notes[i]);
            sleep_ms(500); // Note duration
            stop_tone(BUZZER_PIN);
            sleep_ms(50); // Gap between notes
        }
        for (int i = 7; i >= 0; i--) {
            play_tone(BUZZER_PIN, notes[i]);
            sleep_ms(500); // Note duration
            stop_tone(BUZZER_PIN);
            sleep_ms(50); // Gap between notes
        }
        sleep_ms(2000); // Wait between scales
    }
}
