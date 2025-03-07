#include "task_core1.h"
#include "initializers.h"
#include "sycronization.h"
#include "global_variables.h"
#include "pwm.h"
#include "pid.h"

void pwm_iterate_task(void *params){

    float local_motor_angular_speed_left = 0; 
    float local_motor_angular_speed_right = 0;

    char permissao = 's';
    uint32_t pwm_duty_increment = 0;

    int messure_count = 0;

    float velocities[(MAX_DUTY/DUTY_INCREMENT_STEP) +1][2];

    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    ESP_ERROR_CHECK(uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0));
    esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);
    esp_vfs_dev_uart_port_set_rx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CR);
    esp_vfs_dev_uart_port_set_tx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CRLF);
    xEventGroupWaitBits(initialization_groupEvent, task0_init_done, true, true, portMAX_DELAY);
    while(pwm_duty_increment <= MAX_DUTY){
        printf("prosseguir?: ");
        scanf("%c\n", &permissao);
        

        if (permissao == 's'){

            xSemaphoreTake(xSemaphore_getSpeed,portMAX_DELAY);
            local_motor_angular_speed_left = global_motor_angular_speed_left;
            local_motor_angular_speed_right = global_motor_angular_speed_right;
            xSemaphoreGive(xSemaphore_getSpeed);

            printf("duty atual: %d\n", pwm_duty_increment);

            ledc_set_duty(LEDC_LOW_SPEED_MODE, channel_choose(ESQ), (pwm_duty_increment));
            ledc_update_duty(LEDC_LOW_SPEED_MODE, channel_choose(ESQ));
            ledc_set_duty(LEDC_LOW_SPEED_MODE, channel_choose(DIR), (pwm_duty_increment));
            ledc_update_duty(LEDC_LOW_SPEED_MODE, channel_choose(DIR));

            vTaskDelay(pdMS_TO_TICKS(1000));

            printf("velocidade atual esquerda: %f\n", local_motor_angular_speed_left);
            printf("velocidade atual direita: %f\n", local_motor_angular_speed_right);
            
            velocities[messure_count][0] = local_motor_angular_speed_left;
            velocities[messure_count][1] = local_motor_angular_speed_right;

            //printf("velocidade atual esquerda: %f\n", velocities[messure_count][0]);
            //printf("velocidade atual direita: %d\n", messure_count);

            messure_count++;
            pwm_duty_increment += DUTY_INCREMENT_STEP;
        }
        printf("\n");
       
    }

    
    for(int i = 0; i <= (MAX_DUTY/DUTY_INCREMENT_STEP); i++){
        printf("%f %f\n",velocities[i][0],velocities[i][1]);
    }

    vTaskDelete(incremento_duty);
}