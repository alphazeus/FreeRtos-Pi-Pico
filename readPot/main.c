#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

#define POT_PIN 26
#define LED_PIN 23

void vReadPotTask(void *pvParameters) {
    adc_init();
    adc_gpio_init(POT_PIN);
    adc_select_input(0);

    gpio_set_function(LED_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(LED_PIN);
    pwm_set_wrap(slice_num, 255);
    pwm_set_enabled(slice_num, true);

    while (1) {
        // Read the potentiometer value
        uint16_t pot_value = adc_read();
        uint16_t duty_cycle = 255 - ((pot_value * 255) / 4095);

        pwm_set_gpio_level(LED_PIN, duty_cycle);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

int main() {
    stdio_init_all();

    xTaskCreate(vReadPotTask, "Read_Pot_Task", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1) {
    }
}

