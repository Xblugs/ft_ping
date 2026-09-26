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

// Static section
static int parse_prep(char *flag, t_ping *data, char type);

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
	Brings --option=argument to the argument part for parsing
		and gets rid of arg-less options
*/
static int parse_prep(char *flag, t_ping *data, char type)
{
	int i = 0, j = (type == 's');

	if (type == 'w')
	{
		while (flag[i] != '=' && flag[i] != 0)
			i++;

		// comment to consider no arg as correct input
		if (flag[i] == '=')
			i++;
	}

	if (strlen(&flag[i+j]) == 0)
	{
		if (type == 's')
			MSG_REQ_ARG_S(flag);
		else
			MSG_REQ_ARG_W(flag);
		fprintf(stderr, "%s", MSG_MORE_INFO);
		data->ret = 1;
		return (-1);
	}
	return (i);
}

/*
	data->ret is used as a cursor in single flags

	parametric flags don't allow for another flag
		after them in the same string = skip the rest
*/

int	parse_value_int(char *flag, t_ping *data, char type)
{
	int value = parse_prep(flag, data, type);

	if (value == -1)
		return (-1);
	flag = flag + value;

	for (size_t i = (type == 's'); flag[i] != 0; i++)
	{
		if (!isdigit(flag[i]))
		{
			data->ret = invalid_value(&flag[(type == 's')], &flag[i]);
			return (-1);
		}
	}

	if (type == 's')
	{
		int len = strlen(flag);
		data->ret = -len;
	}
	value = atoi(&flag[(type == 's')]);

	return (value);
}

/*
	Some flags can receive float arguments like -i for exemple
*/
float	parse_value_float(char *flag, t_ping *data, char type)
{
	float value = parse_prep(flag, data, type);

	if (value == -1)
		return (-1);
	flag = flag + (int) value;

	char dot = 0;
	for (size_t i = (type == 's'); flag[i] != 0; i++)
	{
		if (!isdigit(flag[i]))
		{
			if (flag[i] == '.' && !dot)
			{
				dot++;
				continue ;
			}
			data->ret = invalid_value(&flag[(type == 's')], &flag[i]);
			return (-1);
		}
	}
		
	if (type == 's')
	{
		int len = strlen(flag);
		data->ret = -len;
	}
	value = atof(&flag[(type == 's')]);

	return (value);
}

/*
	Used only for the pattern flag
*/
void	parse_value_hex(char *flag, t_ping *data, char type)
{
	if (type == 'w')
		while (flag[0] != '=')
			flag = &flag[1];
	if (strlen(&flag[1]) == 0)
	{
		MSG_REQ_ARG_S(flag);
		data->ret = 1;
		return ;
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
	
	// manage the pattern & eventual padding
	size_t len = strlen(flag);
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
