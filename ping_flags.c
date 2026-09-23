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

// Static section
// void	flag_wrapper(char *flag, t_ping *data);

// int		parse_value_int(char *flag, t_ping *data, char type);
// float 	parse_value_float(char *flag, t_ping *data, char type);
// void	parse_value_hex(char *flag, t_ping *data, char type);

// int		unknown_flag(char *flag, char f);
// int		invalid_argument(char *flag, char f);
// int		invalid_pattern(char *flag, char f);


/*
	This file handles mostly the value parsing and the error messages

	Managed flags: -c, -i, -n, -p, -V
		--count, --interval, --numeric, --pattern
*/

/*
	This function exist because i wanted to put everything in static

	For simplicity sake, argument parameters are only looked up
		if they are bundled with the argument.

	`ping localhost -c 3` is correct for the real ping, not for ft_ping
*/
void	flag_wrapper(char *flag, t_ping *data)
{
	if (flag[1] == '-')
		word_flag(flag, data);		// i.e --count=5
	else
		single_flag(flag, data);	// i.e -c5
}

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
		
	/*
		data->ret is used as a cursor in single flags

		parametric flags don't allow for another flag
			after them in the same string = skip the rest
	*/
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
		
	/*
		data->ret is used as a cursor in single flags

		parametric flags don't allow for another flag
			after them in the same string = skip the rest
	*/
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

	/*
		data->ret is used as a cursor in single flags

		parametric flags don't allow for another flag
			after them in the same string = skip the rest
	*/
	
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

/*
	Your flag is in another castle!

	To be called on unknown flag to build the message
		`-narnia`  --> invalid option 'a'
		`--narnia` --> unrecognized option '--narnia'
	Returns 1 (cancel code)
*/
int	unknown_flag(char *flag, char f)
{
	if (flag != NULL)
		fprintf(stderr, "%s'%s'\n", MSG_INVAL_OPT2, flag);
	else
		fprintf(stderr, "%s'%c'\n", MSG_INVAL_OPT, f);
	fprintf(stderr, "%s", MSG_MORE_INFO);
	return (1);
}

int	invalid_argument(char *flag, char f)
{
	if (flag != NULL)
		fprintf(stderr, "%s'%s'\n", MSG_INVAL_ARG, flag);
	else
		fprintf(stderr, "%s'%c'\n", MSG_INVAL_ARG, f);
	return (1);
}

int	invalid_pattern(char *flag, char f)
{
	if (flag != NULL)
		fprintf(stderr, "%s%s\n", MSG_PATTERN_ERR, flag);
	else
		fprintf(stderr, "%s%c\n", MSG_PATTERN_ERR, f);
	return (1);
}

/*
	Print usage message
	Returns (1) to reduce the line count in the flag files
*/
int print_usage(void) 
{
    printf("Usage: ping [OPTION...] HOST...\n");
    printf("%s", MSG_MORE_INFO);
	return (1);
}

/*
	Print help message
	Returns (1) to reduce the line count in the flag files
*/
int print_help(void)
{
    printf("Usage: ping [OPTION...] HOST...\n");
    printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
    
    printf(" Options controlling ping behavior:\n");
    printf("  -c, --count=NUMBER         Stop after sending NUMBER packets\n");
    printf("  -i, --interval=NUMBER      Wait NUMBER seconds between sending each packet\n");
    printf("  -n, --numeric              Do not resolve host addresses to names\n");
    printf("  -p, --pattern=PATTERN      Fill packet with PATTERN (hex format)\n");
    printf("      --ttl=NUMBER           Set Time to Live (TTL) on outbound packets\n");
    printf("  -v, --verbose              Verbose output, display all received ICMP packets\n\n");
    
    printf(" Informative output:\n");
    printf("  -V, --version              Display program version and exit\n");
    printf("  -?, --help                 Give this help list\n");
    printf("      --usage                Give a short usage message\n\n");
	return (1);
}
