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

int init3712(void){
  outb(128, PCM3712_OUT);

}

void SetDA(int channel, int value){
  if(channel == 0){
    PCM3712setda0(value);
  }
  else{
    PCM3712setda1(value);
  }
}

void acq_task(int id){
  while(1){
    SetDA(0,156)
    rt_task_wait_period();
  }
}


static int init_module(void) {

  int ierr;
  RTIME now;
  init3712();

  //taches
  rt_set_oneshot_mode();
  ierr = rt_task_init(&acq, acq_task, 0, STACK_SIZE, PRIORITE, 0, 0);

  start_rt_timer(nano2count(TICK_PERIOD));
  now = rt_get_time();
  rt_task_make_periodic(&acq, now, nano2count(PERIODE_CONTROL));
 
  printk("Init\n");
 
 return(0);
}

static void exit_module(void) {
 stop_rt_timer(); 
 rt_task_delete(&acq);

}

module_init(init_module);
module_exit(exit_module);
