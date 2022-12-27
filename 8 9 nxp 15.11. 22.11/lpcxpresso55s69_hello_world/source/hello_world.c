/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

#include "fsl_power.h"
#include "peripherals.h"
#include "fsl_powerquad.h"
#include "math.h"
#include <string.h>


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define PI 3.14
#define TABLE_LENGHT 100
//#define USE_POWERQUAD 1


char ch;
float Sample_f;
uint32_t index=0;
float SinTable_f[TABLE_LENGHT];




/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */

void TimerIRQ(uint32_t flags)
{
	Sample_f=SinTable_f[index];
	index++;
	if(index==TABLE_LENGHT)index=0;


}

void Generate_sin_table_float(float *table, uint32_t length,float amplitude,
		float frequency)
{
	uint32_t index;
	float theta, sample_time;
	sample_time=1.0f/(frequency * (float)length);
	for(index=0;index < length;index++)
	{
		theta= 2.0f * PI * frequency * sample_time * (float)index;
		table[index]=amplitude * sin(theta);
		#if USE_POWERQUAD
		PQ_SinF32(&theta, &result);
		table[index]=amplitude * result;
		else
		table[index]=amplitude * sin(theta);
		#endif
	}
}




int main(void)
{


		/* Init board hardware. */
		/* set BOD VBAT level to 1.65V */
		POWER_SetBodVbatLevel(kPOWER_BodVbatLevel1650mv, kPOWER_BodHystLevel50mv, false);
		/* attach main clock divide to FLEXCOMM0 (debug console) */
		CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

		BOARD_InitBootPins();
		BOARD_InitBootClocks();
		BOARD_InitDebugConsole();
		BOARD_InitBootPeripherals();
		PQ_Init(POWERQUAD);
		BOARD_InitPeripherals();
#if !defined(DONT_ENABLE_FLASH_PREFETCH)
		/* enable flash prefetch for better performance */
		SYSCON->FMCCR |= SYSCON_FMCCR_PREFEN_MASK;
#endif

		PRINTF("\r\nhello world.\r\n");

		CTIMER_StartTimer(CTIMER0_PERIPHERAL);
		Generate_sin_table_float(&SinTable_f[0],TABLE_LENGHT,1.0f, 1.0f);

		char pass_stored[20] = "1234";

		while (1)
		{
			int32_t DWT1, DWT2;
			char input[20];
			PRINTF("\r\nEnter pass:");
			//ch = GETCHAR();
			//PUTCHAR(ch);
			SCANF("%s", input);

			//meranie casu
			DWT1 = DWT -> CYCCNT;


			//uint32_t status = strcmp(input, pass_stored);

			uint8_t ok = 0;
			uint8_t nok = 0;
			for(uint8_t i = 0 ; i < strlen(input); i++ ) {
				if (input[i] == pass_stored[i]) {
					ok++;
				}
				else {
					ok--;
				}
			}

			uint8_t status;
			if (ok == strlen(pass_stored)) {
					status = 0;

			}
			else if (ok == strlen(input)) {
					status = 1;
				}
			else status = 1;





			DWT2 = DWT -> CYCCNT;
			//koniec merania casu




			if (status == 0) {
				PRINTF("\r\n valid ");
			}
			else {
				PRINTF("\r\nINVALID ");
			}
			PRINTF("Cycles in function: %d", DWT2-DWT1);


		}
}

