#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "server.h"

char 	*cvtInt(char *str, int num)
{
	sprintf(str, "%d", num);
	return (str);
}

/*
** Wrote that shit myself
*/

void			free_tab(char ***tab)
{
	char	**tmp;

	if (tab)
	{
		tmp = *tab;
		show_tab(tmp);
		while (tmp && *tmp)
		{
			free(*tmp);
			tmp++;
		}
 		free(*tab);
 		*tab = NULL;
	}

}

void			show_tab(char **tab)
{
	fprintf(stdout, "show tab : ");
	while (tab && *tab)
	{
		fprintf(stdout, "[%s],", *(tab++));
	}
	fprintf(stdout, "\n");
}
