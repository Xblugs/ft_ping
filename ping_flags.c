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
void		flag_wrapper(char *flag, t_ping *data);

// i.e --help
static void	word_flag(char *flag, t_ping *data);
static void	word_flag_am(char *flag, t_ping *data);
static void	word_flag_nz(char *flag, t_ping *data);
static void	word_flag_else(char *flag, t_ping *data);

// i.e -f
static void	single_flag(char *flag, t_ping *data);
static void	single_flag_am(char *flag, t_ping *data);
static void	single_flag_nz(char *flag, t_ping *data);
static void	single_flag_else(char *flag, t_ping *data);

static int	unknown_flag(char *flag, char f);

/*
	This function exist because i wanted to put everything in static
*/
void	flag_wrapper(char *flag, t_ping *data)
{
	if (flag[1] == '-')
		word_flag(flag, data);
	else
		single_flag(flag, data);
}

/*
	Flag are batched together in an attempt to minimize the comparisons
*/
static void	word_flag(char *flag, t_ping *data)
{
	if (flag[2] >= 'a' && flag[2] <= 'm')
		word_flag_am(flag, data);
	else if (flag[2] >= 'n' && flag[2] <= 'z')
		word_flag_nz(flag, data);
	else
		word_flag_else(flag, data);
}

static void	word_flag_am(char *flag, t_ping *data)
{
	if (strncmp(flag, "--i", 3) == 0)
	{
		if (strcmp(flag, "--ignore-routing") == 0)
			data->flags |= IGN_ROUTE_FLAG;
		else if (strncmp(flag, "--ip-timestamp", 15) == 0)
			data->flags |= TIMESTAMP_FLAG;
	}
	else if (strncmp(flag, "--count", 8) == 0)
		data->flags |= COUNT_FLAG;
	else if (strcmp(flag, "--flood") == 0)
		data->flags |= FLOOD_FLAG;
	else if (strcmp(flag, "--help") == 0)
		// call help here
		data->ret = 1;
	else if (strncmp(flag, "--linger", 9) == 0)
		data->flags |= WAIT_FLAG;
	else
		data->ret = unknown_flag(flag, '\0');
}

static void	word_flag_nz(char *flag, t_ping *data)
{
	if (strncmp(flag, "--t", 3) == 0)
	{
		if (strncmp(flag, "--timeout", 10) == 0)
			data->flags |= STOP_FLAG;
		else if (strncmp(flag, "--tos", 6) == 0)
			data->flags |= TYPE_FLAG;
		else if (strcmp(flag, "--ttl") == 0)
			data->flags |= TTL_FLAG;
	}
	else if (strncmp(flag, "--p", 3) == 0)
	{
		if (strncmp(flag, "--pattern", 10) == 0)
			data->flags |= PATTERN_FLAG;
		else if (strncmp(flag, "--preload", 10) == 0)
			data->flags |= LOAD_FLAG;
	}
	else if (strcmp(flag, "--numeric") == 0)
			data->flags |= NUMERIC_FLAG;
	else if (strncmp(flag, "--size", 7) == 0)
		data->flags |= SIZE_FLAG;
	else if (strcmp(flag, "--verbose") == 0)
		data->flags |= VERBOSE_FLAG;
	else
		data->ret = unknown_flag(flag, '\0');
}

static void	word_flag_else(char *flag, t_ping *data)
{
	// implementation secrets
	if (strcmp(flag, "--911") == 0)
		; // call 911 and return 1
	else
		data->ret = unknown_flag(flag, '\0');
}

/*
	Flag are batched together in an attempt to minimize the comparisons
	For single flags, multiples ones can be given at once
*/
static void	single_flag(char *flag, t_ping *data)
{
	int	i 	= 1;

	while (flag[i] != 0)
	{
		if (flag[i] >= 'a' && flag[i] <= 'm')
			single_flag_am(flag + i, data);
		else if (flag[i] >= 'n' && flag[i] <= 'z')
			single_flag_nz(flag + i, data);
		else
			single_flag_else(flag + i, data);
		if (data->ret > 0)
			break;
		else if (data->ret < 0)
		{
			i -= data->ret;
			data->ret = 0;
		}
		else
			i++;
	}
}

static void	single_flag_am(char *flag, t_ping *data)
{
	switch (flag[0])
	{
	case 'c': // require parameter
		data->flags |= COUNT_FLAG;
		break;

	case 'f':
		data->flags |= FLOOD_FLAG;
		break;

	case 'l': // require parameter
		data->flags |= LOAD_FLAG;
		break;

	default:
		data->ret = unknown_flag(NULL, flag[0]);
		break;
	}
}

static void	single_flag_nz(char *flag, t_ping *data)
{
	switch (flag[0])
	{
	case 'n':
		data->flags |= NUMERIC_FLAG;
		break;

	case 'p': // require parameter
		data->flags |= PATTERN_FLAG;
		break;

	case 'r':
		data->flags |= IGN_ROUTE_FLAG;
		break;

	case 's': // require parameter
		data->flags |= SIZE_FLAG;
		break;

	case 'T': // require parameter
		data->flags |= TYPE_FLAG;
		break;

	case 'w': // require parameter
		data->flags |= STOP_FLAG;
		break;

	case 'W': // require parameter
		data->flags |= WAIT_FLAG;
		break;

	case 'v':
		data->flags |= VERBOSE_FLAG;
		break;

	case 'V':
		// version printing here
		data->ret = 1;
		break;

	default:
		data->ret = unknown_flag(NULL, flag[0]);
		break;
	}
}

static void	single_flag_else(char *flag, t_ping *data)
{
	// implementation secrets
	if (flag[0] == '?')
		data->ret = 1; // call 911 and return 1
	else
		data->ret = unknown_flag(NULL, flag[0]);
}

/*
	to be called on unknown flag to build the message
		`-narnia`  --> invalid option 'a'
		`--narnia` --> invalid option '--narnia'
	Returns 1 (cancel code)
*/
static int	unknown_flag(char *flag, char f)
{
	if (flag != NULL)
		printf("%s '%s'\n", MSG_INVAL_OPT, flag);
	else
		printf("%s '%c'\n", MSG_INVAL_OPT, f);
	return (1);
}
