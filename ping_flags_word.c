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
// i.e --help
static void	word_flag_am(char *flag, t_ping *data);
static void	word_flag_nz(char *flag, t_ping *data);
static void	word_flag_else(char *flag, t_ping *data);


/*
	Flag are batched together in an attempt to minimize the comparisons
	usage exemple `--help` or `--count=3`
*/
void	word_flag(char *flag, t_ping *data)
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
	if (strncmp(flag, "--i", 3) == 0)
	{
	// 	if (strcmp(flag, "--ignore-routing") == 0)
	// 		data->flag_bag.flags |= IGN_ROUTE_FLAG;
		if (strncmp(flag, "--interval", 10) == 0)
		{
			if (strlen(flag) <= 11)
			{
				MSG_REQ_ARG_W(flag);
				data->ret = 1;
				return ;
			}
			SETFLAG(INTERVAL_FLAG);
			data->flag_bag.interval = parse_value_float(flag, data, 'w') * 1000;
			if (!FLAG(ROOT_FLAG) && data->flag_bag.interval < 200 )
			{
				fprintf(stderr, "%s", MSG_CNT_FLOOD);
				data->ret = 1;
			}
		}
	// 	else if (strncmp(flag, "--ip-timestamp", 14) == 0)
	// 		data->flag_bag.flags |= TIMESTAMP_FLAG;
	}
	else if (strncmp(flag, "--count", 7) == 0)
	{
		if (strlen(flag) <= 8)
		{
			MSG_REQ_ARG_W(flag);
			data->ret = 1;
			return ;
		}
		SETFLAG(COUNT_FLAG);
		data->flag_bag.count_val = parse_value_int(flag, data, 'w');
		if (data->flag_bag.count_val == 0)
			data->flag_bag.count_val--;
	}
	// else if (strcmp(flag, "--flood") == 0)
	// {
	// 	data->flag_bag.flags |= FLOOD_FLAG;
	// }
	else if (strcmp(flag, "--help") == 0)
		data->ret = print_help();
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
	if (strncmp(flag, "--t", 3) == 0)
	{
	// 	if (strncmp(flag, "--timeout", 9) == 0)
	// 		data->flag_bag.flags |= STOP_FLAG;
	// 	else if (strncmp(flag, "--tos", 5) == 0)
	// 		data->flag_bag.flags |= TYPE_FLAG;
		if (strncmp(flag, "--ttl", 5) == 0)
		{
			if (strlen(flag) <= 6)
			{
				MSG_REQ_ARG_W(flag);
				data->ret = 1;
				return ;
			}
			SETFLAG(TTL_FLAG);
			data->flag_bag.ttl = (char) parse_value_int(flag, data, 'w');
			if (data->flag_bag.ttl > 255)
				fprintf(stderr, "%s%d\n", MSG_VAL_TOO_BIG, data->flag_bag.ttl);
			else if (data->flag_bag.ttl == 0)
				fprintf(stderr, "%s%d\n", MSG_VAL_TOO_SMALL, data->flag_bag.ttl);
		}
	}
	// else if (strncmp(flag, "--p", 3) == 0)
	// {
	else if (strncmp(flag, "--pattern", 9) == 0)
	{
		SETFLAG(PATTERN_FLAG);
		parse_value_hex(flag, data, 'w');
	}
	// 	else if (strncmp(flag, "--preload", 9) == 0)
	// 		data->flag_bag.flags |= LOAD_FLAG;
	// }
	else if (strcmp(flag, "--numeric") == 0)
			SETFLAG(NUMERIC_FLAG);
	// else if (strncmp(flag, "--size", 7) == 0)
	// 	data->flag_bag.flags |= SIZE_FLAG;
	else if (strcmp(flag, "--usage") == 0)
		data->ret = print_usage();
	else if (strcmp(flag, "--verbose") == 0)
		SETFLAG(VERBOSE_FLAG);
	else if (strcmp(flag, "--version") == 0)
		data->ret = print_version();
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
		data->ret = print_help();
	else
		data->ret = unknown_flag(flag, '\0');
}
