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
#define REG_PACER BASE+10

int sel_channel = 0;

static RT_TASK acq;

int init3718(void){
  outb(REG_CTRL, 0);
  outb(REG_PACER, 1);

}

void SetChanel(int in_channel){
  sel_channel = in_channel;
  printk("Definition channel : %d\n", in_channel);
  outb(REG_MUX, in_channel + in_channel<<4); // à verif (p27)
}

void ADRangeSelect(int channel, int range){
  SetChanel(channel);
  outb(REG_RANGE, range); //Range code (p26) à définir
  printk("Definition range : %d\n", range);
}

u16 ReadAD(void){
  printk("-Debut read\n");
  outb(BASE, 1);//trigger conversion
  int pret = inb(REG_STATUS);
  printk("-Status : %d\n", pret);
  if(pret == 48){//16 pour software (INT) 7 pour EOC
	printk("-Debut conversion\n");
	int channelLu = inb(BASE) && 15;
	printk("-Channel conversion : %u\n", channelLu);
	if(channelLu == sel_channel){ //15 : masque pour les 4 lowbyte
		printk("-Channel valide\n");
    		int lowbyte = inb(BASE)>>4;
		printk("-lowbyte : %u\n", lowbyte);
    		int highbyte = inb(REG_RANGE);
		printk("-highbyte : %u\n", highbyte);
		u16 resultat = lowbyte;
		resultat = resultat + (highbyte<<4);
    		return resultat;
  	}else{
		printk("-Channel non valide (ignore)\n");
	}
  }else{
	printk("--ECHEC pas de INT\n");
	return -1;
  }
  
}

void acq_task(int id){
  while(1){
  	ADRangeSelect(1, 8);
  	u16 value = ReadAD();
 
  	printk("Resultat : %u", value);
  	rt_task_wait_period();
  }
}


static int tpcan_init(void) {

  int ierr;
  RTIME now;
  init3718();

  //taches
  rt_set_oneshot_mode();
  ierr = rt_task_init(&acq, acq_task, 0, STACK_SIZE, PRIORITE, 0, 0);

  start_rt_timer(nano2count(TICK_PERIOD));
  now = rt_get_time();
  rt_task_make_periodic(&acq, now, nano2count(PERIODE_CONTROL));
 
  printk("Init\n");
 
 return(0);
}

static void tpcan_exit(void) {
 stop_rt_timer(); 
 rt_task_delete(&acq);

}

module_init(tpcan_init);
module_exit(tpcan_exit);

