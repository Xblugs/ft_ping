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
	t0 = ping sent
	t1 = now = ping received
	Return time in ms
*/
unsigned long	timespan_ms(struct timeval *t0)
{
	struct timeval 	t1;
	gettimeofday(&t1, NULL);
	return ((t1.tv_sec - t0->tv_sec) * 1000UL
		+ (t1.tv_usec - t0->tv_usec / 1000UL));
}

/*
	t0 = ping sent
	t1 = now = ping received
	Return time in seconds
*/
unsigned long	timespan(struct timeval *t0)
{
	struct timeval 	t1;
	gettimeofday(&t1, NULL);
	return ((t1.tv_sec - t0->tv_sec));
}
