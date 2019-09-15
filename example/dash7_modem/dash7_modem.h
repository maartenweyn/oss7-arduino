#ifndef DASH7_MODEM_H
#define DASH7_MODEM_H

#define BUFFER_SIZE 256


#include <CircularBuffer.h>
#include "stdint.h"
#include "modem.h"



typedef struct {
  uint8_t tag_id;
  bool is_active;
  //CircularBuffer<uint8_t,BUFFER_SIZE>* fifo;
  //uint8_t buffer[256];
} command_t;

// library interface description
class D7_modem
{
  // user-accessible "public" interface
  public:
    D7_modem(void);
    void process_rx(void);
    void register_callbacks(modem_callbacks_t* cbs);

  // library-accessible "private" interface
  private:
    int payload_len = 0;
    bool parsed_header  = false;
    CircularBuffer<uint8_t,BUFFER_SIZE> rx_fifo; 
    command_t command;
    modem_callbacks_t* callbacks;
    
    void process_rx_fifo(void);
    void process_serial_frame();
};

#endif
