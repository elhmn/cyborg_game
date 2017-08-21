#include "libft.h"
#include "server.h"
#include "websocket.h"

/*
** Update connection list connection message
*/

void		update_con_list(t_env *env)
{
	int		i;

	i = 0;
	memset(env->con_list, 0, BUFSIZE);
 	strcpy(env->con_list, "ws/list/");
	for (i = 0; i < MAXPLAYER; i++)
	{
		if (i != 0)
			strcat(env->con_list, ";");
		if (env->com_tab[i].sock == -1)
			strcat(env->con_list, "");
		else
			strcat(env->con_list, env->com_tab[i].ip);
	}
	put_comtab(env->com_tab, MAXPLAYER);//_DEBUG_//
	fprintf(stdout, "env.con_list [%s]\n", env->con_list);//_DEBUG_//
}

void		send_con_list(t_env *env)
{
	int		i;

	i = 0;
	if (env)
	{
		for (i = 0; i < MAXPLAYER; i++)
		{
			if (env->com_tab[i].sock == -1)
				continue ;
			memset(env->msg, 0, BUFSIZE);
			strcpy(env->msg, "con:list:");
			strcat(env->msg, env->con_list);
			pipe_com_write(env->ptoc_pipe[i][1], env->msg);

			fprintf(stdout, "send connection list [%d][%s]\n",
					i, env->msg);//_DEBUG_//
		}
	}
}

int			check_con_list(t_env *env, int idx, unsigned char *buftmp)
{
	char				**tab;
	int					ret;

	ret = 0;
	if (!env)
	{
		fprintf(stdout, "Error : env set to NULL!\n");
		return (0);
	}
	if (buftmp)
	{
//  		fprintf(stdout, "check con buftmp = [%s]\n", buftmp);//_DEBUG_//
		tab = ft_strsplit((char*)buftmp, ':');
		if (tab)
		{
// 			show_tab(tab);
			if (!strcmp((const char*)tab[0], "con")
						&& !strcmp((const char*)tab[1], "list"))
			{
				memset(env->lan_msg[idx], 0, BUFSIZE);
				if (tab[2])
					strcpy(env->lan_msg[idx], tab[2]);
				fprintf(stdout, "check con lan_msg[%d] = [%s]\n", idx, env->lan_msg[idx]);//_DEBUG_//
				fprintf(stdout, "connection list sent\n");//_DEBUG_//
				ret = 1;
			}
			free_tab(&tab);
		}
	}
	return (ret);
}

void		send_con_challenger(t_env *env, int state)
{
	char	buf[256];
	int		i;

	i = 0;
	if (env)
	{
		for (i = 0; i < MAXPLAYER; i++)
		{
			if (env->com_tab[i].sock == -1)
				continue ;
			memset(env->msg, 0, BUFSIZE);
			strcpy(env->msg, "con:challenger:");
			strcat(env->msg, "ws/challenger/");
			strcat(env->msg, cvtInt(buf, state));
			pipe_com_write(env->ptoc_pipe[i][1], env->msg);

			fprintf(stdout, "send con challenger [%d][%s]\n",
					i, env->msg);//_DEBUG_//
		}
	}
}

int				check_con_challenger(t_env *env, int idx, unsigned char *buftmp)
{
	char				**tab;
	int					ret;

	ret = 0;
	if (!env)
	{
		fprintf(stdout, "Error : env set to NULL!\n");
		return (0);
	}
	if (buftmp)
	{
//  		fprintf(stdout, "check con buftmp = [%s]\n", buftmp);//_DEBUG_//
		tab = ft_strsplit((char*)buftmp, ':');
		if (tab)
		{
// 			show_tab(tab);
			if (!strcmp((const char*)tab[0], "con")
						&& !strcmp((const char*)tab[1], "challenger"))
			{
				memset(env->lan_msg[idx], 0, BUFSIZE);
				if (tab[2])
					strcpy(env->lan_msg[idx], tab[2]);
				fprintf(stdout, "check con challenger lan_msg[%d] = [%s]\n", idx, env->lan_msg[idx]);//_DEBUG_//
				fprintf(stdout, "connection challenger sent\n");//_DEBUG_//
				ret = 1;
			}
		}
	}
	return (ret);
}

void			send_ws_ready(t_env *env)
{
	int		i;
	int		j;
	char	buf[256];

	i = 0;
	if (env)
	{
		for (i = 0; i < MAXPLAYER; i++)
		{
			if (env->com_tab[i].sock == -1)
				continue ;
			memset(env->msg, 0, BUFSIZE);
			strcpy(env->msg, cvtInt(buf, i));
			strcat(env->msg, "/ws/ready/");
			for (j = 0; j < MAXPLAYER; j++)
			{
				fprintf(stdout, "env->ready = [%d]\n", env->ready[j]);
				strcat(env->msg, cvtInt(buf, env->ready[j]));
				if (j != MAXPLAYER - 1)
					strcat(env->msg, ";");
			}
// 			strcpy(env->msg, (char*)env->rcv_msg[env->rcv_idx]);
			pipe_com_write(env->ptoc_pipe[i][1], env->msg);
			fprintf(stdout, "send ws ready [%d][%s]\n",
					i, env->msg);//_DEBUG_//
		}
	}
}

