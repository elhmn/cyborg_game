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

int			check_con_list(t_env *env, int idx)
{
	char				**tab;
	unsigned char		*buftmp;
	int					ret;

	ret = 0;
	if (!env)
	{
		fprintf(stdout, "Error : env set to NULL!\n");
		return (0);
	}
	buftmp = pipe_com_read(env->ptoc_pipe[idx][0]);
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
				memset(env->lan_msg, 0, BUFSIZE);
				if (tab[2])
					strcpy(env->lan_msg, tab[2]);
				fprintf(stdout, "check con lan_msg = [%s]\n", env->lan_msg);//_DEBUG_//
				fprintf(stdout, "connection list sent\n");//_DEBUG_//
				ret = 1;
			}
			free_tab(&tab);
		}
		free(buftmp);
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

int				check_con_challenger(t_env *env, int idx)
{
	char				**tab;
	unsigned char		*buftmp;
	int					ret;

	ret = 0;
	if (!env)
	{
		fprintf(stdout, "Error : env set to NULL!\n");
		return (0);
	}
	buftmp = pipe_com_read(env->ptoc_pipe[idx][0]);
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
				memset(env->lan_msg, 0, BUFSIZE);
				if (tab[2])
					strcpy(env->lan_msg, tab[2]);
				fprintf(stdout, "check con lan_msg = [%s]\n", env->lan_msg);//_DEBUG_//
				fprintf(stdout, "connection challenger sent\n");//_DEBUG_//
				ret = 1;
			}
			free_tab(&tab);
		}
		free(buftmp);
	}
	return (ret);
}
