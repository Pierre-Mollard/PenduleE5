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
float adc[4][4] = {{0.6300f, -0.1206f, -0.0008f, 0.0086f}, {-0.0953f, 0.6935f, 0.0107f, 0.0012f}, {-0.2896f, -1.9184f, 1.1306f, 0.2351f}, {-3.9680f, -1.7733f, -0.1546f, 0.7222f}};
float bdc[4][2] = {{0.3658f, 0.1200f}, {0.0993f, 0.3070f}, {1.0887f, 2.0141f}, {3.1377f, 1.6599f}};
float cdc[4] = {-80.3092f, -9.6237f, -14.1215f, -23.6260f};
float ddc[2] = {0.0f, 0.0f};

//vecteur d'état
float x[4] = {0.0f, 0.0f, 0.0f, 0.0f};
//vecteur sortie
float y[2] = {0.0f, 0.0f};
//vecteur commande
float u = 0.0f;

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
  float valueNorm0, valueNorm1;
  float temp[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  while(1){
  	// Code iteratif du controle

	// Acquisition angle/position
	value0 = getAngle(&ptr_angle);
	value1 = getPos(&ptr_pos);

	valueNorm0 = (value0-2036)*0.298974/(2792-2036);		//17.13 °
	valueNorm1 = (value1-2162)*0.91/(3880-2162);
	
	printk("Resultat Angle (cal): %u\n", value0);
	printk("Resultat Pos (cal): %u\n", value1);

	//printk("Resultat Angle (-10/+10): %d\n", (int)(valueNorm0*1000));
	//printk("Resultat Position (-10/+10): %d\n", (int)(valueNorm1*1000));
	
	y[0] = valueNorm0;
	y[1] = valueNorm1;

	//printk("Matrice Y : [%d, %d]\n", (int)(y[0]*1000), (int)(y[1]*1000));

	// Calcul vecteur etat et commande
	temp[0] = adc[0][0]*x[0] + adc[0][1]*x[1] + adc[0][2]*x[2] + adc[0][3]*x[3] + bdc[0][0]*y[0] + bdc[0][1]*y[1];
        temp[1] = adc[1][0]*x[0] + adc[1][1]*x[1] + adc[1][2]*x[2] + adc[1][3]*x[3] + bdc[1][0]*y[0] + bdc[1][1]*y[1];
        temp[2] = adc[2][0]*x[0] + adc[2][1]*x[1] + adc[2][2]*x[2] + adc[2][3]*x[3] + bdc[2][0]*y[0] + bdc[2][1]*y[1];
        temp[3] = adc[3][0]*x[0] + adc[3][1]*x[1] + adc[3][2]*x[2] + adc[3][3]*x[3] + bdc[3][0]*y[0] + bdc[3][1]*y[1];
	x[0] = temp[0];
	x[1] = temp[1];
	x[2] = temp[2];
	x[3] = temp[3];
	u = 0.9*cdc[0]*x[0] + 0.9*cdc[1]*x[1] + 0.9*cdc[2]*x[2] + 0.9*cdc[3]*x[3]; //1.4* = gain yolo

	//printk("Matrice X : [%d, %d, %d, %d]\n", (int)(x[0]*1000), (int)(x[1]*1000), (int)(x[2]*1000), (int)(x[3]*1000));

	// Convertion en binaire
	if(u >= 10){
		u = 10;
	}else if(u <= -10){
		u = -10;
	}
	cmde = (u+10)*(-4095)/20;

	// Envoi commande
	setCmd(cmde);
	//printk("Resultat Commande (-10/+10) : %d (%u)\n", (int)(u*1000), cmde);
	
  	rt_task_wait_period();
  }
}

static int modC_init(void) {

  int ierr;
  RTIME now;

  // Code initial du controleur

  //taches
  rt_set_oneshot_mode();
  ierr = rt_task_init(&task_ctrl, methode_ctrl, 0, STACK_SIZE, PRIORITE, 1, 0); //Registre float (cf plus tard)

  start_rt_timer(nano2count(TICK_PERIOD));
  now = rt_get_time();
  rt_task_make_periodic(&task_ctrl, now, nano2count(PERIODE_CONTROL/2));
 
  printk("Init Module Controleur (C)\n");
 
 return(0);
}

static void modC_exit(void) {
 stop_rt_timer();
 rt_task_delete(&task_ctrl);

}

module_init(modC_init);
module_exit(modC_exit);

