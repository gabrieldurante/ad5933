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

#include <avr/power.h>
#include <util/twi.h> //TWI peripheral status definitions
#include <avr/interrupt.h>
#include "twi.h"


void twi_init(uint8_t a_freq) {

	power_twi_enable();		

	// enable interrupt, twi interface and acknowledge bit
	TWCR = _BV(TWEN);

	_twi_common_frequency_setup(a_freq);
}

void twi_wait_interrupt(void) {

	while(!(TWCR & _BV(TWINT)));
}

unsigned char twi_send_start(void) {
	//Send START
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	
	//Wait for TWI interrupt flag to be set
	twi_wait_interrupt();		

	//Check value of TWI Status Register is different of START or REPEAT START
	if( ((TWSR & 0xF8) != TW_START ) || ((TWSR & 0xF8) != TW_REP_START  ) ) {
		//If it failed, return the TWSR value
		return TWSR;	 	
	}
	
	//If succeeded, return SUCCESS
	return TWI_SUCCESS;		
}

void twi_send_stop(void) {
	//Send a STOP condition
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
}

char twi_send_address(unsigned char adr) {
	
	//Load address value to TWDR register
	TWDR = adr;
	
	//Clear int flag to send byte 
	TWCR = _BV(TWINT) | _BV(TWEN);

	//Wait for TWI interrupt flag set
	twi_wait_interrupt();		

	//Check value of TWI Status Register is different of MASTER TRANSMIT SLAVE ACK or MASTER RECEIVE SLAVE ACK
	if( ((TWSR & 0xF8) != TW_MT_SLA_ACK) || ((TWSR & 0xF8) != TW_MR_SLA_ACK )) {
		//If NACK received return TWSR
		return TWSR;	
	}		
	
	//Else return SUCCESS
	return TWI_SUCCESS;		
}

unsigned char twi_send_byte(unsigned char data) {

	//Load data value to TWDR register
	TWDR = data;
	
	//Clear int flag to send byte
 	TWCR = _BV(TWINT) | _BV(TWEN);  	

	//Wait for TWI interrupt flag set
	twi_wait_interrupt();

	//Check value of TWI Status Register is different of MASTER TRANSMIT DATA ACK
	if( (TWSR & 0xF8) != TW_MT_DATA_ACK ) {
		//If NACK received return TWSR
		return TWSR;	
	}	
																
	return TWI_SUCCESS;		//Else return SUCCESS
}

