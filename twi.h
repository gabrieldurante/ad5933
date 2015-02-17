#ifndef TWI_H_V8WDZFBC
#define TWI_H_V8WDZFBC

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
 * @file twi.h 
 *
 * @brief API for configure Atmega's TWI peripheral
 *
 * Using the twi interface to communicate with i2c device
 */

#include "config.h"
#include "common.h"
#include "twi_common.h"

/**
 * @brief TWI peripheral messages define
 */
#define TWI_SUCCESS 0xff //Flag
#define SLA_W 0x1a
#define SLA_R 0x1b

/**
 * @brief general TWI interface initialization
 *
 * @param a_freq SCL frequency
 *
 */
void twi_init(uint8_t a_freq);

/**
 * @brief Wait for TWI interrupt flag
 * @param none.
 *
 */
void twi_wait_interrupt(void);

/**
 * @brief Send TWI start condition
 * @param none.
 *
 */
unsigned char twi_send_start(void);

/**
 * @brief Send TWI stop condition
 * @param none.
 *
 */
void twi_send_stop(void);

/**
 * @brief Send TWI address
 * @param adr a data address.
 *
 */
char twi_send_address(unsigned char adr);

/**
 * @brief Send TWI byte
 * @param data a data byte.
 *
 */
unsigned char twi_send_byte(unsigned char data);


#endif /* end of include guard: TWI_H_V8WDZFBC */

