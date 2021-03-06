/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     system.c
 * @brief    CSI Device System Source File
 * @version  V1.0
 * @date     02. Oct 2019
 ******************************************************************************/

#include <csi_config.h>
#include <soc.h>
#include <csi_core.h>
#include <drv_irq.h>

#ifndef CONFIG_SYSTICK_HZ
#define CONFIG_SYSTICK_HZ 100
#endif

int g_system_clock = IHS_VALUE;
extern int32_t g_top_irqstack;
extern void irq_vectors_init(void);
extern void mm_heap_initialize(void);

#ifdef CONFIG_KERNEL_NONE
static void _system_init_for_baremetal(void)
{
    __enable_excp_irq();

    csi_coret_config(drv_get_sys_freq() / CONFIG_SYSTICK_HZ, CORET_IRQn);    //10ms

    mm_heap_initialize();
}
#endif

#ifndef CONFIG_KERNEL_NONE
static void _system_init_for_kernel(void)
{
    irq_vectors_init();

    csi_coret_config(drv_get_sys_freq() / CONFIG_SYSTICK_HZ, CORET_IRQn);    //10ms
    drv_irq_enable(CORET_IRQn);

#ifndef CONFIG_KERNEL_RHINO
#ifndef CONFIG_NUTTXMM_NONE
    mm_heap_initialize();
#endif
#endif
}
#endif

/**
  * @brief  initialize the system
  *         Initialize the psr and vbr.
  * @param  None
  * @return None
  */
void SystemInit(void)
{
    int i;

    CLIC->CLICCFG = 0x4UL;

    for (i = 0; i < 12; i++) {
        CLIC->INTIP[i] = 0;
    }

    drv_irq_enable(Machine_Software_IRQn);

#ifdef CONFIG_KERNEL_NONE
    _system_init_for_baremetal();
#else
    _system_init_for_kernel();
#endif
}
