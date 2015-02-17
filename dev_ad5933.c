#include "pca.h"
#include "dev_ad5933.h"

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
 
 /**
 * @brief AD5933 platform data
 */
static ad5933_platform_data g_ad5933_platform_data;


volatile ad5933_platform_data* ad5933_init(void) {

	//Init TWI at 250KHz
	twi_init(E_TWI_SCL_250K);
	
	//ADG849 control -> Default pull-up select 20 ohm resistor, write logic low to select 100K ohm
	DDRD |= _BV(DDD6);
	//PORTD &= ~_BV(AD5933_IO_PORT); //Enable 100K ohms
	PORTD |= _BV(AD5933_IO_PORT); //Enable 20 ohms
	
	//Reset DA5933
	ad5933_write_byte(AD5933_CTRL_LOW, AD5933_RESET);
	
	//Set start frequency 30KHz, delta frequency 1KHz, number of sweeps 10
	ad5933_set_frequency(30000,1000,10);
	
	//Set measure trigger to IDLE
	g_ad5933_platform_data.measure_trigger = E_FLAGS_AD5933_IDLE;
	
	return &g_ad5933_platform_data;
}

void ad5933_set_pointer( unsigned char a_reg_loc ) {

	//Send a Start condition on the bus
	twi_send_start();
	
	//Send address SLA+R/W
 	twi_send_address(SLA_W);
	
	//Write the pointer command code
	twi_send_byte(AD5933_ADDR_PTR);	
	
	//Write a register location
	twi_send_byte(a_reg_loc);
	
	//Send a Stop condition on the bus
	twi_send_stop();
}

void ad5933_write_byte( unsigned char a_reg_addr, unsigned char a_data ) {

	//Send a Start condition on the bus
	twi_send_start();
	
	//Send address SLA+R/W
 	twi_send_address(SLA_W);
	
	//Send register
    twi_send_byte(a_reg_addr);
	
	//Send data
   	twi_send_byte(a_data);
	
	//Send a Stop condition on the bus
   	twi_send_stop();
}

void ad5933_write_block( unsigned char a_reg_loc, unsigned char a_byte_num, unsigned char* a_data_p ) {

	unsigned char i, a_data;
	
	//set the pointer location
	ad5933_set_pointer(a_reg_loc);	
	
	//Send a Start condition on the bus
	twi_send_start();
	
	//Send address SLA+R/W
 	twi_send_address(SLA_W);
	
	//Block write command code
	twi_send_byte(AD5933_BLOCK_WR);
	
	//Num of data to be sent
	twi_send_byte(a_byte_num);	
    
	//Send the data bytes
	for(i = 0; i < a_byte_num; i++) {
		a_data = *(a_data_p+i);
		twi_send_byte(a_data);					
	}
	
	//Send a Stop condition on the bus
	twi_send_stop();
}

unsigned char ad5933_read_byte( unsigned char a_reg_loc ) {

	//set the pointer location
	ad5933_set_pointer(a_reg_loc);	
	
	//Send a Start condition on the bus
	twi_send_start();
	
	//Send address SLA+R/W
 	twi_send_address(SLA_R);	
	
	//Clear int flag to send byte 
	TWCR = _BV(TWINT) | _BV(TWEN);
	
	//Wait for TWI interrupt flag set
	twi_wait_interrupt(); 	
	
	//Send a Stop condition on the bus
	twi_send_stop();
	
	return TWDR;
}

