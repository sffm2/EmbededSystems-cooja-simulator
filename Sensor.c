#include "contiki.h"
#include "net/rime/rime.h"

#include "dev/sht11/sht11-sensor.h"



#include <stdio.h>
//how the unicast process begins it has a name and and a variable name also it start automatically 
/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "Example unicast");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/


static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  printf("unicast message received from %d.%d:\n",
	 from->u8[0], from->u8[1]);
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
static  int temperature;
static  int storage[4];
// the collecter ask for for the average temp reading also a packet identifer
static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from){
  
  printf("broadcast message received from %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());

  linkaddr_t add ;

  char charac [10];
   

  sprintf(charac, "%d", temperature);

  packetbuf_copyfrom(charac, 7);

  add.u8[0] = from->u8[0];
  add.u8[1] = from->u8[1];

  if(!linkaddr_cmp(&add, &linkaddr_node_addr)) {
      unicast_send(&uc, &add);
  }
}


static const struct broadcast_callbacks broadcast_call = { broadcast_recv };
static struct broadcast_conn broadcast;


PROCESS_THREAD(example_unicast_process, ev, data)
{
  
PROCESS_EXITHANDLER(unicast_close(&uc));
    
 
  PROCESS_BEGIN();
  
  SENSORS_ACTIVATE(sht11_sensor);
  
// sets the connection with the bandwidth and the call back to the function if a packet is recieved
  unicast_open(&uc, 150, &unicast_callbacks);
  broadcast_open(&broadcast,160,&broadcast_call);
// populating the array with zeros 
	int i =0;
	for ( i =0 ;i<=4 ;i++)
	{
	storage[i] = 0;
	}
 // periodic timer called e timer 
 static struct etimer et;
 while(1) 
  {
   
    etimer_set(&et, CLOCK_SECOND * 4);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
   //aquiring a value then calibrating this value to be more realistic temprature reading 
    int value = sht11_sensor.value(SHT11_SENSOR_TEMP);
    temperature = (-39.60 + (0.01*value));
// populating the array such that if i is less than 4 it will continue to take up  the   next available position   untill the array is full  
for(i = 0; i < 4; i++){
	    if((i + 1) < 4){
	    storage[i] += storage[i + 1];
      }
    }
   // the last position in the array will always be taken up by the most recent value reading then this value will be 0
   storage[3] = value;
   value =0;
 // i sum all the values in the array to get the average temp 
  for (i=0;i<4;i++){
   value += storage[3];
   }
    //formula to get average temp
    temperature= value/4;
   //packet buffer with the copy function and pointer back to the struct
    packetbuf_copyfrom("unicast", 10);
    broadcast_send(&broadcast);
    printf("unicast message sent\n");
   SENSORS_DEACTIVATE(sht11_sensor);
  }

  PROCESS_END();
}