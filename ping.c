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

#include "ping.h"

int	main(int ac, char** av)
{
	if (ac == 1)
	{
		printf("%s", MSG_MISS_HOST);
		return (64);
	}
	t_ping data = (t_ping){0};
	data.ret = ping_init(&ac, av, &data);

	return (data.ret);
}

int	ping_init(int *ac, char **av, t_ping *data)
{
	data->ac = ac;
	data->av = av;
	if(arg_parse(data))
		return (1);
	// data->header.code = 0;
	data->header.type = ECHO_REQUEST;

	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); // require root
	if (sockfd > 0)
		data->flags |= ROOT_FLAG;
	else
		sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
	printf("%d\n", sockfd);
	if (sockfd < 0)
	{
		printf("%s", MSG_MISS_HOST);
		return (1);
	}

	// bind + accept?
	// do stuff

	// if closing a fd fail, pray your OS will do the cleanup
	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);
	return (0);
}

/*
	Retrieve flags and their values
	[-?, -h, --help] don't need to be stored, execute directly
*/
int arg_parse(t_ping *data)
{
	// '-' and '--' are valid inputs
	for (int i = 1; i < *data->ac; i++)
	{
		if (data->av[i][0] != '-' || strlen(data->av[i]) == 1)
			continue;
		flag_wrapper(data->av[i], data);

		if (DEBUG)
			printf("%s\nflag = %#x\n", data->av[i], data->flags);
		if (data->ret)
			break;
	}
	
	return (0);
}
