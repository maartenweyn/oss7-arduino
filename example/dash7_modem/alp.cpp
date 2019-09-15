#include "alp.h"

#include <CircularBuffer.h>

#define DPRINT  SerialUSB.println



uint32_t alp_parse_length_operand(CircularBuffer<uint8_t,BUFFER_SIZE>* buffer) {
  uint8_t len = 0;
  len = buffer->shift();
  //fifo_pop(cmd_fifo, (uint8_t*)&len, 1);
  uint8_t field_len = len >> 6;
  if(field_len == 0)
    return (uint32_t)len;

  uint32_t full_length = (len & 0x3F) << ( 8 * field_len); // mask field length specificier bits and shift before adding other length bytes
  uint8_t* full_lenght_ptr = (uint8_t*)&full_length;
  for (int i = 0; i < field_len; i++)
   *full_lenght_ptr++ = buffer->shift();
   
  return full_length;
}

alp_operand_file_offset_t alp_parse_file_offset_operand(CircularBuffer<uint8_t,BUFFER_SIZE>* buffer) {
  alp_operand_file_offset_t operand;
  operand.file_id =  buffer->shift();
 // error_t err = fifo_pop(cmd_fifo, &operand.file_id, 1); assert(err == SUCCESS);
  operand.offset = alp_parse_length_operand(buffer);
  return operand;
}

static void parse_operand_file_data(CircularBuffer<uint8_t,BUFFER_SIZE>* buffer, alp_action_t* action) {
  action->file_data_operand.file_offset = alp_parse_file_offset_operand(buffer);
  action->file_data_operand.provided_data_length = alp_parse_length_operand(buffer);
  //assert(action->file_data_operand.provided_data_length <= sizeof(action->file_data_operand.data));
  for (int i = 0; i < action->file_data_operand.provided_data_length; i++)
  {
    action->file_data_operand.data[i] = buffer->shift();
  }
  //fifo_pop(fifo, action->file_data_operand.data, action->file_data_operand.provided_data_length);
}

static void parse_op_write_file_data(CircularBuffer<uint8_t,BUFFER_SIZE>* buffer, alp_action_t* action) {
  parse_operand_file_data(buffer, action);
  SerialUSB.print("parsed write file data file "); 
  SerialUSB.print(action->file_data_operand.file_offset.file_id); 
  SerialUSB.print(", len "); 
  SerialUSB.println(action->file_data_operand.provided_data_length);
}


static void parse_op_return_file_data(CircularBuffer<uint8_t,BUFFER_SIZE>* buffer, alp_action_t* action) {
  parse_operand_file_data(buffer, action);
  //DPRINT("parsed return file data file %i, len %i", action->file_data_operand.file_offset.file_id, action->file_data_operand.provided_data_length);
}

static void parse_op_return_tag(CircularBuffer<uint8_t,BUFFER_SIZE>* buffer, alp_action_t* action, bool b6, bool b7) {
  action->tag_response.completed = b7;
  action->tag_response.error = b6;
  //assert(fifo_pop(fifo, &action->tag_response.tag_id, 1) == SUCCESS);
  action->tag_response.tag_id = buffer->shift();
  //DPRINT("parsed return tag %i, eop %i, err %i", action->tag_response.tag_id, action->tag_response.completed, action->tag_response.error);
}

static void parse_op_return_status(CircularBuffer<uint8_t,BUFFER_SIZE>* buffer, alp_action_t* action, bool b6, bool b7) {
   uint8_t int_bufffer[8];
      
  //assert(b6 && !b7); // TODO implement action status
  uint8_t itf_id = buffer->shift();
  //assert(fifo_pop(fifo, &itf_id, 1) == SUCCESS);
  // TODO uint32_t itf_len = parse_length_operand(fifo);
  // assert(itf_len == sizeof(d7ap_session_result_t));
  action->status.type= (alp_itf_id_t) itf_id;
  if (itf_id == ALP_ITF_ID_D7ASP)
  {
    d7ap_session_result_t* interface_status =  ((d7ap_session_result_t*)action->status.data);

    interface_status->channel.channel_header = buffer->shift();


    int_bufffer[1] = buffer->shift();
    int_bufffer[0] = buffer->shift();
    memcpy(&interface_status->channel.center_freq_index, int_bufffer, 2);
    interface_status->rx_level = buffer->shift();
    interface_status->link_budget = buffer->shift();
    interface_status->target_rx_level = buffer->shift();
    interface_status->status.raw = buffer->shift();
    interface_status->fifo_token = buffer->shift();
    interface_status->seqnr = buffer->shift();
    interface_status->response_to = buffer->shift();
    interface_status->addressee.ctrl.raw = buffer->shift();
    interface_status->addressee.access_class = buffer->shift();
    uint8_t addressee_len = d7ap_addressee_id_length(interface_status->addressee.ctrl.id_type);

    for (int i = 0; i< addressee_len; i++) 
      int_bufffer[i] = buffer->shift();
  
    memcpy(&interface_status->addressee.id, int_bufffer, addressee_len);
  }
  else if ( (itf_id == ALP_ITF_ID_LORAWAN_OTAA) || (itf_id == ALP_ITF_ID_LORAWAN_ABP))
  {
    lorawan_session_result_t* interface_status = ((lorawan_session_result_t*)action->status.data);
    buffer->shift();
    interface_status->attempts = buffer->shift();
    interface_status->error_state = (lorawan_stack_status_t) buffer->shift();
    int_bufffer[1] = buffer->shift();
    int_bufffer[0] = buffer->shift();
    memcpy(&interface_status->duty_cycle_wait_time, int_bufffer, 2);
    
    //fifo_skip(fifo,1); //size
//    fifo_pop(fifo, &interface_status->attempts, 1);
//    fifo_pop(fifo, &interface_status->error_state, 1);
//    fifo_pop(fifo,  (uint8_t*)&interface_status->duty_cycle_wait_time,2);
   // interface_status->duty_cycle_wait_time= __builtin_bswap16(interface_status->duty_cycle_wait_time);
  }

  DPRINT("parsed interface status");
}

void alp_parse_action(CircularBuffer<uint8_t,BUFFER_SIZE>* buffer, alp_action_t* action) {
  uint8_t op = buffer->shift();
  bool b6 = (op >> 6) & 1;
  bool b7 = op >> 7;
  op &= 0x3F; // op is in b5-b0
  action->operation = (alp_operation_t) op;
  switch(op) {
    case ALP_OP_WRITE_FILE_DATA:
      parse_op_write_file_data(buffer, action);
      break;
    case ALP_OP_RETURN_FILE_DATA:
      parse_op_return_file_data(buffer, action);
      break;
    case ALP_OP_RETURN_TAG:
      parse_op_return_tag(buffer, action, b6, b7);
      break;
    case ALP_OP_RETURN_STATUS:
      parse_op_return_status(buffer, action, b6, b7);
      break;
    default:
      SerialUSB.print("op not implemented: "); SerialUSB.println(op);
     // assert(false);
  }

  DPRINT("parsed action");

}
