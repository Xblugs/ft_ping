/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cczerwin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 16:05:27 by cczerwin          #+#    #+#             */
/*   Updated: 2026/08/21 16:05:28 by cczerwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

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

/*
	t0 = ping sent
	t1 = now
	Return time in ms
*/
double	timespan_ms(struct timeval *t0)
{
	struct timeval 	t1;
	gettimeofday(&t1, NULL);
	return ((double) (t1.tv_sec - t0->tv_sec) * 1000.0
		+ (double) (t1.tv_usec - t0->tv_usec) / 1000.0);
}

/*
	Get a time difference between previous time t0 and current t1
		gettimeofday returns second and microseconds
*/
unsigned long	time_since(struct timeval *t0, struct timeval *t1)
{
	// gettimeofday(t1, NULL);
	return ((t1->tv_sec - t0->tv_sec) * 1000000UL
		+ (t1->tv_usec - t0->tv_usec));
}
