#include <audio_cmd.h>
#include <audio_regs.h>
#include "FreeRTOS.h"
#include <cmsis_os2.h>
#include "spi.h"
#include "logger.h"

#define CODEC_SPI_HANDLER	&hspi4

static void CS_LOW ()
{
  HAL_GPIO_WritePin(CODEC_CS_GPIO_Port, CODEC_CS_Pin, GPIO_PIN_RESET);
}
static void CS_HIGH ()
{
  HAL_GPIO_WritePin(CODEC_CS_GPIO_Port, CODEC_CS_Pin, GPIO_PIN_SET);
}
static void audio_cmd_write_cmd (uint8_t reg, uint8_t value);
//static uint8_t audio_cmd_read_cmd (uint8_t reg);

static void audio_cmd_write_cmd (uint8_t reg, uint8_t value)
{
  uint8_t data_tx[2];
  data_tx[0] = reg << 1 | 0x00;
  data_tx[1] = value;

  CS_LOW();
  HAL_SPI_Transmit(CODEC_SPI_HANDLER, data_tx, 2, 100);
  CS_HIGH();
}

//static uint8_t audio_cmd_read_cmd (uint8_t reg)
//{
//  uint8_t data_rx[2];
//  uint8_t data_tx[2];
//  data_tx[0] = reg << 1 | 0x01;
//  data_tx[1] = 0xFF;
//
//  CS_LOW();
//  HAL_SPI_TransmitReceive(CODEC_SPI_HANDLER, data_tx, data_rx, 2, 100);
//  CS_HIGH();
//
//  return data_rx[1];
//}

void audio_cmd_reset (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x00);		// Page 0
  audio_cmd_write_cmd(AIC32X4_RESET, 0x01);		// Software reset
}

void audio_cmd_init_power (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);		// Page 1
  audio_cmd_write_cmd(AIC32X4_PWRCFG, 0x08);    // Disable weak AVDD connection
  audio_cmd_write_cmd(AIC32X4_LDOCTL, 0x00); // Master analog power control (LDO off if external supply)
  audio_cmd_write_cmd(AIC32X4_CMMODE, 0x00);	// Input common mode 0.9 V
  audio_cmd_write_cmd(AIC32X4_REFPOWERUP, 0x01); // REF charging time 40 ms
}

void audio_cmd_init_playback (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x00);		// Page 0
  audio_cmd_write_cmd(AIC32X4_CLKMUX, 0x00);// CODEC_CLKIN = MCLK directly (no PLL)

  audio_cmd_write_cmd(AIC32X4_NDAC, 0x81);		// NDAC = 1, powered up
  audio_cmd_write_cmd(AIC32X4_MDAC, 0x82);		// MDAC = 2, powered up
  audio_cmd_write_cmd(AIC32X4_DOSRMSB, 0x00);		// DOSR = 1024
  audio_cmd_write_cmd(AIC32X4_DOSRLSB, 0x80);		// DOSR = 128

  audio_cmd_write_cmd(AIC32X4_IFACE1, 0x01); // 0x00 = I2S, 16-bit, BCLK/WCLK as inputs (slave)
  audio_cmd_write_cmd(AIC32X4_DACSPB, 0x08); // PRB_P8 for DAC (simple playback)

  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);		// Page 1
  audio_cmd_write_cmd(AIC32X4_HEADSTART, 0x25);	// Headphone soft stepping (pop reduction)
  audio_cmd_write_cmd(AIC32X4_LPLAYBACK, 0x00);	// Playback Configuration Register 1
  audio_cmd_write_cmd(AIC32X4_RPLAYBACK, 0x00);	// Playback Configuration Register 2

  audio_cmd_write_cmd(AIC32X4_HPLROUTE, 0x08); 		// LDAC -> HPL
  audio_cmd_write_cmd(AIC32X4_HPRROUTE, 0x08);   	// RDAC -> HPR
  audio_cmd_write_cmd(AIC32X4_HPLGAIN, 0x40);   	// HPL gain 0 dB
  audio_cmd_write_cmd(AIC32X4_HPRGAIN, 0x40);		// HPR gain 0 dB