unsigned long ad5933_read_block( unsigned char a_reg_loc, unsigned char a_byte_num ) {

	unsigned char i;
	unsigned long a_data_buf[AD5933_DATA_BUFFER_SIZE];
	unsigned long a_data;
	
	//Set the pointer location
	ad5933_set_pointer(a_reg_loc);	
    
	//Send a Start condition on the bus
	twi_send_start();
	
	//Send address SLA+R/W
	twi_send_address(SLA_W);
	
	//Block read command
 	twi_send_byte(AD5933_BLOCK_RD);	
	
	//Num of data to be received
 	twi_send_byte(a_byte_num);
 	
	//Send a Start condition on the bus
 	twi_send_start();
	
	//Send address SLA+R/W
 	twi_send_address(SLA_R);
	
	//Receive all the bytes
 	for(i = 0;i < a_byte_num;i++) {
		
		//Clear int flag and enable acknowledge to receive data
		TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN);
		
		//Wait for TWI interrupt flag set
		twi_wait_interrupt();
		
		a_data_buf[i]=TWDR;
	}
	
	//Clear int flag to send byte
	TWCR = _BV(TWINT) | _BV(TWEN);	

	//Wait for TWI interrupt flag set
	twi_wait_interrupt();		

	//Save Last byte
	a_data_buf[i]=TWDR;		
	
	//Send a Stop condition on the bus
 	twi_send_stop();
	
	//Reassemble data
	switch(a_byte_num){
	case 2:
		a_data = ((a_data_buf[0] << 8) | (a_data_buf[1]));
		break;
	case 3:
		a_data = ((a_data_buf[0] << 16) | (a_data_buf[1] << 8) | (a_data_buf[2]));
	default:
		a_data = (a_data_buf[0]);
		break;
	}

    //Return data
 	return a_data;
}

void ad5933_get_temperature( void ) {

	unsigned char reg_val;
	short temperature;

	ad5933_write_byte(AD5933_CTRL_HIGH, AD5933_MEASURE_TEMP);
	
	reg_val = ad5933_read_byte(AD5933_STATUS);
		
	while((reg_val & AD5933_STAT_TEMP_VALID) != AD5933_STAT_TEMP_VALID); // Wait temperature trigger

	temperature = ad5933_read_block(AD5933_TEMP_HIGH,2);
	
	if(temperature < 8192) {
        temperature /= 32;
    }
    else {
        temperature -= 16384;
        temperature /= 32;
    }
	
	g_ad5933_platform_data.temperature = (char)temperature;
}

void ad5933_set_frequency( unsigned long int a_start_freq_hz, unsigned long int a_delta_freq_hz, unsigned char a_nof_increments ) {

	g_ad5933_platform_data.frequency_start = (a_start_freq_hz * AD5933_INT_OSC_FREQ_RATIO);
	g_ad5933_platform_data.delta_frequency = (a_delta_freq_hz * AD5933_INT_OSC_FREQ_RATIO);
	g_ad5933_platform_data.number_of_increments = a_nof_increments;
	g_ad5933_platform_data.delay_value = ((a_start_freq_hz + (a_delta_freq_hz * a_nof_increments))/1000);
}

void ad5933_config_measure( void ) {

	unsigned char a_data_buf[AD5933_DATA_BUFFER_SIZE];
	
	_delay_ms(150);
	
	//Convert start frequency data to register map
	a_data_buf[0]=(0x000000ff & (g_ad5933_platform_data.frequency_start>>16));
	a_data_buf[1]=(0x000000ff & (g_ad5933_platform_data.frequency_start>>8));
	a_data_buf[2]=(0x000000ff & g_ad5933_platform_data.frequency_start);
	
	//Program start frequency
	ad5933_write_block(AD5933_FREQ_HIGH, 3, &a_data_buf[0]);
	
	//Convert delta frequency data to register map
	a_data_buf[0]=(0x000000ff & (g_ad5933_platform_data.delta_frequency>>16));
	a_data_buf[1]=(0x000000ff & (g_ad5933_platform_data.delta_frequency>>8));
	a_data_buf[2]=(0x000000ff & g_ad5933_platform_data.delta_frequency);
	
	//Program delta frequency
	ad5933_write_block(AD5933_FREQ_INC_HIGH, 3, &a_data_buf[0]);
	
	//Convert number of increments data to register map
	a_data_buf[0]=(0x000000ff & (g_ad5933_platform_data.number_of_increments>>8));
	a_data_buf[1]=(0x000000ff & g_ad5933_platform_data.number_of_increments);
	
	//Program number of increments
	ad5933_write_block(AD5933_NUM_INC_HIGH, 2, &a_data_buf[0]);
	
	//Convert delay data to register map
	a_data_buf[0]=(0x000000ff & (g_ad5933_platform_data.delay_value>>8));
	a_data_buf[1]=(0x000000ff & g_ad5933_platform_data.delay_value);
	
	//Program delay
	ad5933_write_block(AD5933_NUM_SETTLE_HIGH, 2, &a_data_buf[0]);
	
	//Place AD5933 in Stand-by mode
	ad5933_write_byte(AD5933_CTRL_HIGH, AD5933_BASE_CFG|AD5933_STANDBY);
	
	//Set measure trigger to IDLE
	g_ad5933_platform_data.measure_trigger = E_FLAGS_AD5933_IDLE;
}

