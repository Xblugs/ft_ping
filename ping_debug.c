/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_debug.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cczerwin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 18:32:18 by cczerwin          #+#    #+#             */
/*   Updated: 2026/08/31 18:32:20 by cczerwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>

#include "ping.h"

/*
	Hub for variouses debug prints
*/

void	address_format_debug(struct sockaddr_in *ip, char *av)
{
	if (inet_pton(AF_INET, av, &ip->sin_addr.s_addr))
		DEBUG_PRINT("found IP! (%s)", av);
	else
		DEBUG_PRINT("found Hostname! (%s = %s)", av, inet_ntoa(ip->sin_addr));
}

/*
	Print the type corresponding to the given type value
*/
void	icmp_type_debug(uint8_t type)
{
  	switch (type)
	{
  		case ICMP_ECHOREPLY:      DEBUG_PRINT("Type: ICMP_ECHOREPLY"); 		break;
  		case ICMP_DEST_UNREACH:   DEBUG_PRINT("Type: ICMP_DEST_UNREACH"); 	break;
  		case ICMP_SOURCE_QUENCH:  DEBUG_PRINT("Type: ICMP_SOURCE_QUENCH"); 	break;
  		case ICMP_REDIRECT:       DEBUG_PRINT("Type: ICMP_REDIRECT"); 		break;
  		case ICMP_ECHO:           DEBUG_PRINT("Type: ICMP_ECHO"); 			break;
  		case ICMP_TIME_EXCEEDED:  DEBUG_PRINT("Type: ICMP_TIME_EXCEEDED"); 	break;
  		case ICMP_PARAMETERPROB:  DEBUG_PRINT("Type: ICMP_PARAMETERPROB"); 	break;
  		case ICMP_TIMESTAMP:      DEBUG_PRINT("Type: ICMP_TIMESTAMP"); 		break;
  		case ICMP_TIMESTAMPREPLY: DEBUG_PRINT("Type: ICMP_TIMESTAMPREPLY"); break;
  		case ICMP_INFO_REQUEST:   DEBUG_PRINT("Type: ICMP_INFO_REQUEST"); 	break;
  		case ICMP_INFO_REPLY:     DEBUG_PRINT("Type: ICMP_INFO_REPLY"); 	break;
  		case ICMP_ADDRESS:        DEBUG_PRINT("Type: ICMP_ADDRESS"); 		break;
  		case ICMP_ADDRESSREPLY:   DEBUG_PRINT("Type: ICMP_ADDRESSREPLY"); 	break;
  		default:                  DEBUG_PRINT("Type: <0x%02x>", type); 		break;
  	}
}

void flag_type_debug(t_ping *data)
{
	if (!FLAG(NO_FLAG))
	{
		DEBUG_PRINT("No flags!");
		return;
	}
	if (FLAG(FLOOD_FLAG))
		DEBUG_PRINT("Flood flag");
	if (FLAG(LOAD_FLAG))
		DEBUG_PRINT("Load flag");
	if (FLAG(NUMERIC_FLAG))
		DEBUG_PRINT("Numeric flag");
	if (FLAG(PATTERN_FLAG))
		DEBUG_PRINT("Pattern flag");
	if (FLAG(IGN_ROUTE_FLAG))
		DEBUG_PRINT("Ignore routing flag");
	if (FLAG(SIZE_FLAG))
		DEBUG_PRINT("Size flag");
	if (FLAG(STOP_FLAG))
		DEBUG_PRINT("Stop/Timeout flag");
	if (FLAG(WAIT_FLAG))
		DEBUG_PRINT("Wait/Linger flag");
	if (FLAG(TTL_FLAG))
		DEBUG_PRINT("TTL flag");
	if (FLAG(TYPE_FLAG))
		DEBUG_PRINT("Type/ToS flag");
	if (FLAG(TIMESTAMP_FLAG))
		DEBUG_PRINT("IP timestamp flag");
	if (FLAG(VERBOSE_FLAG))
		DEBUG_PRINT("Verbose flag");
	if (FLAG(COUNT_FLAG))
		DEBUG_PRINT("Count flag");
	if (FLAG(ROOT_FLAG))
		DEBUG_PRINT("Root flag");
	if (FLAG(INTERVAL_FLAG))
		DEBUG_PRINT("Interval flag");
}
