#include <stdio.h>
#include <string.h>

#include "file.h"
#include "utils.h"
#include "const.h"
#include "graph.h"
#include "word.h"
#include "dijkstra.h"

int *find_max_perms(FILE *fpal)
{
	int *max_perms;
	char buffer[MAX_WORD_SIZE];
	char word1[MAX_WORD_SIZE], word2[MAX_WORD_SIZE];
	int perm = 0;
	size_t size;

	/* Array cujos índices são os tamanhos de palavra necessários e cujos
	 * valores são o número máximo de permutações para esse tamanho. */
	max_perms = (int *) ecalloc(MAX_WORD_SIZE, sizeof(int));

	/* Ler cada linha do .pal */
	while (fgets(buffer, MAX_WORD_SIZE, fpal) != NULL) {
		sscanf(buffer, "%s %s %d", word1, word2, &perm);
		size = strlen(word1);
		/* Guardar número máximo de permutações */
		if (max_perms[size] < perm) {
			max_perms[size] = perm;
		}
	}

	return max_perms;
}

Graph **read_dic(FILE *fdic, int *max_perms)
{
	char buffer[MAX_WORD_SIZE];
	int num_words[MAX_WORD_SIZE] = {0};
	Graph **graphs;
	size_t size;
	int i;

	/* Ler o dicionário uma primeira vez para saber quantos vértices
	 * de cada tamanho de palavra alocar, para construir os grafos. */
	while (fscanf(fdic, "%s", buffer) == 1) {
		num_words[strlen(buffer)]++;
	}

	rewind(fdic);

	/* Array de MAX_WORD_SIZE grafos, em que apenas alocamos
	 * grafos cujos índices no array correspondem a tamanhos de palavra
	 * que precisamos (os outros ficam a NULL) */
	graphs = (Graph **) emalloc(MAX_WORD_SIZE * sizeof(Graph *));
	for (i = 0; i < MAX_WORD_SIZE; i++) {
		if (max_perms[i] != 0) {
			graphs[i] = g_init(num_words[i], max_perms[i]);
		}
		else {
			graphs[i] = NULL;
		}
	}

	/* Reler o dicionário para construir os grafos. */
	while (fscanf(fdic, "%s", buffer) == 1) {
		size = strlen(buffer);
		/* Ignorar palavras cujos tamanhos já sabemos que não precisamos. */
		if (max_perms[size] != 0) {
			g_insert(graphs[size], w_new(buffer));
			/*TODO: A criação de edges é demasiado lenta, não sei se por estar a
			 * fazer algo estupido ou se por ser uma má forma de abordar o
			 * problema.
			 */
		}
	}

	/* Construir as arestas entre cada palavra, com pesos até o quadrado
	 * do número máximo de permutações para cada tamanho. */
	for (i = 0; i < MAX_WORD_SIZE; i++)
		if (max_perms[i] != 0) {
			g_make_edges(graphs[i], w_diff);
		}

	return graphs;
}

void solve_pal(FILE *fpal, FILE *fpath, Graph **graphs)
{
	Graph *g;
	char word1[MAX_WORD_SIZE], word2[MAX_WORD_SIZE];
	int max_perm;
	int i; /* indíce do vértice de origem do grafo. */
	size_t size;
	int *st = NULL, *wt = NULL;
	int v;

	while (fscanf(fpal, "%s %s %d", word1, word2, &max_perm) == 3) {
		g = graphs[strlen(word1)];
		for (i=0; i<g_get_free(g) &&
			 strcmp(word1, (char *) v_get_item(v_get(g, i))); i++) ;

		st = realloc(st, g_get_free(g) * sizeof(int));
		wt = shortest_path(g, i, st, max_perm);

		/*for (j=0; j<g_get_free(g); j++)
			printf("%d: %d %d\n", j, st[j], wt[j]);*/
		v = v_find(g, word2, w_cmp);
		if (st[v] == -1)
			printf("%s %d\n%s\n", word1, -1, word2);
		else {
			printf("%s %d\n", (char *) v_get_item(v_get(g, i)), wt[v]);
			walk_tree(g, st, wt, st[v]);
			
			printf("%s\n", (char *) v_get_item(v_get(g, v)));
		}
	}
	free(wt);
	free(st);
}

void walk_tree(Graph *g, int *st, int *wt, int dst)
{
	if (st[dst] != -1) {
		walk_tree(g, st, wt, st[dst]);
		printf("%s\n", (char *) v_get_item(v_get(g, dst)));
	}
	return;
}
