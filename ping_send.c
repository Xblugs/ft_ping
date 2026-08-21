/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_send.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cczerwin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 17:28:32 by cczerwin          #+#    #+#             */
/*   Updated: 2026/08/21 17:28:33 by cczerwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

int	ping_send()
{

}

/*
	Checksum for error checking

	Taken from https://en.wikipedia.org/wiki/Internet_checksum#Algorithm
*/
uint16_t checksum(void *data, int len)
{
    uint32_t sum = 0;
    uint16_t *ptr = data;
    
    // Sum all 16-bit words
    while (len > 1)
	{
        sum += *ptr++;
        len -= 2;
    }
    
    // Handle odd byte
    if (len == 1)
        sum += *(uint8_t *)ptr;
    
    // Fold 32-bit sum to 16 bits
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    
    return ~sum;  // One's complement
}
