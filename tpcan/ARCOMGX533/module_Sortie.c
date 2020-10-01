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

static RT_TASK acq;

void init3712(void){
  outb(128, PCM3712_OUT);

}

void setDA_async(int channel, int value){
  if(channel == 0){
    PCM3712setda0(value);
  }
  else{
    PCM3712setda1(value);
  }
  outb(1,PCM3712_SYNC);
}

static int tpcan_init(void) {

  int ierr;
  RTIME now;
  init3712();

  //taches
  rt_set_oneshot_mode();

  start_rt_timer(nano2count(TICK_PERIOD));
  now = rt_get_time();
 
  printk("Init Module Sortie\n");
 
 return(0);
}

static void tpcan_exit(void) {
 stop_rt_timer();

}

module_init(tpcan_init);
module_exit(tpcan_exit);

EXPORT_SYMBOL(setDA_async);
