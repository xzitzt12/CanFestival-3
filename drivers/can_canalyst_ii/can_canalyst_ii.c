/*
This file is part of CanFestival, a library implementing CanOpen Stack. 

Copyright (C): Edouard TISSERANT and Francis DUPIN

See COPYING file for copyrights details.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
	Virtual CAN driver.
*/

#include <stdio.h>
#include <unistd.h>

#include <stdint.h>
#include "controlcan.h"

#define NEED_PRINT_MESSAGE
#include "can_driver.h"
#include "def.h"

#define DATA_BUFFER (2500)

struct baudrate_t {
  int baudrate_kb;
  uint8_t timing0;
  uint8_t timing1;
};

VCI_BOARD_INFO pInfo[50];
VCI_INIT_CONFIG config;
struct baudrate_t baudrate_array[] = {
  {500, 0x00, 0x1C},
  {1000, 0x00, 0x14}
};
VCI_CAN_OBJ vco[DATA_BUFFER+100];

/*********functions which permit to communicate with the board****************/
UNS8 canReceive_driver(CAN_HANDLE fd0, Message *m)
{
  int recv_len = 0;
  int timeout = 0;

  do {
    recv_len = VCI_Receive(VCI_USBCAN2, 0, 0, vco, DATA_BUFFER, 0);
    timeout++;
  } while ((recv_len <=0) && timeout < 1000);

  if (recv_len <= 0) {
    perror("canReceive_driver (canalyst_ii) : error of reading.\n");
    printf("error code %d\r\n", recv_len);
    return 1;
  }

  m->cob_id = vco[recv_len-1].ID;
  if (vco[recv_len-1].RemoteFlag) {
    m->rtr = 1;
  } else {
    m->rtr = 0;
  }

  m->len = vco[recv_len-1].DataLen;
  
  for (size_t i = 0; i < vco[recv_len-1].DataLen; i++) {
    m->data[i] = vco[recv_len-1].Data[i];
  }

#if defined DEBUG_MSG_CONSOLE_ON
  MSG("in : ");
  print_message(m);
#endif
  
  return 0;
}

/***************************************************************************/
UNS8 canSend_driver(CAN_HANDLE fd0, Message const *m)
{

  VCI_CAN_OBJ msg;

  msg.ID = m->cob_id;
  if (m->rtr) {
    msg.RemoteFlag = 1;
  } else {
    msg.RemoteFlag = 0;
  }
  msg.DataLen = m->len;
  for (size_t i = 0; i < m->len; i++) {
    msg.Data[i] = m->data[i];
  }

#if defined DEBUG_MSG_CONSOLE_ON
  MSG("out : ");
  print_message(m);
#endif

  if (VCI_Transmit(VCI_USBCAN2, 0, 0, &msg, 1) != 1) {
    perror("canSend_driver (canalyst_ii) : error of writing.\n");
    return 1;
  }

  return 0;
}

/***************************************************************************/
int TranslateBaudRate(char* optarg){
	if(!strcmp( optarg, "1M")) return (int)1000;
	if(!strcmp( optarg, "500K")) return (int)500;
	if(!strcmp( optarg, "250K")) return (int)250;
	if(!strcmp( optarg, "125K")) return (int)125;
	if(!strcmp( optarg, "100K")) return (int)100;
	if(!strcmp( optarg, "50K")) return (int)50;
	if(!strcmp( optarg, "20K")) return (int)20;
	if(!strcmp( optarg, "10K")) return (int)10;
	if(!strcmp( optarg, "5K")) return (int)5;
	if(!strcmp( optarg, "none")) return 0;
	return 0x0000;
}

UNS8 canChangeBaudRate_driver( CAN_HANDLE fd0, char* baud)
{
  printf("canChangeBaudRate not yet supported by this driver\n");
	return 0;
}

/***************************************************************************/
CAN_HANDLE canOpen_driver(s_BOARD *board)
{
  int num = 0;
  int baudrate;

  num = VCI_FindUsbDevice2(pInfo);
  printf("USBCAN Device Num %d\r\n", num);

  for(size_t i = 0; i < num; i++) {
    printf("Serial Num :");
    for (size_t j = 0; j < 20; j++) {
      printf("%c", pInfo[i].str_Serial_Num[j]);
    }
    printf("\r\n");
  }

  if (VCI_OpenDevice(VCI_USBCAN2, 0, 0) == 1) {
    printf("Open first device\r\n");
  } else {
    printf("Open first device failed\r\n");
    return (CAN_HANDLE)0;
  }

  config.AccCode = 0;
  config.AccMask = 0xFFFFFFFF;
  config.Filter = 1;
  config.Mode = 0;

  baudrate = TranslateBaudRate(board->baudrate);
  for (size_t i = 0; i < sizeof(baudrate_array)/sizeof(baudrate_array[0]); i++) {
    if (baudrate_array[i].baudrate_kb == baudrate) {
      config.Timing0 = baudrate_array[i].timing0;
      config.Timing1 = baudrate_array[i].timing1;
      break;
    }
  }

  if (VCI_InitCAN(VCI_USBCAN2, 0, 0, &config) != 1) {
    printf("Init device failed\r\n");
    VCI_CloseDevice(VCI_USBCAN2, 0);
  }

  if (VCI_StartCAN(VCI_USBCAN2, 0, 0) != 1) {
    printf("Start device failed\r\n");
    VCI_CloseDevice(VCI_USBCAN2, 0);
  }

  return (CAN_HANDLE)1;
}

/***************************************************************************/
int canClose_driver(CAN_HANDLE fd0)
{
  VCI_CloseDevice(VCI_USBCAN2, 0);
  return 0;
}


