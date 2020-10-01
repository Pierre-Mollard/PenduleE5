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
#define PERIODE_CONTROL_10 10000000 //10ms
#define N_BOUCLE 10000000
#define NUMERO 1
#define PRIORITE 1

SEM sem1; 
SEM sem2; 

static RT_TASK task_acq_c1;

void test_acq_c1(int id){//test acquisition entree 2 p/r signal ext
  while(1){
  	ADRangeSelect(2, 8);
  	u16 value1 = ReadAD();
	
	setDA_async(2, value1); 

	ADRangeSelect(1, 8);
  	u16 value2 = ReadAD();
	
	setDA_async(1, value2); 
 
  	rt_task_wait_period();
  }
}


static int modTest_init(void) {

  int ierr;
  RTIME now;

  //taches
  rt_set_oneshot_mode();
  ierr = rt_task_init(&task_acq_c1, test_acq_c1, 0, STACK_SIZE, PRIORITE, 0, 0);

  rt_typed_sem_init(&sem1, 1, BIN_SEM);
  rt_typed_sem_init(&sem2, 0, BIN_SEM);

  start_rt_timer(nano2count(TICK_PERIOD));
  now = rt_get_time();
  rt_task_make_periodic(&task_acq_c1, now, nano2count(PERIODE_CONTROL));
 
  printk("Init Module Test\n");

  setDA_async(1, 4095); //test valeur max sortie 1
 
 return(0);
}

static void modTest_exit(void) {
 stop_rt_timer(); 
 rt_sem_delete(&sem1);
 rt_sem_delete(&sem2);
 rt_task_delete(&task_acq_c1);

}

module_init(modTest_init);
module_exit(modTest_exit);

