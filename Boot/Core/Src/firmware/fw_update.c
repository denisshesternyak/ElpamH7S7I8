#include <firmware/fw_update.h>
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32_extmem.h"
#include "stm32_extmem_conf.h"

#define FW_UPDATE_HANDLER	&huart4

static const uint32_t FW_SOF = 0xAAAAAAAA;
static const uint32_t FW_EOF = 0x55555555;

uint8_t flash_buffer[EXT_FLASH_SECTOR_SIZE];
uint32_t buffer_index;
uint32_t current_addr_offset;

typedef struct
{
  uint32_t total_size;
  uint16_t total_crc;
  uint16_t packet_size;
  bool is_header;
} FW_Header_t;

static FW_Header_t header;

static uint32_t packet_count = 0;
static uint32_t error_count = 0;
static uint32_t total_error_count = 0;
static uint32_t total_bytes_received = 0;

static uint8_t temp_byte;
static uint8_t rx_buffer[RX_BUFFER_SIZE];
static uint8_t tx_buffer[RX_BUFFER_SIZE];

static bool is_received_packet;
static uint16_t bytes_received = 0;

static uint16_t fw_update_CRC16 (const uint8_t *data, uint16_t length);
static void fw_update_send_packet (uint8_t cmd,
				   const uint8_t *data,
				   uint16_t len);
static FW_STATUS fw_update_receive_packet (uint8_t *cmd,
					   uint8_t *data,
					   uint16_t *len,
					   uint32_t timeout_ms);
static void fw_update_callback (UART_HandleTypeDef *huart);
static void fw_metadata_write (const uint8_t *data, uint16_t len);
static void fw_metadata_finish_write (uint32_t size, uint16_t crc);
static bool fw_update_get_header (void);

static void fw_metadata_write (const uint8_t *data, uint16_t len)
{
  while (len > 0)
  {
    uint16_t space_left = EXT_FLASH_SECTOR_SIZE - buffer_index;
    uint16_t bytes_to_copy = (len < space_left) ? len : space_left;

    memcpy(&flash_buffer[buffer_index], data, bytes_to_copy);
    buffer_index += bytes_to_copy;
    data += bytes_to_copy;
    len -= bytes_to_copy;

    if (buffer_index >= EXT_FLASH_SECTOR_SIZE)
    {
      EXTMEM_EraseSector(EXTMEMORY_1, current_addr_offset, EXT_FLASH_SECTOR_SIZE);
      EXTMEM_Write(EXTMEMORY_1, current_addr_offset, flash_buffer, EXT_FLASH_SECTOR_SIZE);

      current_addr_offset += EXT_FLASH_SECTOR_SIZE;
      buffer_index = 0;
    }
  }
}

static void fw_metadata_finish_write (uint32_t size, uint16_t crc)
{
  if (buffer_index > 0)
  {
    EXTMEM_EraseSector(EXTMEMORY_1, current_addr_offset, EXT_FLASH_SECTOR_SIZE);
    EXTMEM_Write(EXTMEMORY_1, current_addr_offset, flash_buffer, EXT_FLASH_SECTOR_SIZE);
  }

  current_addr_offset = 0;
}

static uint16_t fw_update_CRC16 (const uint8_t *data, uint16_t length)
{
  uint16_t crc = 0xFFFF;
  for (uint16_t i = 0; i < length; i++)
  {
    crc ^= data[i];
    for (uint8_t j = 0; j < 8; j++)
    {
      if (crc & 0x0001)
	crc = (crc >> 1) ^ 0xA001;
      else
	crc >>= 1;
    }
  }
  return crc;
}

static void fw_update_send_packet (uint8_t cmd,
				   const uint8_t *data,
				   uint16_t len)
{
  uint16_t crc;
  uint16_t idx = 0;

  memcpy(&tx_buffer[idx], &FW_SOF, 4);
  idx += 4;

  tx_buffer[idx++] = cmd;
  tx_buffer[idx++] = len & 0xFF;
  tx_buffer[idx++] = (len >> 8) & 0xFF;

  if (data && len > 0)
  {
    memcpy(&tx_buffer[idx], data, len);
    idx += len;
  }

  crc = fw_update_CRC16(tx_buffer, idx);
  tx_buffer[idx++] = crc & 0xFF;
  tx_buffer[idx++] = (crc >> 8) & 0xFF;

  memcpy(&tx_buffer[idx], &FW_EOF, 4);
  idx += 4;

//  HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);
  HAL_UART_Transmit(FW_UPDATE_HANDLER, tx_buffer, idx, 100);
//  HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);
}

