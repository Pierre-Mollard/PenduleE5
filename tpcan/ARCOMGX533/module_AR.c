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

static RT_TASK task_acq_angle;
static RT_TASK task_acq_position;

float angle_MAX = 3.620;
float angle_MIN = -3.519;

float position_MAX = 8.413;
float position_MIN = -8.373;

void methode_acq_angle(int id){ //angle sur channel 1
  while(1){
  	ADRangeSelect(1, 8);
  	u16 value = ReadAD();
	float valueVolt = (value-2048)*10/4095;
        float valueAngle = valueVolt*22/3.620;
 
  	printk("angle (bit) : %u\n", value);
  	printk("angle (volt) : %u\n", valueVolt);
  	printk("angle (°) : %u\n", valueAngle);
  	rt_task_wait_period();
  }
}

void methode_acq_position(int id){//position sur channel 2
  while(1){
  	ADRangeSelect(2, 8);
  	u16 value = ReadAD();
	float valueVolt = (value-2048)*10/4095;
        float valuePos = valueVolt*22/3.620;
 
  	printk("position (bit) : %u\n", value);
  	printk("position (volt) : %u\n", valueVolt);
  	printk("position (°) : %u\n", valuePos);
  	rt_task_wait_period();
  }
}

static int modAR_init(void) {

  int ierr;
  RTIME now;

  //taches
  rt_set_oneshot_mode();
  ierr = rt_task_init(&task_acq_angle, methode_acq_angle, 0, STACK_SIZE, PRIORITE, 0, 0);
  ierr = rt_task_init(&task_acq_position, methode_acq_position, 0, STACK_SIZE, PRIORITE, 0, 0);

  start_rt_timer(nano2count(TICK_PERIOD));
  now = rt_get_time();
  rt_task_make_periodic(&task_acq_angle, now, nano2count(PERIODE_CONTROL));
  rt_task_make_periodic(&task_acq_position, now, nano2count(PERIODE_CONTROL));
 
  printk("Init Module Acquisition Restitution (AR)\n");
 
 return(0);
}

static void modAR_exit(void) {
 stop_rt_timer(); 
 rt_task_delete(&task_acq_angle);
 rt_task_delete(&task_acq_position);

}

module_init(modAR_init);
module_exit(modAR_exit);

