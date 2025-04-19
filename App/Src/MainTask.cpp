#include "MainTask.h"
#include "MessageTask.h"
#include "com_time.h"
#include "component.h"
#include "stm32g4xx_hal_tim.h"
#include "tim.h"

// #include "cmsis_os2.h"

cMain Main;
extern TIM_HandleTypeDef htim17;

com_thread MainTask("MainTask", 10, maintask);
com_thread MessageTask("MessageTask", 1, messagetask);

com_os OS;

void com_main(void *argument)
{
    // HAL_TIM_Base_Init(&htim17);
    HAL_TIM_Base_Start(&htim17);

    OS.Init_time(&htim17.Instance->CNT, 65535);
    OS.new_thread(MainTask);
    OS.new_thread(MessageTask);
    OS.Scheduler();
}

void maintask(void *argument) { Main.Task(); }

void cMain::Task() { OS.Delay(500); }

cMain::cMain() {}
