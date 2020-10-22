#include<linux/init.h>
#include<linux/module.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_fifos.h>
#include <rtai_sem.h>

#include"module_Entree.h"
#include"module_Sortie.h"

MODULE_LICENSE("GPL");

/* define pour tache periodique */
#define STACK_SIZE  2000
#define TICK_PERIOD 1000000    //  1 ms
#define PERIODE_CONTROL 20000000 //20ms
#define N_BOUCLE 10000000
#define NUMERO 1
#define PRIORITE 1

#define FIFO 1

static RT_TASK task_acq;
u16 value0, value1;

void methode_acq(int id){ //acquisition
  while(1){
        printk("-------------\n");

        //angle sur channel 0
  	SetChanel(0);
  	value0 = ReadAD();
	//printk("Resultat Angle (0/4096) : %u\n", value0);

	//FIFO
	char * tampon;
	tampon = value0;
	rtf_put(FIFO, tampon, sizeof(char));

	//wait 
	rt_busy_sleep(nano2count(TICK_PERIOD*5));

	//position sur channel 1
  	SetChanel(1);
  	value1 = ReadAD();
	//printk("Resultat Position (0/4096) : %u\n", value1);
	

  	rt_task_wait_period();
  }
}

u16 getAngle(u16 * ptr_angle){
  *ptr_angle = value0;
  return *ptr_angle;
}

u16 getPos(u16 * ptr_pos){
  *ptr_pos = value1;
  return *ptr_pos;
}

void setCmd(u16 value){
  setDA_async(0, value); 
}

static int modAR_init(void) {

  int ierr;
  RTIME now;

  rtf_create(FIFO, 2000);

  //taches
  rt_set_oneshot_mode();
  ierr = rt_task_init(&task_acq, methode_acq, 0, STACK_SIZE, PRIORITE, 1, 0);

  start_rt_timer(nano2count(TICK_PERIOD));
  now = rt_get_time();
  rt_task_make_periodic(&task_acq, now, nano2count(PERIODE_CONTROL/2));
 
  printk("Init Module Acquisition Restitution (AR)\n");
  ADRangeSelect(0, 8);
  ADRangeSelect(1, 8);
 
 return(0);
}

static void modAR_exit(void) {
 stop_rt_timer(); 
 rtf_destroy(FIFO);
 rt_task_delete(&task_acq);

}

module_init(modAR_init);
module_exit(modAR_exit);

EXPORT_SYMBOL(getAngle);
EXPORT_SYMBOL(getPos);
EXPORT_SYMBOL(setCmd);
