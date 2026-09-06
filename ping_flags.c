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

static int	parse_value_int(char *flag, t_ping *data, char type);
static float parse_value_float(char *flag, t_ping *data, char type);
static void	parse_value_hex(char *flag, t_ping *data, char type);
static int	invalid_argument(char *flag, char f);


/*
	This function exist because i wanted to put everything in static

	For simplicity sake, argument parameters are only looked up
		if they are bundled with the argument.

	`ping localhost -c 3` is correct for the real ping, not for ft_ping
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
	usage exemple `--help` or `--count=3`
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

/*
	Commented values are not implemented yet
*/
static void	word_flag_am(char *flag, t_ping *data)
{
	// if (strncmp(flag, "--i", 3) == 0)
	// {
	// 	if (strcmp(flag, "--ignore-routing") == 0)
	// 		data->flag_bag.flags |= IGN_ROUTE_FLAG;
	// 	else if (strncmp(flag, "--ip-timestamp", 14) == 0)
	// 		data->flag_bag.flags |= TIMESTAMP_FLAG;
	// }
	if (strncmp(flag, "--count", 8) == 0)
	{
		data->flag_bag.flags |= COUNT_FLAG;
	}
	// else if (strcmp(flag, "--flood") == 0)
	// {
	// 	data->flag_bag.flags |= FLOOD_FLAG;
	// }
	else if (strcmp(flag, "--help") == 0)
	{
		// call help here
		data->ret = 1;
	}
	// else if (strncmp(flag, "--linger", 8) == 0)
	// {
	// 	data->flag_bag.flags |= WAIT_FLAG;
	// }
	else
		data->ret = unknown_flag(flag, '\0');
}

/*
	Commented values are not implemented yet
*/
static void	word_flag_nz(char *flag, t_ping *data)
{
	// if (strncmp(flag, "--t", 3) == 0)
	// {
	// 	if (strncmp(flag, "--timeout", 9) == 0)
	// 		data->flag_bag.flags |= STOP_FLAG;
	// 	else if (strncmp(flag, "--tos", 5) == 0)
	// 		data->flag_bag.flags |= TYPE_FLAG;
	// 	else if (strcmp(flag, "--ttl") == 0)
	// 		data->flag_bag.flags |= TTL_FLAG;
	// }
	// else if (strncmp(flag, "--p", 3) == 0)
	// {
	if (strncmp(flag, "--pattern", 9) == 0)
	{
		data->flag_bag.flags |= PATTERN_FLAG;
		parse_value_hex(flag, data, 'w');
	}
	// 	else if (strncmp(flag, "--preload", 9) == 0)
	// 		data->flag_bag.flags |= LOAD_FLAG;
	// }
	else if (strcmp(flag, "--numeric") == 0)
			data->flag_bag.flags |= NUMERIC_FLAG;
	// else if (strncmp(flag, "--size", 7) == 0)
	// 	data->flag_bag.flags |= SIZE_FLAG;
	else if (strcmp(flag, "--verbose") == 0)
		data->flag_bag.flags |= VERBOSE_FLAG;
	else
		data->ret = unknown_flag(flag, '\0');
}

/*
	Commented values are not implemented yet
*/
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

	usage exemple `-f` or `-fc3`, note that `-c3f` is incorrect
