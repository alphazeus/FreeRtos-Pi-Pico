#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hardware/pwm.h"

#define RED_LED 24
#define GREEN_LED 23

void vPwmGreenLedTask(void *pvParameters) {
    // 1. Configure the GPIO pin for PWM
    gpio_set_function(GREEN_LED, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(GREEN_LED);

    // 2. Set PWM wrap and clock dividers
    pwm_set_wrap(slice_num, 255); // 8-bit resolution
    pwm_set_enabled(slice_num, true);

    uint brightness = 0;
    int fade_amount = 5;

    while (1) {
        // Set brightness
        pwm_set_gpio_level(GREEN_LED, brightness);

        // Adjust brightness for next iteration
        brightness += fade_amount;
        if (brightness <= 0 || brightness >= 255) {
            fade_amount = -fade_amount; // Reverse fade direction
        }

        // Yield to other tasks for 10ms
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void vPwmRedLedTask(void *pvParameters) {
    // 1. Configure the GPIO pin for PWM
    gpio_set_function(RED_LED, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(RED_LED);

    // 2. Set PWM wrap and clock dividers
    pwm_set_wrap(slice_num, 255); // 8-bit resolution
    pwm_set_enabled(slice_num, true);

    uint brightness = 0;
    int fade_amount = 2;

    while (1) {
        // Set brightness
        pwm_set_gpio_level(RED_LED, brightness);

        // Adjust brightness for next iteration
        brightness += fade_amount;
        if (brightness <= 0 || brightness >= 255) {
            fade_amount = -fade_amount; // Reverse fade direction
        }

        // Yield to other tasks for 10ms
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

int main() {
    stdio_init_all();

    // Create your PWM task here
    xTaskCreate(vPwmRedLedTask, "PWM_Red_Task", 256, NULL, 1, NULL);
    xTaskCreate(vPwmGreenLedTask, "PWM_Green_Task", 256, NULL, 1, NULL);

    // Start the scheduler (this loop never returns)
    vTaskStartScheduler();

    while(1); // Should never get here
}

