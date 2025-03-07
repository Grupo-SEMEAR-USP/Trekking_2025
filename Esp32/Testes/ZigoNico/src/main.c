#include "types.h"
#include "task_core0.h"
#include "task_core1.h"
#include "sycronization.h"
#include "global_variables.h"

    //declaring locks
    SemaphoreHandle_t xSemaphore_getSpeed;
    SemaphoreHandle_t xSemaphore_getRosSpeed;
    EventGroupHandle_t initialization_groupEvent;
    TaskHandle_t Core0_Task_Handle;

    const int task0_init_done = 0b01;
    const int task1_init_done = 0b10;

    //declaring and initializing global variables
    float global_ros_angular_speed_left = 0;
    float global_ros_angular_speed_right = 0;

    float global_motor_angular_speed_left = 0 ;
    float global_motor_angular_speed_right = 0;

void app_main() {
    //initializing locks
    xSemaphore_getSpeed = xSemaphoreCreateMutex();
    xSemaphore_getRosSpeed = xSemaphoreCreateMutex();
    initialization_groupEvent = xEventGroupCreate(); //it's perhaps not necessary

    //Inicializar as tasks
    xTaskCreatePinnedToCore(&core0fuctions, "task que inicializa pwm,encoders e pid no core 0", 2048, NULL, 1, &Core0_Task_Handle, 0);
    //xTaskCreatePinnedToCore(&core1functions, "task que inicializa o i2c no core 1", 2048, NULL, 1, NULL, 1);
}