//	audio_cmd_write_cmd(AIC32X4_OUTPWRCTL, 0x30); 		// Power up HPL and HPR drivers

//	audio_cmd_write_cmd(AIC32X4_LOLROUTE, 0x08); 		// LDAC -> LOL
//	audio_cmd_write_cmd(AIC32X4_LORROUTE, 0x08);   		// RDAC -> LOR
  audio_cmd_write_cmd(AIC32X4_LOLGAIN, 0x40);  // LOL gain 0 dB, driver is muted
  audio_cmd_write_cmd(AIC32X4_LORGAIN, 0x40);  // LOR gain 0 dB, driver is muted
//	audio_cmd_write_cmd(AIC32X4_OUTPWRCTL, 0x0C); 	// Power up LOL and LOR drivers

  audio_cmd_write_cmd(AIC32X4_OUTPWRCTL, 0x3F); // Power up HPL/HPR and LOL/LOR and MAL/MAR

  osDelay(200);	// Wait for soft stepping (2.5 sec in TI example)

  audio_cmd_write_cmd(AIC32X4_PSEL, 0x00);		// Page 0
  // 0xC8 -28dB,
//    audio_cmd_write_cmd(AIC32X4_LDACVOL, 0x81); 		// Left DAC Channel Digital Volume -63.5dB
//    audio_cmd_write_cmd(AIC32X4_RDACVOL, 0x81); 		// Right DAC Channel Digital Volume -63.5dB
//  audio_cmd_write_cmd(AIC32X4_LDACVOL, 0x00); // Left DAC Channel Digital Volume 0dB
//  audio_cmd_write_cmd(AIC32X4_RDACVOL, 0x00); // Right DAC Channel Digital Volume 0dB
  audio_cmd_write_cmd(AIC32X4_DACSETUP, 0xD6); // LDAC + RDAC powered, soft step 1/fs
//    audio_cmd_write_cmd(AIC32X4_DACMUTE, 0x0C); 		// Mute both DACs, gain 0 dB
  audio_cmd_write_cmd(AIC32X4_DACMUTE, 0x00); 	// Unmute both DACs, gain 0 dB

}

void audio_cmd_init_record (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x00);		// Page 0
  audio_cmd_write_cmd(AIC32X4_NADC, 0x82);   		// NADC = 2, powered up
  audio_cmd_write_cmd(AIC32X4_MADC, 0x86);     		// MADC = 6, powered up
  audio_cmd_write_cmd(AIC32X4_AOSR, 0x80);     		// AOSR = 128
  audio_cmd_write_cmd(AIC32X4_ADCSPB, 0x04); // PRB_R4 for ADC (default recording)

//	audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);			// Page 1
//	audio_cmd_write_cmd(AIC32X4_ADCPWTUNE, 0x00);		// Select ADC PTM_R4
//
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);		// Page 1
//  audio_cmd_write_cmd(AIC32X4_MICBIAS, 0x40);		// MIC BIAS power-up
//  audio_cmd_write_cmd(AIC32X4_INPWRCTRL, 0x31);  // MicPGA startup delay ~3.1 ms
//  audio_cmd_write_cmd(AIC32X4_RMIXAMPL, 0x00); // Mixer Amplifier Right Volume Control

//  audio_cmd_write_cmd(AIC32X4_LMICPGANIN, 0x80); // Route CM to LEFT_N with 20K input impedance
//  audio_cmd_write_cmd(AIC32X4_RMICPGANIN, 0x80); // Route CM to RIGHT_N with 20K input impedance

  audio_cmd_write_cmd(AIC32X4_RMICPGAPIN, 0x08); // Route IN3R to RIGHT_P with 20K input impedance
  audio_cmd_write_cmd(AIC32X4_RMICPGANIN, 0x80); // Route CM1 to RIGHT_N with 20K input impedance

//  audio_cmd_write_cmd(AIC32X4_LMICPGAVOL, 0x0C);  // Left MicPGA enable, +6 dB
  audio_cmd_write_cmd(AIC32X4_RMICPGAVOL, 0x0C);  // Right MicPGA enable, +6 dB

