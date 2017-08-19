#include "libft.h"
#include "server.h"
#include "websocket.h"

void				put_comtab(t_com *tab, int s)
{
	int		i;

	for (i = 0; i < s; i++)
	{
		fprintf(stdout, "(%d, %d)", tab[i].pid, tab[i].sock);
		if (i != s - 1)
		{
			fprintf(stdout, "; ");
		}
	}
	fprintf(stdout, "\n");
}

void				init_comtab(t_com *tab, int s)
{
	int		i;

	for (i = 0; i < s; i++)
	{
		tab[i].sock = -1;
		tab[i].pid = -1;
	}
}
