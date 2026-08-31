/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_receive.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cczerwin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 19:10:50 by cczerwin          #+#    #+#             */
/*   Updated: 2026/08/31 19:10:52 by cczerwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

/*
	Placeholder for the reception part
*/
int	ping_receive(t_ping *data)
{
	(void) data;
	return (0);
}

/*
	Verify the received packet integrity
*/
int	verify_checksum(struct icmphdr *icmp)
{
	uint16_t old;

	old = icmp->checksum;
	icmp->checksum = 0;
	if (checksum(icmp, HEADER_SIZE + PAYLOAD_SIZE) != old)
	{
		if (DEBUG)
			DEBUG_PRINT("checksum not matching, packet dropped");
		return (1);
	}
	return (0);
}