//    audio_cmd_write_cmd(AIC32X4_LMIXAMPL, 0x00);  		// Mixer Amplifier Left Volume Control, 0 dB
//    audio_cmd_write_cmd(AIC32X4_RMIXAMPL, 0x00);		// Mixer Amplifier Right Volume Control, 0 dB

  audio_cmd_write_cmd(AIC32X4_PSEL, 0x00);		// Page 0
//  audio_cmd_write_cmd(AIC32X4_ADCSETUP, 0xC0); 	// Left + Right ADC powered up
  audio_cmd_write_cmd(AIC32X4_ADCSETUP, 0x40); 	  // Right ADC powered up
  audio_cmd_write_cmd(AIC32X4_ADCMUTE, 0x80);     // Mute both ADCs, gain 0 dB
}

void audio_cmd_send_volume_announc (uint8_t lvl)
{
//    audio_cmd_write_cmd(AIC32X4_PSEL, 0x00);			// Page 0
//	audio_cmd_write_cmd(AIC32X4_LDACVOL, lvl); 			// Left DAC Channel Digital Volume
//	audio_cmd_write_cmd(AIC32X4_RDACVOL, lvl); 			// Right DAC Channel Digital Volume

//	LOG_DEBUG("lvl %d", lvl);

  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);		// Page 1
  audio_cmd_write_cmd(AIC32X4_RMIXAMPL, lvl); // Mixer Amplifier Right Volume Control
}

void audio_cmd_send_volume_dac (uint8_t lvl)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x00);			// Page 0
  audio_cmd_write_cmd(AIC32X4_LDACVOL, lvl); 			// Left DAC Channel Digital Volume
  audio_cmd_write_cmd(AIC32X4_RDACVOL, lvl); 			// Right DAC Channel Digital Volume
}

void audio_cmd_enable_HP (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);		// Page 1
  audio_cmd_write_cmd(AIC32X4_OUTPWRCTL, 0x30); // Power up HPL and HPR drivers
  osDelay(2);
}

void audio_cmd_enable_LO (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);		// Page 1
  audio_cmd_write_cmd(AIC32X4_OUTPWRCTL, 0x0C); // Power up LOL and LOR drivers
  osDelay(2);
}

void audio_cmd_unmute_LO (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);		// Page 1

//    audio_cmd_write_cmd(AIC32X4_DACMUTE, 0x00); 	// Unute both DACs
  audio_cmd_write_cmd(AIC32X4_LOLGAIN, 0x00); // LOL gain 0 dB, driver is not unmuted
  audio_cmd_write_cmd(AIC32X4_LORGAIN, 0x00); // LOR gain 0 dB, driver is not unmuted
}

void audio_cmd_mute_LO (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);		// Page 1

//    audio_cmd_write_cmd(AIC32X4_DACMUTE, 0x0C); 		// Mute both DACs
  audio_cmd_write_cmd(AIC32X4_LOLGAIN, 0x40); // LOL gain 0 dB, driver is muted
  audio_cmd_write_cmd(AIC32X4_LORGAIN, 0x40); // LOR gain 0 dB, driver is muted
}

void audio_cmd_unmute_HP (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);		// Page 1

  audio_cmd_write_cmd(AIC32X4_HPLGAIN, 0x00); // HPL gain 0 dB, driver is not unmuted
  audio_cmd_write_cmd(AIC32X4_HPRGAIN, 0x00); // HPR gain 0 dB, driver is not unmuted
}

void audio_cmd_mute_HP (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);		// Page 1

  audio_cmd_write_cmd(AIC32X4_HPLGAIN, 0x40); // HPL gain 0 dB, driver is muted
  audio_cmd_write_cmd(AIC32X4_HPRGAIN, 0x40); // HPR gain 0 dB, driver is muted
}

void audio_cmd_playback_pwr_up (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);		// Page 1
  audio_cmd_write_cmd(AIC32X4_OUTPWRCTL, 0x3C); // Power up HPL/HPR and LOL/LOR drivers
}

void audio_cmd_playback_pwr_down (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);		// Page 1
  audio_cmd_write_cmd(AIC32X4_OUTPWRCTL, 0x00); // Power down HPL/HPR and LOL/LOR drivers
}

