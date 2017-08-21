#include "libft.h"
#include "server.h"
#include "websocket.h"

void				put_comtab(t_com *tab, int sz)
{
	int		i;

	for (i = 0; i < sz; i++)
	{
		fprintf(stdout, "(%d, %d)", tab[i].pid, tab[i].sock);
		if (i != sz - 1)
		{
			fprintf(stdout, "; ");
		}
	}
	fprintf(stdout, "\n");
}

void				init_comtab(t_com *tab, int sz)
{
	int		i;

	for (i = 0; i < sz; i++)
	{
		tab[i].sock = -1;
		tab[i].pid = -1;
	}
}


void				init_env(t_env *env)
{
	if (!env)
	{
		fprintf(stdout, "error : env set to null!");
		exit(EXIT_FAILURE);
	}
	env->ctop_update = 0;
	env->ptoc_update = 0;
	env->state = WAITROOM;
	env->isfull = 0;
	init_comtab(env->com_tab, MAXPLAYER);
	init_tab(env->pts, MAXPLAYER, 0);
	init_tab(env->ready, MAXPLAYER, 0);
	init_tab(env->choice, MAXPLAYER, -1);
}

//Check whether the was a child to parent communication or not
int					isctop(t_env *env)
{
	int		tmp;

	if (!env)
	{
		fprintf(stdout, "error : env set to null!");
		exit(EXIT_FAILURE);
	}
	tmp = env->ctop_update;
	env->ctop_update = !(env->ctop_update);
	return (tmp);
}

//Check whether the was a parent to child communication or not
int					isptoc(t_env *env)
{
	int		tmp;

	if (!env)
	{
		fprintf(stdout, "Error : env set to NULL!");
		exit(EXIT_FAILURE);
	}
	tmp = env->ptoc_update;
	env->ctop_update = !(env->ptoc_update);
	return (tmp);
}

void				set_ptoc(t_env *env)
{
	if (!env)
	{
		fprintf(stdout, "Error : env set to NULL!");
		exit(EXIT_FAILURE);
	}
	env->ptoc_update = 1;
}

void				set_ctop(t_env *env)
{
	if (!env)
	{
		fprintf(stdout, "Error : env set to NULL!");
		exit(EXIT_FAILURE);
	}
	env->ctop_update = 1;
}