*/
static void	single_flag(char *flag, t_ping *data)
{
	int	i = 1;

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

/*
	Commented values are not implemented yet
*/
static void	single_flag_am(char *flag, t_ping *data)
{
	switch (flag[0])
	{
	case 'c': // require parameter
		data->flag_bag.flags |= COUNT_FLAG;
		data->flag_bag.count_val = parse_value_int(flag, data, 's');
		break;

	// case 'f':
	// 	data->flag_bag.flags |= FLOOD_FLAG;
	// 	break;

	case 'i':
		data->flag_bag.flags |= INTERVAL_FLAG;
		data->flag_bag.interval = parse_value_float(flag, data, 's') * 1000;
		if (FLAG(INTERVAL_FLAG) && data->flag_bag.interval < 200 )
		{
			fprintf(stderr, "%s", MSG_CNT_FLOOD);
			data->ret = 1;
		}
		break;

	// case 'l': // require parameter
	// 	data->flag_bag.flags |= LOAD_FLAG;
	// 	break;

	default:
		data->ret = unknown_flag(NULL, flag[0]);
		break;
	}
}

/*
	Commented values are not implemented yet
*/
static void	single_flag_nz(char *flag, t_ping *data)
{
	switch (flag[0])
	{
	case 'n':
		data->flag_bag.flags |= NUMERIC_FLAG;
		break;

	case 'p': // require parameter
		data->flag_bag.flags |= PATTERN_FLAG;
		parse_value_hex(flag, data, 's');
		break;

	// case 'r':
	// 	data->flag_bag.flags |= IGN_ROUTE_FLAG;
	// 	break;

	// case 's': // require parameter
	// 	data->flag_bag.flags |= SIZE_FLAG;
	// 	break;

	// case 'T': // require parameter
	// 	data->flag_bag.flags |= TYPE_FLAG;
	// 	break;

	// case 'w': // require parameter
	// 	data->flag_bag.flags |= STOP_FLAG;
	// 	break;

	// case 'W': // require parameter
	// 	data->flag_bag.flags |= WAIT_FLAG;
	// 	break;

	case 'v':
		data->flag_bag.flags |= VERBOSE_FLAG;
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

/*
	Commented values are not implemented yet
*/
static void	single_flag_else(char *flag, t_ping *data)
{
	// implementation secrets
	if (flag[0] == '?')
		data->ret = 1; // call 911 and return 1
	else
		data->ret = unknown_flag(NULL, flag[0]);
}

/*
	Your flag is in another castle!

	To be called on unknown flag to build the message
		`-narnia`  --> invalid option 'a'
		`--narnia` --> invalid option '--narnia'
	Returns 1 (cancel code)
*/
static int	unknown_flag(char *flag, char f)
{
	if (flag != NULL)
		fprintf(stderr, "%s'%s'\n", MSG_INVAL_OPT, flag);
	else
		fprintf(stderr, "%s'%c'\n", MSG_INVAL_OPT, f);
	return (1);
}

static int	parse_value_int(char *flag, t_ping *data, char type)
{
	int value = 0;

	if (type == 'w')
	{

	}
	else // single flag
	{
		for (size_t i = 1; flag[i] != 0; i++)
		{
			if (!isdigit(flag[i]))
			{
				invalid_argument(&flag[i], '\0');
				data->ret = 1;
				return (-1);
			}
		}
		
		/*
			data->ret is used as a cursor in single flags

			parametric flags don't allow for another flag
				after them in the same string = skip the rest
		*/
		int len = strlen(flag);
		data->ret = -len;
		value = atoi(&flag[1]);
	}
	return (value);
}

/*
	Some flags can receive float arguments like -i for exemple
*/
static float	parse_value_float(char *flag, t_ping *data, char type)
{
	float value = 0;

	if (type == 'w')
	{

	}
	else // single flag
	{
		char dot = 0;
		for (size_t i = 1; flag[i] != 0; i++)
		{
			if (!isdigit(flag[i]))
			{
				if (flag[i] == '.' && dot != 2)
				{
					dot++;
					continue ;
				}
				invalid_argument(&flag[i], '\0');
				data->ret = 1;
				return (-1);
			}
		}
		
		/*
			data->ret is used as a cursor in single flags

			parametric flags don't allow for another flag
				after them in the same string = skip the rest
		*/
		int len = strlen(flag);
		data->ret = -len;
		value = atof(&flag[1]);
	}
	return (value);
}

/*
	Used only for the pattern flag
*/
static void	parse_value_hex(char *flag, t_ping *data, char type)
{
	size_t len = strlen(flag);

	// "-p" or "--pattern"
	if (len == 1 || len == 9)
	{
		fprintf(stderr, "%s%s\n", MSG_REQ_ARG, flag);
		data->ret = 1;
		return ;
	}
	if (type == 'w')
	{
		while (flag[0] != '=' && flag[0] != 0)
			flag = &flag[1];
	}
	for (size_t i = 1; flag[i] != 0; i++)
	{
		if (!isxdigit(flag[i]))
		{
			invalid_argument(&flag[i], '\0');
			data->ret = 1;
			return ;
		}
	}
	
	/*
		data->ret is used as a cursor in single flags

		parametric flags don't allow for another flag
			after them in the same string = skip the rest
	*/
	
	if (type == 's')
		data->ret = -len;
	len--;

	// 2 hexa symbol = 1 char --> 0x00 to 0xff
	data->flag_bag.pattern_len = len / 2 + len % 2;
	if (len % 2)
		sscanf(&flag[1], "%1hhx", &data->flag_bag.pattern_val[0]);
	for (size_t i = 0; i < len; i++)
	{
		sscanf(&flag[i * 2 + (len % 2) + 1], "%2hhx", &data->flag_bag.pattern_val[i + (len % 2)]);
	}
	DEBUG_PRINT("%s for %d pattern len [%s]",
		data->flag_bag.pattern_val, data->flag_bag.pattern_len, flag);
}

static int	invalid_argument(char *flag, char f)
{
	if (flag != NULL)
		fprintf(stderr, "%s'%s'\n", MSG_INVAL_ARG, flag);
	else
		fprintf(stderr, "%s'%c'\n", MSG_INVAL_ARG, f);
	return (1);
}