void audio_cmd_playback_enable (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);		// Page 1

  audio_cmd_write_cmd(AIC32X4_LOLROUTE, 0x08); 		// LDAC -> LOL
  audio_cmd_write_cmd(AIC32X4_LORROUTE, 0x08);   	// RDAC -> LOR

  //	audio_cmd_playback_pwr_up();
  audio_cmd_unmute_LO();
  audio_cmd_unmute_HP();
}

void audio_cmd_playback_disable (void)
{
//	audio_cmd_playback_pwr_down();
  audio_cmd_mute_LO();
  audio_cmd_mute_HP();
}

// MICROPHONE
//void audio_cmd_microphone_unmute(void)
//{
//    audio_cmd_write_cmd(AIC32X4_PSEL, 0x00);			// Page 0
//    audio_cmd_write_cmd(AIC32X4_ADCMUTE, 0x00);     	// Unmute both ADCs, gain 0 dB
//}
//
//void audio_cmd_microphone_mute(void)
//{
//    audio_cmd_write_cmd(AIC32X4_PSEL, 0x00);			// Page 0
//    audio_cmd_write_cmd(AIC32X4_ADCMUTE, 0x88);     	// Mute both ADCs, gain 0 dB
//}

//void audio_cmd_microphone_pwr_up(void)
//{
//    audio_cmd_write_cmd(AIC32X4_PSEL, 0x00);			// Page 0
//    audio_cmd_write_cmd(AIC32X4_ADCSETUP, 0xC0); 		// Left + Right ADC powered up
//}
//
//void audio_cmd_microphone_pwr_down(void)
//{
//    audio_cmd_write_cmd(AIC32X4_PSEL, 0x00);			// Page 0
//    audio_cmd_write_cmd(AIC32X4_ADCSETUP, 0x00); 		// Left + Right ADC powered up
//}
//
//void audio_cmd_microphone_enable(void)
//{
//    audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);			// Page 1
//
//    audio_cmd_write_cmd(AIC32X4_LMICPGAPIN, 0x04);		// Route IN3L to LEFT_P with 10K input impedance
//    audio_cmd_write_cmd(AIC32X4_LMICPGANIN, 0x04);		// Route IN3R to LEFT_N with 10K input impedance
//    audio_cmd_write_cmd(AIC32X4_RMICPGAPIN, 0x04);		// Route IN3L to RIGHT_P with 10K input impedance
//    audio_cmd_write_cmd(AIC32X4_RMICPGANIN, 0x04);		// Route IN3R to RIGHT_N with 10K input impedance
//
////    audio_cmd_write_cmd(AIC32X4_HPLROUTE, 0x02);		// MAL -> HPL
////    audio_cmd_write_cmd(AIC32X4_HPRROUTE, 0x02);		// MAR -> HPR
//    audio_cmd_write_cmd(AIC32X4_LOLROUTE, 0x02);		// MAL -> LOL
//    audio_cmd_write_cmd(AIC32X4_LORROUTE, 0x02);		// MAR -> LOR
//
////	audio_cmd_microphone_pwr_up();
//    audio_cmd_unmute_LO();
//}
//
//void audio_cmd_microphone_disable(void)
//{
////	audio_cmd_microphone_pwr_down();
//	audio_cmd_mute_LO();
//}

void audio_cmd_A2D_loopback (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x00);		// Page 0
  audio_cmd_write_cmd(AIC32X4_IFACE3, 0x10);// Stereo ADC output is routed to Stereo DAC input
  audio_cmd_write_cmd(AIC32X4_DACSETUP, 0xF2);// LDAC + RDAC powered, soft stepping is disable. Left DAC data is Mono Mix of Left and Right Channel
  audio_cmd_write_cmd(AIC32X4_ADCMUTE, 0x00);     // Unmute both ADCs, gain 0 dB
}

void audio_cmd_I2S_to_DAC (void)
{
//  audio_cmd_write_cmd(AIC32X4_HPLROUTE, 0x08); 		// LDAC -> HPL

  audio_cmd_write_cmd(AIC32X4_PSEL, 0x00);			// Page 0
  audio_cmd_write_cmd(AIC32X4_IFACE3, 0x00);			// No Loopback
  audio_cmd_write_cmd(AIC32X4_DACSETUP, 0xD6);// LDAC + RDAC powered, soft stepping is disable. LDATA->LDAC and RDATA->RDAC
}

