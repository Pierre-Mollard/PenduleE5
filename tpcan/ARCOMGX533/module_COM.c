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

/* define pour gestion PCI CARTE CAN 7841 */
#define CAN_VENDOR_ID 0x144A
#define CAN_DEVICE_ID 0x7841

/* déclaration variables pour addresse et irq */
/*  de la carte CAN PCI 7841                  */
static u32 base;  //adresse de base de la carte CAN
static u32 base2;
static unsigned int irq_7841;

/* define pour gestion registres CAN 7841 */
#define vBTR0 0x5F  //(0101 1111 car facteur de 95)
#define vBTR1 0x1c  
#define vOCR 0xFA

#define BUS1_CR (base)
#define BUS1_CMR (base+1)
#define BUS1_SR_STATUS_REGISTER (base+2)
#define BUS1_ACR (base+4)
#define BUS1_AMR (base+5)
#define BUS1_BTR0 (base+6)
#define BUS1_BTR1 (base+7)
#define BUS1_OCR (base+8)
#define BUS1_DSCR1 (base+10)
#define BUS1_DSCR2 (base+11)

/*******************************************************/
/* initialisation de la carte CAN                      */
/*     - recherche de l'adresse et de l'irq sur le PCI */
/*     - initialisation du SJA1000 : a completer       */
/******************************************************/
void init_can(void)
{
  /* initialisation PCI */
  u16 vendor,device;   
  u8 revision;
  struct pci_dev *dev=NULL;
 
  // RECHERCHE DE L'ADRESSE DE LA CARTE SUR LE BUS PCI

 if(pci_present())
    {
      dev = pci_find_device(CAN_VENDOR_ID, CAN_DEVICE_ID,dev);
      pci_read_config_dword(dev,PCI_BASE_ADDRESS_2,&base);
      if(dev==NULL) 
	{
	  printk("cannot init PCI 7841 card\n");
	  return -1;
	}
    } 
  else 
    {
      printk("cannot init PCI 7841 card\n");
      return -1 ;
    }
  
  base &= PCI_BASE_ADDRESS_IO_MASK; //adresse de base du 1er SJA1000
  printk("CARTE 7841 : BASE  0x%4x\n",base);
  base2 =  base + 0x80;   //adresse de base du 2eme SJA1000
  
  irq_7841 = dev->irq;  //on récupère l'irq de la carte CAN
  
  printk("CARTE 7841 Irq: %d\n", irq_7841);
  pci_read_config_byte(dev,PCI_REVISION_ID,&revision);
  printk("CARTE 7841 Revision : %u\n", revision);
  pci_read_config_word(dev,PCI_VENDOR_ID,&vendor);
  printk("CARTE 7841, VendorId : %x\n", vendor);
  pci_read_config_word(dev,PCI_DEVICE_ID,&device);
  printk("CARTE 7841 DeviceId : %x\n", device);
  printk("CARTE 7841 Base : %x\n", base);
  
  /**** initialisation SJA 1000 ****/
  //ACR Acceptance Code Register (Tous message)
  outb(0xFF,BUS1_ACR);

  //AMR Acceptance mask Register (Tous info de message)
  outb(0xFF,BUS1_AMR);

  //BTR
  outb(vBTR0,BUS1_BTR0);
  outb(vBTR1,BUS1_BTR1);

  //OCR
  outb(vOCR,BUS1_OCR);

  //CR Activation des interruptions de transmission et reception
  outb(0x06,BUS1_CR);
}

void sendmsg(int id, int dlc, void * msg){
	u8 *tablo[8];
	memcpy(msg, *tablo, dlc);

if(id <= 7){
	outb(0x00, BUS1_DSCR1);
        outb(0x1F + (id), BUS1_DSCR2);
}else{

}
	outb(0x01, BUS1_DSCR1);
        outb(0x05, BUS1_DSCR2);

	for(int = 0; i <= dlc; i++){
		outb(tablo(i), BUS1_DSCR2+i);
	}
}

void tache_periodique(long id)
{
  while(1)
    {
     printk("test envoi");

     if(inb(BUS1_SR_STATUS_REGISTER)&&(0x04)==0x04){
	

     	outb(0x01, BUS1_DSCR1);
        outb(0x05, BUS1_DSCR2);

        outb(0x80, BUS1_DSCR2+1);
        outb(0x73, BUS1_DSCR2+2);
        outb(0x69, BUS1_DSCR2+3);
        outb(0x82, BUS1_DSCR2+4);
        outb(0x69, BUS1_DSCR2+5);


        outb(0x01, BUS1_CMR);
     }

     rt_task_wait_period();
}

static int modCOM_init(void) {

  int ierr;
  RTIME now;

  init_can();

  //taches
  rt_set_oneshot_mode();

  start_rt_timer(nano2count(TICK_PERIOD));
  now = rt_get_time();
 
  printk("Init Module COM\n");
  
 return(0);
}

static void modCOM_exit(void) {
 stop_rt_timer(); 

}

module_init(modCOM_init);
module_exit(modCOM_exit);
