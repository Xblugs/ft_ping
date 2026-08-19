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

	struct addrinfo hint = (struct addrinfo){0}, *res = NULL, *rp = NULL;
	hint.ai_family = AF_INET;
	int test = getaddrinfo(av[1], NULL, &hint, &res);
	if (test)
	{
		printf("Addr error: %s\n", gai_strerror(test));
		return (1);
	}
	int	sfd = 0;
	for (rp = res; rp != NULL; rp = rp->ai_next)
	{
		printf("%p %s\n", rp, rp->ai_addr->sa_data);
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;                  /* Success */

		close(sfd);
	}
	freeaddrinfo(res);
	if (rp == NULL)
		printf("Not found :c\n");
	printf("We in boss\n");
	close(sfd);

	data->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); // require root
	if (data->sockfd > 0)
		data->flags |= ROOT_FLAG;
	else
		data->sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
	if (data->sockfd < 0)
	{
		printf("%s", MSG_MISS_HOST);
		return (1);
	}

	// bind + accept?
	// do stuff
	// alarm()

	// if closing a fd fail, pray your OS will do the cleanup
	shutdown(data->sockfd, SHUT_RDWR);
	close(data->sockfd);
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
