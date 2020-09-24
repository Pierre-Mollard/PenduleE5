#include<linux/init.h>
#include<linux/module.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_fifos.h>
#include <rtai_sem.h>

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

#define IT_ACQ 6

int sel_channel = 0;
u16 val_ch1 = 0;
u16 val_ch2 = 0;
SEM sem1;

int init3718(void){
  outb(0xE0, REG_CTRL); //activation des interruptions avec Interrupt Level : 
  outb(1, REG_PACER);

}

void SetChanel(int in_channel){
  sel_channel = in_channel;
  //printk("Definition channel : %d\n", in_channel);
  outb(in_channel + in_channel<<4, REG_MUX); // à verif (p27)
}

void ADRangeSelect(int channel, int range){
  SetChanel(channel);
  outb(range, REG_RANGE); //Range code (p26) à définir
  //printk("Definition range : %d\n", range);
}

u16 ReadAD(void){
  printk("-Ch%u Debut read\n", sel_channel);
  outb(1, BASE);//trigger conversion
  
  
  rt_sem_wait(&sem1);
  
  if(sel_channel == 1){
	return val_ch1;
  }else{
	return val_ch2;
 }
  
}

void gestionnaire_it(void){
  rt_pend_global_irq(IT_ACQ);

  int channelLu = inb(BASE) && 15;

  int lowbyte = inb(BASE)>>4;
  int highbyte = inb(REG_RANGE);
  u16 resultat = lowbyte;
  resultat = resultat + (highbyte<<4);

  if(channelLu == 1){
	val_ch1 = resultat;
  }else{
	val_ch2 = resultat;
  }

  rt_sem_signal(&sem1);
}

static int tpcan_init(void) {

  int ierr;
  RTIME now;
  init3718();

  //taches
  rt_set_oneshot_mode();
  
  rt_typed_sem_init(&sem1, 0, BIN_SEM);

  start_rt_timer(nano2count(TICK_PERIOD));
  now = rt_get_time();

  rt_request_global_irq(IT_ACQ, gestionnaire_it);
  rt_free_global_irq(IT_ACQ);

  printk("Init Module Entree\n");
 
 return(0);
}

static void tpcan_exit(void) {
 stop_rt_timer(); 
 rt_sem_delete(&sem1);
}

module_init(tpcan_init);
module_exit(tpcan_exit);

EXPORT_SYMBOL(SetChanel);
EXPORT_SYMBOL(ADRangeSelect);
EXPORT_SYMBOL(ReadAD);
