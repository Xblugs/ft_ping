/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_flags.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cczerwin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 17:07:50 by cczerwin          #+#    #+#             */
/*   Updated: 2026/08/12 17:07:52 by cczerwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

// function in this file
int	flag_wrapper(char *flag, t_ping *data);

// i.e --help
static int	word_flag(char *flag, t_ping *data);
static int	word_flag_am(char *flag, t_ping *data);
static int	word_flag_nz(char *flag, t_ping *data);
static int	word_flag_else(char *flag, t_ping *data);

// i.e -f
static int	single_flag(char *flag, t_ping *data);
static int	single_flag_am(char flag, t_ping *data);
static int	single_flag_nz(char flag, t_ping *data);
static int	single_flag_else(char flag, t_ping *data);

static int	unknown_flag(char *flag, char f);

/*
	This function exist because i wanted to put everything in static
*/
int	flag_wrapper(char *flag, t_ping *data)
{
	if (flag[1] == '-')
		return (word_flag(flag, data));
	else
		return (single_flag(flag, data));
}

/*
	Flag are batched together in an attempt to minimize the comparisons
*/
static int	word_flag(char *flag, t_ping *data)
{
	if (flag[2] >= 'a' && flag[2] <= 'm')
		return (word_flag_am(flag, data));
	else if (flag[2] >= 'n' && flag[2] <= 'z')
		return (word_flag_nz(flag, data));
	else
		return (word_flag_else(flag, data));
}

static int	word_flag_am(char *flag, t_ping *data)
{
	if (strncmp(flag, "--i", 3))
	{
		if (strcmp(flag, "--ignore-routing"))
			data->flags |= IGN_ROUTE_FLAG;
		else if (strncmp(flag, "--ip-timestamp", 15))
			data->flags |= TIMESTAMP_FLAG;
	}
	else if (strcmp(flag, "--flood"))
		data->flags |= FLOOD_FLAG;
	else if (strcmp(flag, "--help"))
		; // call 911 and return 1
	else if (strncmp(flag, "--linger", 9))
		data->flags |= WAIT_FLAG;
	else
		return (unknown_flag(flag, '\0'));
	return (0);
}

static int	word_flag_nz(char *flag, t_ping *data)
{
	if (strncmp(flag, "--t", 3))
	{
		if (strncmp(flag, "--timeout", 10))
			data->flags |= STOP_FLAG;
		else if (strncmp(flag, "--tos", 6))
			data->flags |= TYPE_FLAG;
		else if (strcmp(flag, "--ttl"))
			data->flags |= TTL_FLAG;
	}
	else if (strncmp(flag, "--p", 3))
	{
		if (strncmp(flag, "--pattern", 10))
			data->flags |= PATTERN_FLAG;
		else if (strncmp(flag, "--preload", 10))
			data->flags |= LOAD_FLAG;
	}
	else if (strcmp(flag, "--numeric"))
			data->flags |= NUMERIC_FLAG;
	else if (strncmp(flag, "--size", 7))
		data->flags |= SIZE_FLAG;
	else if (strcmp(flag, "--verbose"))
		data->flags |= VERBOSE_FLAG;
	else
		return (unknown_flag(flag, '\0'));
	return (0);
}

static int	word_flag_else(char *flag, t_ping *data)
{
	(void) data;
	// implementation secrets
	if (strcmp(flag, "--911"))
		; // call 911 and return 1
	else
		return (unknown_flag(flag, '\0'));
	return (0);
}

static int	single_flag(char *flag, t_ping *data)
{
	int	i 	= 1;
	int ret = 0;

	while (flag[i] != 0)
	{
		if (flag[i] >= 'a' && flag[i] <= 'm')
			ret = single_flag_am(flag[i], data);
		else if (flag[i] >= 'n' && flag[i] <= 'z')
			ret = single_flag_nz(flag[i], data);
		else
			ret = single_flag_else(flag[i], data);
		if (ret)
			break;
		i++;
	}
	return (ret);
}

static int	single_flag_am(char flag, t_ping *data)
{
	if (flag == 'f')
		data->flags |= FLOOD_FLAG;
	else if (flag == 'l')
		data->flags |= LOAD_FLAG;
	else
		return (unknown_flag(NULL, flag));
	return (0);
}

static int	single_flag_nz(char flag, t_ping *data)
{
	switch (flag)
	{
	case 'n':
		data->flags |= NUMERIC_FLAG;
		break;

	case 'p':
		data->flags |= PATTERN_FLAG;
		break;

	case 'r':
		data->flags |= IGN_ROUTE_FLAG;
		break;

	case 's':
		data->flags |= SIZE_FLAG;
		break;

	case 'T':
		data->flags |= TYPE_FLAG;
		break;

	case 'w':
		data->flags |= STOP_FLAG;
		break;

	case 'W':
		data->flags |= WAIT_FLAG;
		break;

	case 'v':
		data->flags |= VERBOSE_FLAG;
		break;

	case 'V':
		// exec version
		break;

	default:
		return (unknown_flag(NULL, flag));
		break;
	}
	// in progress
	return (0);
}

static int	single_flag_else(char flag, t_ping *data)
{
	(void) data;
	// implementation secrets
	if (flag == '?')
		; // call 911 and return 1
	else
		return (unknown_flag(NULL, flag));
	return (0);
}

/*
	to be called on unknown flag to build the message
		`-narnia`  --> invalid option 'a'
		`--narnia` --> invalid option '--narnia'
*/
static int	unknown_flag(char *flag, char f)
{
	if (flag != NULL)
		printf("%s '%s'\n", MSG_INVAL_OPT, flag);
	else
		printf("%s '%c'\n", MSG_INVAL_OPT, f);
	return (1);
}