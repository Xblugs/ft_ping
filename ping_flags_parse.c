/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_flags_parse.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cczerwin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 17:07:50 by cczerwin          #+#    #+#             */
/*   Updated: 2026/08/12 17:07:52 by cczerwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ctype.h>
#include <stdlib.h>

#include "ping.h"

/*
    Those value parsing functions are made using the same blueprint

    If it comes from a word argument (--ttl=42 for exemple)
        They seek the '=' to retrieve the value that comes after

    If it comes from a single char argument (-c3 for exemple)
        They send back in data->ret the negative value 
            representing the increment to skip past the value

    They of course check the validity (int parser check that only digit are present)
    
*/

/*
	data->ret is used as a cursor in single flags

	parametric flags don't allow for another flag
		after them in the same string = skip the rest
*/

int	parse_value_int(char *flag, t_ping *data, char type)
{
	int value = 0;

	if (type == 'w')
		while (*flag != '=')
			flag++;
	if (strlen(&flag[1]) == 0)
	{
		fprintf(stderr, "%s'%s'\n", MSG_REQ_ARG, flag);
		data->ret = 1;
		return (-1);
	}
	for (size_t i = 1; flag[i] != 0; i++)
	{
		if (!isdigit(flag[i]))
		{
			data->ret = invalid_argument(&flag[i], '\0');
			return (-1);
		}
	}

	if (type == 's')
	{
		int len = strlen(flag);
		data->ret = -len;
	}
	value = atoi(&flag[1]);

	return (value);
}

/*
	Some flags can receive float arguments like -i for exemple
*/
float	parse_value_float(char *flag, t_ping *data, char type)
{
	float value = 0;

	if (type == 'w')
		while (*flag != '=')
			flag++;
	if (strlen(&flag[1]) == 0)
	{
		fprintf(stderr, "%s%s\n", MSG_REQ_ARG, flag);
		data->ret = 1;
		return (-1);
	}
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
			data->ret = invalid_argument(&flag[i], '\0');
			return (-1);
		}
	}
		
	if (type == 's')
	{
		int len = strlen(flag);
		data->ret = -len;
	}
	value = atof(&flag[1]);

	return (value);
}

/*
	Used only for the pattern flag
*/
void	parse_value_hex(char *flag, t_ping *data, char type)
{
	size_t len = strlen(flag);

	// "-p" or "--pattern"
	if ((len == 1 && type == 's' )|| (len <= 10 && type == 'w'))
	{
		fprintf(stderr, "%s%s\n", MSG_REQ_ARG, flag);
		data->ret = 1;
		return ;
	}
	if (type == 'w')
	{
		len -= 8;
		while (flag[0] != '=' && flag[0] != 0)
			flag = &flag[1];
	}
	for (size_t i = 1; flag[i] != 0; i++)
	{
		if (!isxdigit(flag[i]))
		{
			invalid_pattern(&flag[i], '\0');
			data->ret = 1;
			return ;
		}
	}
	
	if (type == 's')
		data->ret = -len;
	len--;

	size_t	i = 1, j = 0;
	// 2 hexa symbol = 1 char --> 0x00 to 0xff
	data->flag_bag.pattern_len = len / 2 + len % 2;
	if (len % 2)
	{
		sscanf(&flag[i], "%1hhx", &data->flag_bag.pattern_val[j++]);
		i++;
	}
	while (i < len)
	{
		sscanf(&flag[i], "%2hhx", &data->flag_bag.pattern_val[j++]);
		i += 2;
	}
	DEBUG_PRINT(" [%s] for %d pattern len [%s] ",
		data->flag_bag.pattern_val, data->flag_bag.pattern_len, &flag[1]);
}
