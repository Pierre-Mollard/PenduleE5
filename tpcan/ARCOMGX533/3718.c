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

#define BASE 0x320
#define REG_CTRL BASE+9
#define REG_STATUS BASE+8
#define REG_RANGE BASE+1
#define REG_MUX BASE+2
int sel_channel = 0;

int init3718(void){
  outb(REG_CTRL, 0);

}

void SetChanel(int in_channel){
  sel_channel = in_channel;
  outb(REG_MUX, in_channel + in_channel<<4); // à verif (p27)
}

void ADRangeSelect(int channel, int range){
  SetChanel(channel);
  outb(REG_RANGE, range); //Range code (p26) à définir
}

u16 ReadAD(void){
  outb(BASE, 1);
  if(inb(REG_STATUS)>>7 == 0){
	if((inb(BASE) && 15) == sel_channel){
    		int lowbyte = inb(BASE)>>4;
    		int highbyte = inb(REG_RANGE);
    		return lowbyte + highbyte<<4;
  	}
  }else{
	return 0;
  }
  
}

static int tpcan_init(void) {

  int ierr;
  RTIME now;

  start_rt_timer(nano2count(TICK_PERIOD));
  now = rt_get_time();
 
  init3718();
  ADRangeSelect(0, 0);
  u8 value = ReadAD();
 
  printk("test lo glhf : %d", value);
 
 return(0);
}

static void tpcan_exit(void) {
 stop_rt_timer(); 

}

module_init(tpcan_init);
module_exit(tpcan_exit);

