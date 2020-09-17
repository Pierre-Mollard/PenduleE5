#include<linux/init.h>
#include<linux/module.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_fifos.h>

#include"3712.h"

MODULE_LICENSE("GPL");

/* define pour tache periodique */
#define STACK_SIZE  2000
#define TICK_PERIOD 1000000    //  1 ms
#define PERIODE_CONTROL 20000000 //20ms
#define N_BOUCLE 10000000
#define NUMERO 1
#define PRIORITE 1

#define BASE_3712 0x300
#define PCM3712_DA0 BASE_3712
#define PCM3712_DA1 (BASE_3712 +2)
#define PCM3712_SYNC (BASE_3712 +4)
#define PCM3712_OUT (BASE_3712 +5)

void setDA_async(int channel, int value);
