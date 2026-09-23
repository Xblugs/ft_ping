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

/*
	This file handles mostly the prints and error messages
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
	return (64);
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
    printf("  -v, --verbose              Verbose output, display all received ICMP packets\n");
	printf("  -w, --timeout=NUMBER       Stop after NUMBER seconds\n");
	printf("\n");
    
    printf(" Informative output:\n");
    printf("  -V, --version              Display program version and exit\n");
    printf("  -?, --help                 Give this help list\n");
    printf("      --usage                Give a short usage message\n");
	printf("\n");
	return (1);
}

/*
	Print help message
	Returns (1) to reduce the line count in the flag files
*/
int print_version(void) 
{
    printf("%s", MSG_VERSION);
	return (1);
}

/*
	0 < valid timeout <= INT_MAX
*/
int check_timeout_val(t_ping *data)
{
	if (data->ret)
		return (data->ret);

	if (data->flag_bag.timeout_val > INT_MAX)
	{
		fprintf(stderr, "%s%u\n", MSG_VAL_TOO_BIG, data->flag_bag.timeout_val);
		return (1);
	}
	else if (data->flag_bag.timeout_val == 0)
	{
		fprintf(stderr, "%s%u\n", MSG_VAL_TOO_SMALL, data->flag_bag.timeout_val);
		return (1);
	}
	return (0);
}
