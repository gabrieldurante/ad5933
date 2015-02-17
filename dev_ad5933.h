#ifndef __DEV_AD5933_H__
#define __DEV_AD5933_H__

/* Copyright (C) 
 * 2014 - Gabriel Durante
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */
 
#include <config.h>
#include <common.h>

 /**
 * @file dev_ad5933.h 
 *
 * @brief Implementation Routines of the AD5933 Complex Impedance Meter 
 *
 */
 
/* AD5933 commands definitions */
#define AD5933_BLOCK_WR 0xa0				//Block Write Command
#define AD5933_BLOCK_RD 0xa1				//Block Read Command
#define AD5933_ADDR_PTR 0xb0				//Address Pointer

#define AD5933_INIT 0x10					//Initialize with start Freq
#define AD5933_SWEEP 0x20					//Start Frequency Sweep
#define AD5933_INCFREQ 0x30					//Increment Frequency
#define AD5933_REPEAT_FREQ 0x40				//Repeat Frequency
#define AD5933_MEASURE_TEMP 0x90			//Measure Temperature
#define AD5933_PWR_DWN 0xa0					//Power down mode
#define AD5933_STANDBY 0xb0					//Standby mode
#define AD5933_RESET 0x10					//Reset Command

#define AD5933_VRANGE_200mV 0x02 			//Output Voltage range 200mV
#define AD5933_VRANGE_400mV 0x04 			//Output Voltage range 400mV
#define AD5933_VRANGE_1V 0x06				//Output Voltage range 1V
#define AD5933_VRANGE_2V 0x00				//Output Voltage range 2V

#define AD5933_PGA_1X 0x01					//PGA gain x1
#define AD5933_PGA_5X 0x00					//PGA gain x5

#define AD5933_BASE_CFG (AD5933_VRANGE_2V | AD5933_PGA_1X) //2Vpp and PGA x1

#define AD5933_INT_OSC_FREQ_RATIO 32.002319	//Internal MCLK = 16.776 MHz
#define AD5933_EXT_OSC_FREQ_RATIO 33.554432	//External MCLK = 16.000 MHz

/* AD5933 status definitions */
#define AD5933_STAT_TEMP_VALID 0x01
#define AD5933_STAT_DATA_VALID 0x02
#define AD5933_STAT_SWEEP_DONE 0x04

/* AD5933 registers definitions */
#define AD5933_CTRL_HIGH 0x80 			//RW 2 bytes
#define AD5933_CTRL_LOW 0x81			//RW 2 bytes
#define AD5933_FREQ_HIGH 0x82			//RW 3 bytes			
#define AD5933_FREQ_MID 0x83
#define AD5933_FREQ_LOW 0x84
#define AD5933_FREQ_INC_HIGH 0x85		//RW 3 bytes
#define AD5933_FREQ_INC_MID 0x86
#define AD5933_FREQ_INC_LOW 0x87		
#define AD5933_NUM_INC_HIGH 0x88		//RW 2 bytes, 9 bit
#define AD5933_NUM_INC_LOW 0x89
#define AD5933_NUM_SETTLE_HIGH 0x8a		//RW 2 bytes
#define AD5933_NUM_SETTLE_LOW 0x8b
#define AD5933_STATUS 0x8f				//R 1 byte
#define AD5933_TEMP_HIGH 0x92			//R 2 bytes
#define AD5933_TEMP_LOW 0x93
#define AD5933_REAL_HIGH 0x94			//R 2 bytes
#define AD5933_REAL_LOW 0x95	
#define AD5933_IMAG_HIGH 0x96			//R 2 bytes
#define AD5933_IMAG_LOW 0x97   

/* Max buffer size from AD5933 registers - 3 bytes */
#define AD5933_DATA_BUFFER_SIZE 3

/* Number of samples to av*/

/* SF1 ctrl I/O port */
#define AD5933_IO_PORT PORTD6

/**
 * @brief available flags used by the AD5933 API
 */
typedef enum _e_ad5933_flags {

	E_FLAGS_AD5933_START_MEASURE = 0x01,
	E_FLAGS_AD5933_STOP_MEASURE,
	E_FLAGS_AD5933_FREQUENCY_SWEEP_NEXT,
	E_FLAGS_AD5933_FREQUENCY_REPEAT,
	E_FLAGS_AD5933_DFT_COMPLETE,
	E_FLAGS_AD5933_IDLE
	
} e_ad5933_flags;

/**
 * @brief AD5933 data structure
 */
typedef struct _ad5933_platform_data {
	
	// imaginary data
	long data_imaginary;
	
	// real data
	long data_real;
	
	// temperature data
	char temperature;
	
	// start frequency data
	unsigned long frequency_start;
	
	// delta frequency data
	unsigned long delta_frequency;
	
	// sweep number of increments
	unsigned char number_of_increments;
	
	// delay data
	unsigned char delay_value;
	
	// measure trigger
	unsigned char measure_trigger;

} ad5933_platform_data;

/**
 * @brief initialize AD5933
 */
volatile ad5933_platform_data* ad5933_init(void);

/**
 * @brief Set AD5933 pointer register address
 *
 * @param a_reg_loc a register pointer
 *
 */
void ad5933_set_pointer( unsigned char a_reg_loc );

/**
 * @brief Write a byte to AD5933
 *
 * @param a_reg_addr a register address
 * @param a_data a data byte
 *
 */
void ad5933_write_byte( unsigned char a_reg_addr, unsigned char a_data );

/**
 * @brief Write a byte block to AD5933
 *
 * @param a_reg_loc a register pointer
 * @param a_byte_num a data size
 * @param a_data_p a data pointer
 *
 */
void ad5933_write_block( unsigned char a_reg_loc, unsigned char a_byte_num, unsigned char* a_data_p );

/**
 * @brief Read a byte from AD5933
 *
 * @param a_reg_loc a register pointer
 * @param a_data a data byte
 *
 */
unsigned char ad5933_read_byte( unsigned char a_reg_loc );

/**
 * @brief Read a byte from AD5933
 *
 * @param a_reg_loc a register location
 * @param a_byte_num a data size
 *
 */
unsigned long ad5933_read_block( unsigned char a_reg_loc, unsigned char a_byte_num );

/**
 * @brief Set AD5933 start frequency and increment
 *
 * @param a_start_freq_hz a start frequency
 * @param a_delta_freq_hz a delta frequency
 * @param a_nof_increments a number of increments
 *
 */
void ad5933_set_frequency( unsigned long int a_start_freq_hz, unsigned long int a_delta_freq_hz, unsigned char a_nof_increments );

/**
 * @brief Get AD5933 temperature
 *
 * @param none
 *
 */
void ad5933_get_temperature( void );

/**
 * @brief Configure AD5933 measurement parameters
 *
 * @param none
 *
 */
void ad5933_config_measure( void );

/**
 * @brief Start AD5933 measurement
 *
 * @param none
 *
 */
void ad5933_proc_data( void );


#endif /* __DEV_AD5933_H__ */

