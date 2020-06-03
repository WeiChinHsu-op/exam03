#include "mbed.h"
#include "mbed_rpc.h"

RawSerial pc(USBTX, USBRX);
RawSerial xbee(D12, D11);

EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

void pc_rx_interrupt(void);
void pc_rx(void);
void xbee_rx_interrupt(void);
void xbee_rx(void);
void reply_messange(char *xbee_reply, char *messange);
void check_addr(char *xbee_reply, char *messenger);


int main()
{
  pc.baud(9600);
  char xbee_reply[4];

  // XBee setting
  xbee.baud(9600);
  xbee.printf("+++");
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  if(xbee_reply[0] == 'O' && xbee_reply[1] == 'K'){
    pc.printf("enter AT mode.\r\n");
    xbee_reply[0] = '\0';
    xbee_reply[1] = '\0';
  }

  xbee.printf("ATMY 0x240\r\n");
  reply_messange(xbee_reply, "setting MY : 0x240");

  xbee.printf("ATDL 0x140\r\n");
  reply_messange(xbee_reply, "setting DL : 0x140");

  xbee.printf("ATID 0x1\r\n");
  reply_messange(xbee_reply, "setting PAN ID : 0x1");

  xbee.printf("ATWR\r\n");
  reply_messange(xbee_reply, "write config");

  xbee.printf("ATMY\r\n");
  check_addr(xbee_reply, "MY");

  xbee.printf("ATDL\r\n");
  check_addr(xbee_reply, "DL");

  xbee.printf("ATCN\r\n");
  reply_messange(xbee_reply, "exit AT mode");
  xbee.getc();

  // start
  pc.printf("start\r\n");
  t.start(callback(&queue, &EventQueue::dispatch_forever));

  // Setup a serial interrupt function of receiving data from pc
  pc.attach(pc_rx_interrupt, Serial::RxIrq);
  // Setup a serial interrupt function of receiving data from xbee
  xbee.attach(xbee_rx_interrupt, Serial::RxIrq);
}

void pc_rx_interrupt(void)
{
  pc.attach(NULL, Serial::RxIrq); // detach interrupt
  queue.call(&pc_rx);
}

void pc_rx(void)
{
  static int i = 0;
  static char str[50] = {0};
  while(pc.readable()){
    char c = pc.getc();
    if(c!='\r' && c!='\n'){
      pc.putc(c);
      str[i] = c;
      i++;
      str[i] = '\0';
      if(strncmp(str, "+++", 3) == 0){
        xbee.printf("+++");
        i = 0;
      }
    }else{
      i = 0;
      xbee.printf("%s\r\n", str);
      pc.printf("\r\n", str);
    }
  }
  wait(0.001);
  pc.attach(pc_rx_interrupt, Serial::RxIrq); // reattach interrupt
}

void xbee_rx_interrupt(void)
{
  xbee.attach(NULL, Serial::RxIrq); // detach interrupt
  queue.call(&xbee_rx);
}

void xbee_rx(void)
{
  while(xbee.readable()){
    char c = xbee.getc();
    if(c!='\r' && c!='\n'){
      pc.printf("%c",c);
    }else{
      pc.printf("\r\n");
    }
  }
  wait(0.001);
  xbee.attach(xbee_rx_interrupt, Serial::RxIrq); // reattach interrupt
}

void reply_messange(char *xbee_reply, char *messange){
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  xbee_reply[2] = xbee.getc();
  if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K'){
    pc.printf("%s\r\n", messange);
    xbee_reply[0] = '\0';
    xbee_reply[1] = '\0';
    xbee_reply[2] = '\0';
  }
}

void check_addr(char *xbee_reply, char *messenger){
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  xbee_reply[2] = xbee.getc();
  xbee_reply[3] = xbee.getc();
  pc.printf("%s = %c%c%c\r\n", messenger, xbee_reply[1], xbee_reply[2], xbee_reply[3]);
  xbee_reply[0] = '\0';
  xbee_reply[1] = '\0';
  xbee_reply[2] = '\0';
  xbee_reply[3] = '\0';
}

void getnumber(Arguments *in, Reply *out){
  int ans;
  //int count;
  //int get;
  int count_previos;
   if(initial==0){
     xbee.printf("%d",number);
     pc.printf("%d",number);
     initial++;
   }
   else{
     ans = number-count_previous;
     xbee.printf("%d",ans);
     pc.printf("%d",ans);
   }

   count_previous = number;
}




