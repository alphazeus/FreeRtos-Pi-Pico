#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"

#define POT_PIN 26
#define BUZZER_PIN 15
#define CLK_DIV 64.0f

uint8_t gVolume = 10; // Global variable to hold the volume level (0-100)

// Calculate the PWM wrap value for a given frequency
void play_tone(uint8_t pin, uint16_t frequency, uint16_t duration_ms, uint8_t volume) {
    // Get the slice number of the pin
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Set wrap based on the clock
    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t wrap = (clock_freq / ((uint32_t)CLK_DIV * frequency)) - 1;

    // Set the frequency and 50% duty cycle
    pwm_set_wrap(slice_num, wrap);
    pwm_set_gpio_level(pin, (wrap / 2) * volume / 100);
    vTaskDelay(pdMS_TO_TICKS(duration_ms));

    pwm_set_gpio_level(pin, 0);
    vTaskDelay(pdMS_TO_TICKS(50));

}

void vPlaySongTask() {
    
    // Frequencies for musical notes (C7, D7, E7, F7, G7, A7, B7, C8)
    uint16_t note_frequencies[] = {2093, 2349, 2637, 2793, 3135, 3520, 3951, 4186};

    // Define the sequence of notes and their durations for "Twinkle Twinkle Little Star"
    uint8_t ttls[] = {0, 0, 4, 4, 5, 5, 4, 3, 3, 2, 2, 1, 1, 0, 
                        4, 4, 3, 3, 2, 2, 1, 4, 4, 3, 3, 2, 2, 1,
                        0, 0, 4, 4, 5, 5, 4, 3, 3, 2, 2, 1, 1, 0};
    uint16_t ttls_durations[] = {500, 500, 500, 500, 500, 500, 1000, 500, 500, 500, 500, 500, 500, 1000,
                                   500, 500, 500, 500, 500, 500, 1000, 500, 500, 500, 500, 500, 500, 1000,
                                   500, 500, 500, 500, 500, 500, 1000, 500, 500, 500, 500, 500, 500, 1000};
    
    while (true) {
        for (int i = 0; i < sizeof(ttls)/sizeof(ttls[0]); i++) {
            uint16_t note_index = ttls[i];
            uint16_t duration = ttls_durations[i];
            play_tone(BUZZER_PIN, note_frequencies[note_index], duration, gVolume);
        }
        // Play each note
        // for (size_t i = 0; i < sizeof(note_frequencies) / sizeof(note_frequencies[0]); i++) {
        //     play_tone(BUZZER_PIN, note_frequencies[i], 500, volume);
        // }
    }
}

void vReadVolumeTask(void *pvParameters) {
    adc_init();
    adc_gpio_init(POT_PIN);
    adc_select_input(0);

    while (1) {
        // Read the potentiometer value
        uint16_t pot_value = adc_read();
        uint16_t duty_cycle = 100 - ((pot_value * 100) / 4095);

        gVolume = duty_cycle;

        vTaskDelay(pdMS_TO_TICKS(100));
    }
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


    stdio_init_all();

    xTaskCreate(vPlaySongTask, "Play_Song_Task", 256, NULL, 1, NULL);
    xTaskCreate(vReadVolumeTask, "Read_Volume_Task", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1) {
    }

}
