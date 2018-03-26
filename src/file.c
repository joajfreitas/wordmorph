/**
 * @file file.c
 * @authors João Pinheiro <joao.castro.pinheiro@tecnico.ulisboa.pt>
 * @authors João Freitas <joao.m.freitas@tecnico.ulisboa.pt>
 * @date 14 Dezembro 2016
 *
 * @brief Funções de I/O.
 * @details
 *	Construir grafos e imprimir caminhos.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "utils.h"
#include "const.h"
#include "graph.h"
#include "word.h"
#include "dijkstra.h"


/**
 * @brief Encontrar número máximo de permutações para cada tamanho de palavra.
 *
 * @param fpal Ficheiro .pal de problemas.
 *
 * @return max_perms Tabela de unsigned shorts, indexada pelos tamanhos
 *	de palavra existentes no .pal e cujos valores são o número máximo de permutações
 *	para esse tamanho de palavra, zero se esse tamanho não existir.
 */
unsigned short *find_max_perms(FILE *fpal)
{
	unsigned short *max_perms;
	char buffer[MAX_WORD_SIZE];
	char word1[MAX_WORD_SIZE], word2[MAX_WORD_SIZE];
	unsigned short perm;
	size_t size;

	/* Inicializar max_perms a zero. */
	max_perms = (unsigned short *) ecalloc(MAX_WORD_SIZE, sizeof(unsigned short));

	/* Ler cada linha do .pal */
	while (fgets(buffer, MAX_WORD_SIZE, fpal) != NULL) {
		sscanf(buffer, "%s %s %hu", word1, word2, &perm);
		size = strlen(word1);
		/* Guardar número máximo de permutações */
		if (max_perms[size] < perm && w_diff(word1, word2, 1) > 1) {
			max_perms[size] = perm;
		}
	}

	return max_perms;
}

/**
 * @brief Construir tabela de grafos com vértices sendo palavras vindas
 *	do dicionário.
 *
 * @param fdic Ficheiro de dicionário.
 * @param max_perms Tabela com número máximo de permutações por tamanho,
 *	vinda de find_max_perms()
 *
 * @return graphs Tabela de grafos, indexada pelo tamanho de palavras
 *	que contém; ou seja, graph[2] corresponde o grafo que contém vértices
 *	(palavras) de tamanho dois. Nos tamanhos não existentes, graph[i] é NULL.
 */
Graph **read_dic(FILE *fdic, unsigned short *max_perms)
{
	char buffer[MAX_WORD_SIZE];
	int num_words[MAX_WORD_SIZE] = {0};
	Graph **graphs;
	size_t size;
	int i;

	/* Ler o dicionário uma primeira vez para saber quantos vértices
	 * de cada tamanho de palavra alocar, para construir os grafos. */
	while (fscanf(fdic, "%s", buffer) == 1) {
		i = strlen(buffer);
		if (max_perms[i] != 0) {
			num_words[strlen(buffer)]++;
		}
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
		}
	}

	/* Construir as arestas entre cada palavra, com pesos até o quadrado
	 * do número máximo de permutações para cada tamanho. */
	for (i = 0; i < MAX_WORD_SIZE; i++) {
		if (max_perms[i] != 0) {
			g_make_edges(graphs[i], w_diff);
		}
	}

	return graphs;
}

/**
 * @brief Encontrar caminho mais curto entre cada duas palavras do ficheiro
 *	de problemas.
 *
 * @param fpal Ficheiro .pal de problemas.
 * @param fpath Ficheiro .path de saída com os caminhos e pesos correspondentes.
 * @param graphs Tabela de grafos.
 */
void solve_pal(FILE *fpal, FILE *fpath, Graph **graphs)
{
	Graph *g; /* Grafo do tamanho de palavra pretendido. */
	char word1[MAX_WORD_SIZE], word2[MAX_WORD_SIZE];
	unsigned short max_perm;
	int src; /* Indíce do vértice de origem do grafo. */
	int dst; /* Indíce do vértice de destino do grafo. */
	int *path = NULL; /* Árvore de caminho. */
	int *dist = NULL; /* Tabela de dipathâncias à origem. */
	int d;

	/* TODO: Lift this while out or lift the body to another function */
	while (fscanf(fpal, "%s %s %hu", word1, word2, &max_perm) == 3) {
		/* Se as duas palavras do .pal diferirem de 1 ou 0 carateres,
		 * a solução é trivial. */
		if ((d = w_diff(word1, word2, 1)) <= 1) {
			fprintf(fpath, "%s %d\n%s\n\n", word1, d, word2);
			continue;
		}

		/* Senão, temos de correr o algoritmo de caminho mais curto. */
		g = graphs[strlen(word1)];
		src = g_find_vertex(g, word1, w_cmp);
		dst = g_find_vertex(g, word2, w_cmp);

		/* Realocar path para o tamanho corrente. */
		path = realloc(path, g_get_size(g) * sizeof(int));
		/* shortest_path() devolve dist e trata da sua realocação. */
		dist = shortest_path(g, src, dst, path, max_perm*max_perm);

		if (path[dst] == -1) {
			/* Não foi encotrado um caminho entre word1 e word2. */
			fprintf(fpath, "%s %d\n%s\n", word1, -1, word2);
		}
		else {
			/* Foi encontrado um caminho. Temos de percorrer a árvore de
			 * caminho path. */
			fprintf(fpath, "%s %d\n", (char *) v_get_item(g_get_vertex(g, src)), dist[dst]);
			fprint_path(fpath, g, path, dist, path[dst]);
			fprintf(fpath, "%s\n", (char *) v_get_item(g_get_vertex(g, dst)));
		}

		fprintf(fpath, "\n");
	}

	free(dist);
	free(path);
}

/**
 * @brief Imprime caminho até ao vértice dst.
 * @details É necessário inverter o caminho. Para tal, esta
 * função é recursiva: dst é os sucessivo antecessor de path[dst].
 *
 * @param fpath Ficheiro de saída .path.
 * @param g Grafo de tamanho de palavra pertinente.
 * @param path Árvore de caminho.
 * @param dist Tabela de dipathâncias.
 * @param dst Índice do vértice de destino na tabela de vértices (grafo).
 */
void fprint_path(FILE *fpath, Graph *g, int *path, int *dist, int dst)
{
	if (path[dst] == -1) return;

	/* Enquanto ainda não estamos no fim do caminho chamar recursivamente, com
	 * dst = antecessor de dst. */
	fprint_path(fpath, g, path, dist, path[dst]);
	/* Finalmente, quando chegamos ao fim, imprimir os vários vértices do
	 * caminho. */
	fprintf(fpath, "%s\n", (char *) v_get_item(g_get_vertex(g, dst)));
}