int				check_ws_ready(t_env *env, int idx, unsigned char *buftmp)
{
	char				**tab;
	int					tmp;
	int					ret;

	ret = 0;
	if (!env)
	{
		fprintf(stdout, "Error : env set to NULL!\n");
		return (0);
	}
//  	fprintf(stdout, "check spe con buftmp = [%s]\n", buftmp);//_DEBUG_//
	if (buftmp)
	{
		tab = ft_strsplit((char*)buftmp, '/');
		if (tab)
		{
// 			show_tab(tab);
			tmp = atoi(tab[0]);
 			if (tmp >= 0
			&& tmp <= MAXPLAYER
			&& !strcmp((const char*)tab[1], "ws")
			&& !strcmp((const char*)tab[2], "ready"))
			{
				memset(env->lan_msg[idx], 0, BUFSIZE);
				strcpy(env->lan_msg[idx], (const char *)buftmp);
				fprintf(stdout, "check ws ready lan_msg[%d] = [%s]\n", idx, env->lan_msg[idx]);//_DEBUG_//
				fprintf(stdout, "ready ws sent\n");//_DEBUG_//
				ret = 1;
			}
			free_tab(&tab);
		}
	}
	return (ret);
}

void			send_ws_allready(t_env *env)
{
	int		i;

	i = 0;
	if (env)
	{
		for (i = 0; i < MAXPLAYER; i++)
		{
			if (env->com_tab[i].sock == -1)
				continue ;
			memset(env->msg, 0, BUFSIZE);
			strcpy(env->msg, "ws/allready");
			pipe_com_write(env->ptoc_pipe[i][1], env->msg);
			fprintf(stdout, "send ws all ready [%d][%s]\n",
					i, env->msg);//_DEBUG_//
		}
	}
}

int				check_ws_allready(t_env *env, int idx, unsigned char *buftmp)
{
	char				**tab;
	int					ret;

	ret = 0;
	if (!env)
	{
		fprintf(stdout, "Error : env set to NULL!\n");
		return (0);
	}
	if (buftmp)
	{
		tab = ft_strsplit((char*)buftmp, '/');
		if (tab)
		{
 			fprintf(stdout, "check spe ws allready buftmp = [%s]\n", buftmp);//_DEBUG_//
// 			show_tab(tab);
 			if (!strcmp((const char*)tab[0], "ws")
			&& !strcmp((const char*)tab[1], "allready"))
			{
				memset(env->lan_msg[idx], 0, BUFSIZE);
				strcpy(env->lan_msg[idx], (const char *)buftmp);
				fprintf(stdout, "check ws all ready lan_msg[%d] = [%s]\n", idx, env->lan_msg[idx]);//_DEBUG_//
				fprintf(stdout, "all ready ws sent\n");//_DEBUG_//
				ret = 1;
			}
			free_tab(&tab);
		}
	}
	return (ret);
}

void			send_ws_start(t_env *env)
{
	char	buf[256];

	if (env)
	{
		if (env->com_tab[env->rcv_idx].sock == -1)
			return ;
		memset(env->msg, 0, BUFSIZE);
		strcpy(env->msg, cvtInt(buf, env->rcv_idx));
		strcat(env->msg, "/ws/limits/");
		strcat(env->msg,  cvtInt(buf, env->min));
		strcat(env->msg, ";");
		strcat(env->msg,  cvtInt(buf, env->max));
// 			strcpy(env->msg, (char*)env->rcv_msg[env->rcv_idx]);
		pipe_com_write(env->ptoc_pipe[env->rcv_idx][1], env->msg);
		fprintf(stdout, "send ws start [%d][%s]\n",
				env->rcv_idx, env->msg);//_DEBUG_//

	}
}

int				check_ws_start(t_env *env, int idx, unsigned char *buftmp)
{
	char				**tab;
	int					tmp;
	int					ret;

	ret = 0;
	if (!env)
	{
		fprintf(stdout, "Error : env set to NULL!\n");
		return (0);
	}
//  	fprintf(stdout, "check spe con buftmp = [%s]\n", buftmp);//_DEBUG_//
	if (buftmp)
	{
		tab = ft_strsplit((char*)buftmp, '/');
		if (tab)
		{
// 			show_tab(tab);
			tmp = atoi(tab[0]);
 			if (tmp >= 0
			&& tmp <= MAXPLAYER
			&& !strcmp((const char*)tab[1], "ws")
			&& !strcmp((const char*)tab[2], "limits"))
			{
				memset(env->lan_msg[idx], 0, BUFSIZE);
				strcpy(env->lan_msg[idx], (const char *)buftmp);
				fprintf(stdout, "check ws start lan_msg[%d] = [%s]\n", idx, env->lan_msg[idx]);//_DEBUG_//
				fprintf(stdout, "start ws sent\n");//_DEBUG_//
				ret = 1;
			}
			free_tab(&tab);
		}
	}
	return (ret);
}

