#include<linux/init.h>
#include<linux/module.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_fifos.h>
#include <rtai_sem.h>

#include"module_AR.h"

MODULE_LICENSE("GPL");

/* define pour tache periodique */
#define STACK_SIZE  2000
#define TICK_PERIOD 1000000    //  1 ms
#define PERIODE_CONTROL 20000000 //20ms
#define N_BOUCLE 10000000
#define NUMERO 1
#define PRIORITE 1

static RT_TASK task_test_acq;

float angle_MAX = 3.620;
float angle_MIN = -3.519;

float position_MAX = 8.413;
float position_MIN = -8.373;

void methode_testacq(int id){ //recup de l'acquisition
  double valueAngle, valuePos;
  u16 valueVolt0, valueVolt1;
  u16 value0, value1;
  while(1){
        
	value0 = getAngle();
	value1 = getPos();

	valueVolt0 = (value0-2048)*10/4095;
	valueVolt1 = (value1-2048)*10/4095;
	
	printk("Resultat Angle Volt (-10/+10): %u\n", valueVolt0);
	printk("Resultat Position Volt (-10/+10): %u\n", valueVolt1);

        valueAngle = valueVolt0*17.13/3.620;
        valuePos = valueVolt1*1/8.413;

	//setCmd(1024);

  	rt_task_wait_period();
  }
}

static int modTestAR_init(void) {

  int ierr;
  RTIME now;

  //taches
  rt_set_oneshot_mode();
  ierr = rt_task_init(&task_test_acq, methode_testacq, 0, STACK_SIZE, PRIORITE, 0, 0);

  start_rt_timer(nano2count(TICK_PERIOD));
  now = rt_get_time();
  rt_task_make_periodic(&task_test_acq, now, nano2count(PERIODE_CONTROL));
 
  printk("Init Module Test Acquisition Restitution (TestAR)\n");
 
 return(0);
}

static void modTestAR_exit(void) {
 stop_rt_timer(); 
 rt_task_delete(&task_test_acq);

}

module_init(modTestAR_init);
module_exit(modTestAR_exit);

