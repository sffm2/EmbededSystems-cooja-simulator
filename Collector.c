#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>

// where the code starts
/*****************************************************/
PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*****************************************************/
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  



 }
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/


static void recv_uc(struct unicast_conn *c, const linkaddr_t *from) {
  printf("Unicast message received from %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
}

static const struct unicast_callbacks unicast_callbacks = { recv_uc };
static struct unicast_conn uc;



PROCESS_THREAD(example_broadcast_process, ev, data)
{
  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();
  // where the broadcast connection is set up returns to the struct function if a packet is recieved
  unicast_open(&uc, 150, &unicast_callbacks);
  broadcast_open(&broadcast,160,&broadcast_call);
  //infinite loop broadcasting a message as the collector.
  while(1) {
    
    /* Delay 2-4 seconds */
    etimer_set(&et, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4));
     //wait until the tickers runs out 
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    packetbuf_copyfrom("collector", 10);
    broadcast_send(&broadcast);
    printf("broadcast message sent\n");
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/