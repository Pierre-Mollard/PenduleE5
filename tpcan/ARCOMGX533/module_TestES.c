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

static RT_TASK task_acq_c1;
static RT_TASK task_acq_c2;

void test_acq_c1(int id){ //test sortie 1 et entree 1
  while(1){
  	ADRangeSelect(1, 8);
  	u16 value = ReadAD();
 
  	printk("log acq1 lu (%u)\n", value);
  	rt_task_wait_period();
  }
}

void test_acq_c2(int id){//test acquisition entree 2 p/r signal ext
  while(1){
  	ADRangeSelect(2, 8);
  	u16 value = ReadAD();

	if(value < 4096 && value > 0){
		//printk("log acq2 lu (%u)\n", value);
        	setDA_async(2, value); 
	}
 
  	rt_task_wait_period();
  }
}

static int modTest_init(void) {

  int ierr;
  RTIME now;

  //taches
  rt_set_oneshot_mode();
  ierr = rt_task_init(&task_acq_c1, test_acq_c1, 0, STACK_SIZE, PRIORITE, 0, 0);
  ierr = rt_task_init(&task_acq_c2, test_acq_c2, 0, STACK_SIZE, PRIORITE, 0, 0);

  start_rt_timer(nano2count(TICK_PERIOD));
  now = rt_get_time();
  rt_task_make_periodic(&task_acq_c1, now, nano2count(PERIODE_CONTROL));
  rt_task_make_periodic(&task_acq_c2, now, nano2count(PERIODE_CONTROL));
 
  printk("Init Module Test\n");

  setDA_async(1, 4095); //test valeur max sortie 1
 
 return(0);
}

static void modTest_exit(void) {
 stop_rt_timer(); 
 rt_task_delete(&task_acq_c1);
 rt_task_delete(&task_acq_c2);

}

module_init(modTest_init);
module_exit(modTest_exit);

