#include "dash7_modem.h"
#include "alp.h"
#include <Arduino.h>


#define DPRINT  SerialUSB.println

D7_modem::D7_modem(void)
{
  
}


void D7_modem::process_rx()
{
  static int incomingByte = 0; // for incoming serial data
  
  bool new_data = false;
  // put your main code here, to run repeatedly:
  while (Serial.available() > 0) {
    rx_fifo.push(Serial.read());
    new_data = true;
  }
  if (new_data) process_rx_fifo();
}

void D7_modem::register_callbacks(modem_callbacks_t* cbs)
{
    callbacks = cbs;
    //if(cbs->modem_rebooted_callback)
    //  modem_interface_set_target_rebooted_callback(cbs->modem_rebooted_callback);
}

void D7_modem::process_rx_fifo()
{
  if(!parsed_header) {
    // <sync byte (0xC0)><version (0x00)><length of ALP command (1 byte)><ALP command> // TODO CRC
    if(rx_fifo.size() > SERIAL_ALP_FRAME_HEADER_SIZE) {
        //uint8_t header[SERIAL_ALP_FRAME_HEADER_SIZE];
        //fifo_peek(&rx_fifo, header, 0, SERIAL_ALP_FRAME_HEADER_SIZE);
        //DPRINT_DATA(header, 3); // TODO tmp
        SerialUSB.print(rx_fifo[0], HEX);
        SerialUSB.print(rx_fifo[1], HEX);
        SerialUSB.println(rx_fifo[2], HEX);

        if(rx_fifo[0] != SERIAL_ALP_FRAME_SYNC_BYTE || rx_fifo[1] != SERIAL_ALP_FRAME_VERSION) {
          rx_fifo.shift();
          SerialUSB.println("skip");
          parsed_header = false;
          payload_len = 0;
          if(rx_fifo.size() > SERIAL_ALP_FRAME_HEADER_SIZE)
            process_rx_fifo();

          return;
        }

        parsed_header = true;
        rx_fifo.shift(); // SYNC BYTE
        rx_fifo.shift(); // VERSION
        payload_len = rx_fifo.shift();
        SerialUSB.print("found header, payload size ="); SerialUSB.println(payload_len);
        process_rx_fifo();
    }
  } else {
    if(rx_fifo.size() < payload_len) {
      DPRINT("payload not complete yet");
      return;
    }

    // payload complete, start parsing
    // rx_fifo can be bigger than the current serial packet, init a subview fifo
    // which is restricted to payload_len so we can't parse past this packet.
    //fifo_t payload_fifo;
    //fifo_init_subview(&payload_fifo, &rx_fifo, 0, payload_len);
    //process_serial_frame(&payload_fifo);
    process_serial_frame();

    // pop parsed bytes from original fifo
    //fifo_skip(&rx_fifo, payload_len - fifo_get_size(&payload_fifo));
    parsed_header = false;
  }
 }

 void D7_modem::process_serial_frame() {
  bool command_completed = false;
  bool completed_with_error = false;
  int length_to_go = payload_len;
  
  while(length_to_go > 0) {
    alp_action_t action;
    alp_parse_action(&rx_fifo, &action);

    switch(action.operation) {
      case ALP_OP_RETURN_TAG:
        if(action.tag_response.tag_id == command.tag_id) {
          command_completed = action.tag_response.completed;
          completed_with_error = action.tag_response.error;
        } else {
           // DPRINT("received resp with unexpected tag_id (%i vs %i)", action.tag_response.tag_id, command.tag_id);
          // TODO unsolicited responses
        }
        break;
      case ALP_OP_WRITE_FILE_DATA:
        if(callbacks->write_file_data_callback)
          callbacks->write_file_data_callback(action.file_data_operand.file_offset.file_id,
                                               action.file_data_operand.file_offset.offset,
                                               action.file_data_operand.provided_data_length,
                                               action.file_data_operand.data);
        break;
      case ALP_OP_RETURN_FILE_DATA:
        if(callbacks->return_file_data_callback)
          callbacks->return_file_data_callback(action.file_data_operand.file_offset.file_id,
                                               action.file_data_operand.file_offset.offset,
                                               action.file_data_operand.provided_data_length,
                                               action.file_data_operand.data);
        break;
      case ALP_OP_RETURN_STATUS:
        if(action.status.type==ALP_ITF_ID_D7ASP)
        {
          d7ap_session_result_t interface_status = *((d7ap_session_result_t*)action.status.data);
          uint8_t addressee_len = d7ap_addressee_id_length(interface_status.addressee.ctrl.id_type);
          DPRINT("received resp from: ");
          //DPRINT_DATA(interface_status.addressee.id, addressee_len);
        }
        if(callbacks->modem_interface_status_callback)
          callbacks->modem_interface_status_callback(action.status.type, action.status.data);
        break;
      default:
        //assert(false);
        break;
    }
  }


  if(command_completed) {
    char temp_text[200];
    sprintf(temp_text, "command with tag %i completed", command.tag_id);
    SerialUSB.println(temp_text);
    
    if(callbacks->command_completed_callback)
      callbacks->command_completed_callback(completed_with_error,command.tag_id);

    command.is_active = false;
  }

}