//[FW_SOF:4] [CMD] [LEN:2] [DATA] [CRC16:2] [FW_EOF:4]
static FW_STATUS fw_update_receive_packet (uint8_t *cmd,
					   uint8_t *data,
					   uint16_t *len,
					   uint32_t timeout_ms)
{
  uint32_t start = HAL_GetTick();
  uint16_t packet_len;

  memset(rx_buffer, 0, RX_BUFFER_SIZE);

  is_received_packet = false;

  while (HAL_GetTick() - start < timeout_ms && !is_received_packet)
  {
    HAL_Delay(1);
  }

//  printf("Recived data:\r\n");
//  for(int i = 0; i < bytes_received; i++)
//  {
//    printf("0x%02x ", rx_buffer[i]);
//  }
//  printf("\r\n");

  if (!is_received_packet)
    return FW_NSIZE;

  if (memcmp(rx_buffer, &FW_SOF, 4) != 0)
    return FW_NSOF;

  packet_len = rx_buffer[5] | (rx_buffer[6] << 8);
  if (packet_len > MAX_PACKET_DATA)
    return FW_NSIZE;

  if (memcmp(&rx_buffer[bytes_received - 4], &FW_EOF, 4) != 0)
    return FW_NEOF;

  uint16_t received_crc = rx_buffer[bytes_received - 6] | (rx_buffer[bytes_received - 5] << 8);
  uint16_t calc_crc = fw_update_CRC16(rx_buffer, bytes_received - 6);

  if (calc_crc != received_crc)
    return FW_NCRC;

  *cmd = rx_buffer[4];
  *len = packet_len;
  if (packet_len > 0 && data != NULL)
    memcpy(data, &rx_buffer[7], packet_len);

  bytes_received = 0;
  error_count = 0;

  return FW_OK;
}

static bool fw_update_get_header (void)
{
  if(header.is_header) return true;

  uint8_t cmd;
  uint16_t len;
  FW_STATUS status;

  status = fw_update_receive_packet(&cmd, (uint8_t*) &header, &len, 1000);

  if (status != FW_OK || cmd != CMD_START_UPDATE)
  {
    bytes_received = 0;
//    printf("Timeout or wrong command %d\r\n", status);
    return false;
  }

  error_count = 0;
  fw_update_send_packet(CMD_ACK, NULL, 0);

  header.is_header = true;
  printf("Size FW %ld, CRC 0x%04x\r\n", header.total_size, header.total_crc);

//  printf("Start update received\r\n");
  return true;
}

void fw_update_process (void)
{
  uint8_t cmd;
  uint16_t len;
  uint8_t data[MAX_PACKET_DATA];
  FW_STATUS status;

  usart_register_rx_callback(FW_UPDATE_HANDLER, fw_update_callback);
  HAL_UART_Receive_IT(FW_UPDATE_HANDLER, &temp_byte, 1);

  current_addr_offset = 0;
  buffer_index = 0;

  memset(&header, 0, sizeof(header));

  fw_update_send_packet(CMD_READY, NULL, 0);

  while (error_count < MAX_ERROR_COUNT)
  {
    if (!fw_update_get_header())
    {
      bytes_received = 0;
      error_count++;
      total_error_count++;
      continue;
    }

    status = fw_update_receive_packet(&cmd, data, &len, 3000);

    if (status != 0)
    {
      bytes_received = 0;
      error_count++;
      total_error_count++;
      fw_update_send_packet(CMD_NACK, NULL, 0);
      printf("Packet receive error: %d\r\n", status);
      continue;
    }

    if (cmd == CMD_DATA_PACKET)
    {
      packet_count++;
      total_bytes_received += len;

      fw_metadata_write(data, len);

      fw_update_send_packet(CMD_ACK, NULL, 0);

//      if (packet_count % 20 == 0)
//	printf("Packets: %lu\r\n", packet_count);
    }
    else if (cmd == CMD_END_UPDATE)
    {
      fw_metadata_finish_write(header.total_size, header.total_crc);

//      printf("End of update received\r\n");
      fw_update_send_packet(CMD_ACK, NULL, 0);
      break;
    }
    else
    {
      error_count++;
      total_error_count++;
      fw_update_send_packet(CMD_NACK, NULL, 0);
    }
  }

  uint8_t stats[12];
  memcpy(&stats[0], &packet_count, 4);
  memcpy(&stats[4], &total_error_count, 4);
  memcpy(&stats[8], &total_bytes_received, 4);

  fw_update_send_packet(CMD_STATISTIC, stats, 12);

  printf("Update finished. Packets: %lu, Errors: %lu, Bytes: %lu\r\n", packet_count, total_error_count, total_bytes_received);
}

static void fw_update_callback (UART_HandleTypeDef *huart)
{
  rx_buffer[bytes_received++] = temp_byte;

  if (bytes_received == 4 && memcmp(rx_buffer, &FW_SOF, 4) != 0)
  {
    bytes_received = 0;
  }
  else if (bytes_received >= 8 && memcmp(&rx_buffer[bytes_received - 4], &FW_EOF, 4) == 0)
  {
    is_received_packet = true;
  }
  else if (bytes_received >= RX_BUFFER_SIZE)
  {
    bytes_received = 0;
  }

  HAL_UART_Receive_IT(FW_UPDATE_HANDLER, &temp_byte, 1);
}