void ad5933_proc_data( void ) {

	unsigned char reg_val = 0;
	
	//Start measure
	if(g_ad5933_platform_data.measure_trigger == E_FLAGS_AD5933_START_MEASURE) {
		
		//Init AD5933 with Start frequency, 2Vpp and PGA x1
		ad5933_write_byte(AD5933_CTRL_HIGH, AD5933_BASE_CFG|AD5933_INIT);

		//Wait several ms
		_delay_ms(300);
	
		//Send a frequency sweep command
		ad5933_write_byte(AD5933_CTRL_HIGH, AD5933_BASE_CFG|AD5933_SWEEP);
		
		reg_val = 0;
		
		//Check for DFT conversion
		while((reg_val & AD5933_STAT_DATA_VALID) != AD5933_STAT_DATA_VALID) {
			reg_val = ad5933_read_byte(AD5933_STATUS);
		}
	
		//Read Imaginary and Real registers
		g_ad5933_platform_data.data_imaginary = ad5933_read_block(AD5933_IMAG_HIGH, 2);
		g_ad5933_platform_data.data_real = ad5933_read_block(AD5933_REAL_HIGH, 2);
		
		//Convert 16 bit 2's complement format data to decimal value
		if (g_ad5933_platform_data.data_real > 0x7fff) {
		
			//Negative data
			g_ad5933_platform_data.data_real = (0x10000 - g_ad5933_platform_data.data_real) * -1;
		}
		
		if (g_ad5933_platform_data.data_imaginary > 0x7fff) {
		
			//Negative data
			g_ad5933_platform_data.data_imaginary = (0x10000 - g_ad5933_platform_data.data_imaginary) * -1;
		}
		
		//Send a standby command
		ad5933_write_byte(AD5933_CTRL_HIGH, AD5933_BASE_CFG|AD5933_STANDBY);
		
		//Set trigger to next Sweep frequency
		g_ad5933_platform_data.measure_trigger = E_FLAGS_AD5933_DFT_COMPLETE;
	}
	
	//Increment frequency
	if(g_ad5933_platform_data.measure_trigger == E_FLAGS_AD5933_FREQUENCY_SWEEP_NEXT) {
	
		//Generate next frequency
		ad5933_write_byte(AD5933_CTRL_HIGH, AD5933_BASE_CFG|AD5933_INCFREQ);
		
		//Set trigger to Start a new measure
		g_ad5933_platform_data.measure_trigger = E_FLAGS_AD5933_START_MEASURE;
	}
	
	//Repeat frequency
	if(g_ad5933_platform_data.measure_trigger == E_FLAGS_AD5933_FREQUENCY_REPEAT) {
	
		//Generate next frequency
		ad5933_write_byte(AD5933_CTRL_HIGH, AD5933_BASE_CFG|AD5933_REPEAT_FREQ);
		
		//Set trigger to Start a new measure
		g_ad5933_platform_data.measure_trigger = E_FLAGS_AD5933_START_MEASURE;
	}
	
	//Check if sweep is done
	if((reg_val & AD5933_STAT_SWEEP_DONE) == AD5933_STAT_SWEEP_DONE) {
	
		//Set trigger to stop mode
		g_ad5933_platform_data.measure_trigger = E_FLAGS_AD5933_STOP_MEASURE;
	}
	
	//Stop measure process
	if(g_ad5933_platform_data.measure_trigger == E_FLAGS_AD5933_STOP_MEASURE) {
	
		//Power down the part
		ad5933_write_byte(AD5933_CTRL_HIGH, AD5933_BASE_CFG|AD5933_PWR_DWN);
		
		//Set trigger to stop mode
		g_ad5933_platform_data.measure_trigger = E_FLAGS_AD5933_IDLE;
	}
}

