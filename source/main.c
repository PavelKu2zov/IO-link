#include "stm32f10x.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


#include "Init.h"


xSemaphoreHandle xSemaphorePL;


int main(void)
{
	Init();

// Create semaphore for PL service
	vSemaphoreCreateBinary( xSemaphorePL );	

}