void			send_ws_guess(t_env *env, char **tab)
{
	char	buf[256];
	int		val;

	if (env && tab)
	{
		if (env->com_tab[env->rcv_idx].sock == -1)
			return ;
		memset(env->msg, 0, BUFSIZE);
		strcpy(env->msg, cvtInt(buf, env->rcv_idx));
		if (tab[3])
		{
			val = atoi(tab[3]);
			if (val >= env->min && val <= env->max)
			{
				if (val == env->number)
				{
					strcat(env->msg, "/ws/found");
				}
				else
				{
					strcat(env->msg, "/ws/guess/");
					if (val < env->number)
					{
						strcat(env->msg, "1");
					}
					else if (val > env->number)
					{
						strcat(env->msg, "0");
					}
					strcat(env->msg, "/");//_DEBUG_//
					// cheat
					strcat(env->msg, cvtInt(buf, env->number));//_DEBUG_//
				}
			}
			else
			{
				strcat(env->msg, "/ws/outofrange");
			}
		}
// 			strcpy(env->msg, (char*)env->rcv_msg[env->rcv_idx]);
		pipe_com_write(env->ptoc_pipe[env->rcv_idx][1], env->msg);
		fprintf(stdout, "send ws guess [%d][%s]\n",
				env->rcv_idx, env->msg);//_DEBUG_//

	}
}

int				check_ws_guess(t_env *env, int idx, unsigned char *buftmp)
{
	char				**tab;
	int					tmp;
	int					ret;

	ret = 0;
	if (!env)
	{
		fprintf(stdout, "Error : env set to NULL!\n");
		return (0);
	}
//  	fprintf(stdout, "check spe con buftmp = [%s]\n", buftmp);//_DEBUG_//
	if (buftmp)
	{
		tab = ft_strsplit((char*)buftmp, '/');
		if (tab)
		{
// 			show_tab(tab);
			tmp = atoi(tab[0]);
 			if (tmp >= 0
			&& tmp <= MAXPLAYER
			&& !strcmp((const char*)tab[1], "ws")
			&& (!strcmp((const char*)tab[2], "guess")
				|| !strcmp((const char*)tab[2], "found")
				|| !strcmp((const char*)tab[2], "outofrange")))
			{
				memset(env->lan_msg[idx], 0, BUFSIZE);
				strcpy(env->lan_msg[idx], (const char *)buftmp);
				fprintf(stdout, "check ws guess lan_msg[%d] = [%s]\n", idx, env->lan_msg[idx]);//_DEBUG_//
				fprintf(stdout, "guess ws sent\n");//_DEBUG_//
				ret = 1;
			}
			free_tab(&tab);
		}
	}
	return (ret);
}

void			send_ws_winner(t_env *env, char **tab)
{
	char	buf[256];
	int		i;

	if (env && tab)
	{
		if (env->com_tab[env->rcv_idx].sock == -1)
			return ;
		for (i = 0; i < MAXPLAYER; i++)
		{
			if (env->com_tab[i].sock == -1)
				continue ;
			memset(env->msg, 0, BUFSIZE);
			strcpy(env->msg, "ws/winner/");
			strcat(env->msg, cvtInt(buf, env->rcv_idx));
			strcat(env->msg, ";");
			strcat(env->msg, cvtInt(buf, env->number));
			pipe_com_write(env->ptoc_pipe[i][1], env->msg);
			fprintf(stdout, "send ws winner [%d][%s]\n",
				i, env->msg);//_DEBUG_//
		}
	}
}

int				check_ws_winner(t_env *env, int idx, unsigned char *buftmp)
{
	char				**tab;
	int					ret;

	ret = 0;
	if (!env)
	{
		fprintf(stdout, "Error : env set to NULL!\n");
		return (0);
	}
//  	fprintf(stdout, "check spe con buftmp = [%s]\n", buftmp);//_DEBUG_//
	if (buftmp)
	{
		tab = ft_strsplit((char*)buftmp, '/');
		if (tab)
		{
// 			show_tab(tab);
			if (!strcmp((const char*)tab[0], "ws")
			&& !strcmp((const char*)tab[1], "winner"))
			{
				memset(env->lan_msg[idx], 0, BUFSIZE);
				strcpy(env->lan_msg[idx], (const char *)buftmp);
				fprintf(stdout, "check ws winner lan_msg[%d] = [%s]\n", idx, env->lan_msg[idx]);//_DEBUG_//
				fprintf(stdout, "winner ws sent\n");//_DEBUG_//
				ret = 1;
			}
			free_tab(&tab);
		}
	}
	return (ret);
}
