/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_flags_single.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cczerwin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/12 17:07:50 by cczerwin          #+#    #+#             */
/*   Updated: 2026/08/12 17:07:52 by cczerwin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

// Static section
// i.e -f
static void	single_flag_am(char *flag, t_ping *data);
static void	single_flag_nz(char *flag, t_ping *data);
static void	single_flag_else(char *flag, t_ping *data);


/*
	Flag are batched together in an attempt to minimize the comparisons
	For single flags, multiples ones can be given at once

	usage exemple `-f` or `-fc3`, note that `-c3f` is incorrect
*/
void	single_flag(char *flag, t_ping *data)
{
	int	i = 1;

	while (flag[i] != 0)
	{
		if ((flag[i] >= 'a' && flag[i] <= 'm') || (flag[i] >= 'A' && flag[i] <= 'M'))
			single_flag_am(flag + i, data);
		else if ((flag[i] >= 'n' && flag[i] <= 'z') || (flag[i] >= 'N' && flag[i] <= 'Z'))
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
		SETFLAG(COUNT_FLAG);
		data->flag_bag.count_val = parse_value_int(flag, data, 's');
		if (data->flag_bag.count_val == 0)
			data->flag_bag.count_val--;
		break;

	// case 'f':
	// 	data->flag_bag.flags |= FLOOD_FLAG;
	// 	break;

	case 'i':
		SETFLAG(INTERVAL_FLAG);
		data->flag_bag.interval = parse_value_float(flag, data, 's') * 1000;
		if (!FLAG(ROOT_FLAG) && data->flag_bag.interval < 200 )
		{
			fprintf(stderr, "%s%.3f\n", MSG_CNT_FLOOD, (float) data->flag_bag.interval / 1000);
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
		SETFLAG(NUMERIC_FLAG);
		break;

	case 'p': // require parameter
		SETFLAG(PATTERN_FLAG);
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

	case 'w': // require parameter
		SETFLAG(STOP_FLAG);
		data->flag_bag.timeout_val = parse_value_int(flag, data, 's');
		data->ret = check_timeout_val(data);
		break;

	// case 'W': // require parameter
	// 	data->flag_bag.flags |= WAIT_FLAG;
	// 	break;

	case 'v':
		SETFLAG(VERBOSE_FLAG);
		break;
	
	case 'V':
		data->ret = print_version();
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
		data->ret = print_help();
	else
		data->ret = unknown_flag(NULL, flag[0]);
}
