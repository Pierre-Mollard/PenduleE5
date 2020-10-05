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

static RT_TASK task_ctrl;


//matrice de modélisation du systeme
float adc[4][4] = {{0.6300, -0.1206, -0.0008, 0.0086}, {-0.0953, 0.6935, 0.0107, 0.0012}, {-0.2896, -1.9184, 1.1306, 0.2351}, {-3.9680, -1.7733, -0.1546, 0.7222}};
float bdc[4][2] = {{0.3658, 0.1200}, {0.0993, 0.3070}, {1.0887, 2.0141}, {3.1377, 1.6599}};
float cdc[4] = {-80.3092, -9.6237, -14.1215, -23.6260};
float ddc[2] = {0, 0};

//vecteur d'état
float x[4] = {0, 0, 0, 0};
//vecteur sortie
float y[2] = {0, 0};
//vecteur commande
float u = 0;

/*
float[4] mulmatrice444(float[4][4] px, float[4] py){
  float[4] z = {0, 0, 0, 0};
  for(int i = 0, i<4, i++){
    for(int j = 0, j<4, j++){
	z[i] = px[i][j]*py[j];
    }
  }
  return z;
}

float[4] mulmatrice422(float[4][2] px, float[2] py){
  float[4] z = {0, 0, 0, 0};
  for(int i = 0, i<4, i++){
    for(int j = 0, j<2, j++){
	z[i] = px[i][j]*py[j];
    }
  }
  return z;
}
*/

void methode_ctrl(int id){ //tâche controleur
  u16 cmde = 0;
  u16 value0, value1;
  float valueVolt0, valueVolt1;
  while(1){
  	// Code iteratif du controle

	// Acquisition angle/position
	value0 = getAngle();
	value1 = getPos();
	valueVolt0 = (value0-2048)*10/4095;
	valueVolt1 = (value1-2048)*10/4095;
	
	printk("Resultat Angle Volt (-10/+10): %u\n", valueVolt0*1000);
	printk("Resultat Position Volt (-10/+10): %u\n", valueVolt1*1000);

	y[0] = valueVolt0;
	y[1] = valueVolt1;

	printk("Matrice Y : [%u, %u]\n", y[0], y[1]);

	// Calcul vecteur etat et commande
	x[0] = adc[0][0]*x[0] + adc[0][1]*x[1] + adc[0][2]*x[2] + adc[0][3]*x[3] + bdc[0][0]*y[0] + bdc[0][1]*y[1];
        x[1] = adc[1][0]*x[0] + adc[1][1]*x[1] + adc[1][2]*x[2] + adc[1][3]*x[3] + bdc[1][0]*y[0] + bdc[1][1]*y[1];
        x[2] = adc[2][0]*x[0] + adc[2][1]*x[1] + adc[2][2]*x[2] + adc[2][3]*x[3] + bdc[2][0]*y[0] + bdc[2][1]*y[1];
        x[3] = adc[3][0]*x[0] + adc[3][1]*x[1] + adc[3][2]*x[2] + adc[3][3]*x[3] + bdc[3][0]*y[0] + bdc[3][1]*y[1];
	u = cdc[0]*x[0] + cdc[1]*x[1] + cdc[2]*x[2] + cdc[3]*x[3];

	// Convertion en binaire
	cmde = u*1000;
	//cmdd = floor(u*1000) - floor(u)*1000;

	// Envoi commande
	//setCmd(
	printk("Resultat Commande (?/?) : %u\n", cmde);

  	rt_task_wait_period();
  }
}

static int modC_init(void) {

  int ierr;
  RTIME now;

  // Code initial du controleur

  //taches
  rt_set_oneshot_mode();
  ierr = rt_task_init(&task_ctrl, methode_ctrl, 0, STACK_SIZE, PRIORITE, 0, 0);

  start_rt_timer(nano2count(TICK_PERIOD));
  now = rt_get_time();
  rt_task_make_periodic(&task_ctrl, now, nano2count(PERIODE_CONTROL));
 
  printk("Init Module Controleur (C)\n");
 
 return(0);
}

static void modC_exit(void) {
 stop_rt_timer(); 
 rt_task_delete(&task_ctrl);

}

module_init(modC_init);
module_exit(modC_exit);

