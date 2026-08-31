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
