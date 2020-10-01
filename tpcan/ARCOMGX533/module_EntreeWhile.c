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

#define IT_ACQ 7

int sel_channel = 0;
u16 val_ch1 = 0;
u16 val_ch2 = 0;
SEM sem1; 
SEM sem2; 
SEM semACQ; 

int init3718(void){
  outb(0xF0, REG_CTRL); //activation des interruptions avec Interrupt Level : 7
  outb(1, REG_PACER); //desactivé
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

//////////// ZONE NON PREEMPTIVE

  outb(1, BASE);//trigger conversion

  int channelLu = 0;
  while(inb(REG_STATUS && 0x10) == 0){
     if(inb(REG_STATUS && 0x10) == 1){
	channelLu = inb(BASE) && 15;
    	printk("channel lu : %u\n", channelLu);
    	if(channelLu == sel_channel){
        	int lowbyte = inb(BASE)>>4;
  		int highbyte = inb(REG_RANGE);
  		u16 resultat = lowbyte;
  		resultat = resultat + (highbyte<<4);
  		outb(1, REG_STATUS);//clear INT bit
		return resultat;
     	}
     }else{
	printk("-Ch%u Attente Conversion\n", sel_channel);
     }
  }
  

//////////// FIN ZONE NON PREEMPTIVE

  printk("-Ch%u Fin read\n", sel_channel);

  return 0;
  
}

void gestionnaire_it(void){
  rt_sem_signal(&semACQ);
  rt_ack_irq(IT_ACQ);
}

static int modE_init(void) {

  int ierr;
  RTIME now;
  init3718();

  rt_global_cli(); // desactivation des IT
  rt_request_global_irq(IT_ACQ, gestionnaire_it); //installation du handler sur l'IT_ACQ 
  rt_startup_irq(IT_ACQ); //activation de la ligne d'IT
  rt_global_sti(); // activation des IT

  //taches
  rt_set_oneshot_mode();
  
  rt_typed_sem_init(&sem1, 1, BIN_SEM);
  rt_typed_sem_init(&sem2, 0, BIN_SEM);
  rt_typed_sem_init(&semACQ, 0, BIN_SEM);

  start_rt_timer(nano2count(TICK_PERIOD));
  now = rt_get_time();
  
  

  printk("Init Module Entree\n");

  
 return(0);
}

static void modE_exit(void) {
 stop_rt_timer(); 
 rt_sem_delete(&sem1);
 rt_sem_delete(&sem2);
 rt_sem_delete(&semACQ);
 rt_shutdown_irq(IT_ACQ); //désinstallation du handler sur l'IT_ACQ 
 rt_free_global_irq(IT_ACQ); //désinstallation du handler
}

module_init(modE_init);
module_exit(modE_exit);

EXPORT_SYMBOL(SetChanel);
EXPORT_SYMBOL(ADRangeSelect);
EXPORT_SYMBOL(ReadAD);
