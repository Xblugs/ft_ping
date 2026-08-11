/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cczerwin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/16 19:56:30 by cczerwin          #+#    #+#             */
/*   Updated: 2026/07/16 19:56:33 by cczerwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
	Lots of include to include here

*/

#include "ping.h"

/*
	put prototypes in a header somewhere later
	and make a struct to handle the data

	todo: think about the structs needed
	req: flags, socket_id, message structure, misc values such as icmp_seq
		timestamp and so on
*/

int	main(int ac, char** av)
{
	if (ac > 2)
		recover_flags(ac, av);
	return (0);
}

/*
	[-?, -h, --help] don't need to be stored, execute directly
*/
int recover_flags(int ac, char** av)
{
	int	index = 0;
	while (index <= ac)
	{
		/* code */
	}

	return (0);
}