void audio_cmd_IN1L_enable (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);			// Page 1
  audio_cmd_write_cmd(AIC32X4_LMICPGAPIN, 0x80);// Route IN1L to LEFT_P with 20K input impedance

  audio_cmd_write_cmd(AIC32X4_HPLROUTE, 0x04); 		// IN1 -> HPL
  audio_cmd_write_cmd(AIC32X4_HPLROUTE, 0x02); 		// MAL -> HPL
//    audio_cmd_A2D_loopback();
  audio_cmd_unmute_LO();
  audio_cmd_unmute_HP();

}

void audio_cmd_IN2L_enable (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);			// Page 1
  audio_cmd_write_cmd(AIC32X4_LMICPGAPIN, 0x40);// Route IN2L to LEFT_P with 20K input impedance

  audio_cmd_A2D_loopback();
}

void audio_cmd_IN3L_enable (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);			// Page 1
  audio_cmd_write_cmd(AIC32X4_LMICPGAPIN, 0x08);// Route IN3L to LEFT_P with 20K input impedance

  audio_cmd_A2D_loopback();
}

void audio_cmd_IN1R_enable (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);		// Page 1

//  audio_cmd_write_cmd(AIC32X4_OUTPWRCTL, 0x3F); 	// Power up HPL/HPR and LOL/LOR and MAL/MAR
  audio_cmd_write_cmd(AIC32X4_RMICPGAPIN, 0x80);	// Route IN1R to RIGHT_P with 20K input impedance
  audio_cmd_write_cmd(AIC32X4_RMICPGANIN, 0x80); 	// Route CM1 to RIGHT_N with 20K input impedance

  audio_cmd_write_cmd(AIC32X4_HPLROUTE, 0x01); 		// MAR -> HPL
  audio_cmd_write_cmd(AIC32X4_HPRROUTE, 0x02); 		// MAR -> HPR

  audio_cmd_write_cmd(AIC32X4_LOLROUTE, 0x01); 		// LOR -> LOL
  audio_cmd_write_cmd(AIC32X4_LORROUTE, 0x02);   	// MAR -> LOR

  audio_cmd_unmute_LO();
  audio_cmd_unmute_HP();
}

void audio_cmd_IN1R_disable (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);		// Page 1

////  audio_cmd_write_cmd(AIC32X4_OUTPWRCTL, 0x3C); 	// Power up HPL/HPR and LOL/LOR

  audio_cmd_write_cmd(AIC32X4_RMICPGAPIN, 0x08); // Route IN3R to RIGHT_P with 20K input impedance
//  audio_cmd_write_cmd(AIC32X4_RMICPGANIN, 0x80); // Route CM1 to RIGHT_N with 20K input impedance

  audio_cmd_write_cmd(AIC32X4_HPLROUTE, 0x08); 		// LDAC -> HPL
  audio_cmd_write_cmd(AIC32X4_HPRROUTE, 0x08);   	// RDAC -> HPR

  audio_cmd_write_cmd(AIC32X4_LOLROUTE, 0x08); 		// LDAC -> LOL
  audio_cmd_write_cmd(AIC32X4_LORROUTE, 0x08);   	// RDAC -> LOR

  audio_cmd_mute_LO();
  audio_cmd_mute_HP();
}

void audio_cmd_quiet_enable (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);			// Page 1
  audio_cmd_write_cmd(AIC32X4_HPRROUTE, 0x08);   	// RDAC -> HPR
  audio_cmd_write_cmd(AIC32X4_HPRGAIN, 0x00);		 // HPR gain 0 dB, driver is not unmuted
}

void audio_cmd_quiet_disable (void)
{
  audio_cmd_write_cmd(AIC32X4_PSEL, 0x01);		// Page 1
  audio_cmd_write_cmd(AIC32X4_HPRGAIN, 0x40); 		// HPR gain 0 dB, driver is not muted
}
