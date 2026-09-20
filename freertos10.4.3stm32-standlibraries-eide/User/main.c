#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "task.h"
#include "LED.h"
#define TASK1_PRIORITY    2       // 任务1优先级（数值越大优先级越高）
#define TASK2_PRIORITY    2       // 任务2优先级
void vTask1(void *pvParameters);  // 任务1：示例任务（低优先级）
void vTask2(void *pvParameters);  // 任务2：示例任务（高优先级）
void Hardware_Init(void)
{
	LED_Init();
	GPIO_WriteBit(GPIOA,GPIO_Pin_0,(BitAction)1);
	GPIO_WriteBit(GPIOA,GPIO_Pin_1,(BitAction)1);
}
int main(void)
{
    Hardware_Init();
    // 创建任务
    BaseType_t xReturn1, xReturn2;
    xReturn1 = xTaskCreate(vTask1, "Task1", 128, NULL, TASK1_PRIORITY, NULL);
    xReturn2 = xTaskCreate(vTask2, "Task2", 128, NULL, TASK2_PRIORITY, NULL);
    // 检查任务创建结果
    if (xReturn1 != pdPASS || xReturn2 != pdPASS)
{
    // 任务创建失败，可通过LED闪烁提示（需单独配置）
    while (1)
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_0, (BitAction)!GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0));
        for(int i=0;i<1000000;i++); // 简单延时
    }
}
    // 启动FreeRTOS调度器（启动后任务开始运行）
    vTaskStartScheduler();
    // 若调度器启动成功，程序不会执行到这里
    // 若执行到这里，说明调度器启动失败（通常是内存不足）
    while (1);
}
void vTask1(void *pvParameters)
{
    while (1)
    {
		GPIO_WriteBit(GPIOA,GPIO_Pin_0,(BitAction)1);
        vTaskDelay(pdMS_TO_TICKS(300));  // 延时300ms（毫秒转FreeRTOS ticks）
		GPIO_WriteBit(GPIOA,GPIO_Pin_0,(BitAction)0);
        vTaskDelay(pdMS_TO_TICKS(300));  // 延时300ms（毫秒转FreeRTOS ticks）
    }
}
void vTask2(void *pvParameters)
{
    while (1)
    {
		GPIO_WriteBit(GPIOA,GPIO_Pin_1,(BitAction)1);
        vTaskDelay(pdMS_TO_TICKS(500));  // 延时300ms
		GPIO_WriteBit(GPIOA,GPIO_Pin_1,(BitAction)0);
        vTaskDelay(pdMS_TO_TICKS(500));  // 延时300ms
    }
}
