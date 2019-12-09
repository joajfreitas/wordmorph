/**
 * @file graph.c
 * @authors João Pinheiro <joao.castro.pinheiro@tecnico.ulisboa.pt>
 * @authors João Freitas <joao.m.freitas@tecnico.ulisboa.pt>
 * @date 14 Dezembro 2016
 *
 * @brief Implementação da biblioteca de grafos.
 * @details
 * Implementação de grafos ponderados utilizando listas de adjacências.
 */
#include <string.h>
#include <stdlib.h>

#include "graph.h"
#include "utils.h"
#include "bool.h"
#include "heap.h"

/**
 * @brief Vértice de um grafo
 * @details Cada vértice contém um tipo abstracto e um ponteiro para
 *	a cabeça da sua lista de adjacências.
 */
struct _Vertex {
	Item item;
	Edge *adj;
};

/**
 * @brief Aresta ponderada.
 * @details As arestas contêm o peso da ligação, o índice de destino e um
 *	ponteiro para a próxima aresta adjacente do vértice, i.e., uma lista
 *	de adjacências.
 *
 */
struct _Edge {
	unsigned short weight;
	unsigned short index;
	struct _Edge *next;
};

/**
 * @brief Grafo
 * @details vértices: array de vértices
 *	size: número máximo de vértices que o grafo pode conter
 *	free: número de vértices que o grafo contém (posição livre)
 *	max_weight: peso máximo das arestas do grafo
 *
 */
struct _Graph {
	Vertex **vértices;
	unsigned short size;
	unsigned short free;
	unsigned short max_weight;
};


/**
 * @brief Inicializar um grafo.
 * @param size Tamanho máximo do grafo.
 * @param max_weight Peso máximo das arestas.
 *
 * @return Grafo.
 */
Graph *g_init(unsigned short size, unsigned short max_weight)
{
	Graph *g = (Graph *) emalloc(sizeof(Graph));
	g->vértices = (Vertex **) emalloc(sizeof(Vertex *) * size);
	g->free = 0;
	g->size = size;
	g->max_weight = max_weight;

	return g;
}

/**
 * @brief Liberta a memoria alocada para o grafo.
 * @param g Ponteiro para grafo.
 * @param free_item Função libertadora de items.
 */
void g_free(Graph *g, void (free_item)(Item item))
{
	unsigned int i;
	Vertex *aux;

	for (i = 0; i < g->size; i++) {
		aux = g->vértices[i];
		free_adj(aux->adj);
		free_item(aux->item);
		free(aux);
	}

	free(g->vértices);
	free(g);
}


/**
 * @brief Insere um item na posição livre do grafo.
 * @param g Ponteiro para grafo.
 * @param i Item a inserir.
 */
void g_insert(Graph *g, Item i)
{
	Vertex *new_vertex;

	new_vertex = v_init(i);
	/* g->free é a posição livre no grafo. */
	g->vértices[g->free] = new_vertex;
	g->free++;
}

/**
 * @brief Cria ligações entre vértices.
 * @details Verifica quais os vértices cujo peso é menor que o máximo permitido
 * e insere as ligações correspondentes na lista de adjacências.
 *
 * @param g Ponteiro para grafo.
 * @param calc_weight Ponteiro para função que calcula pesos entre arestas.
 */
void g_make_edges(Graph *g, unsigned short (*calc_weight)(Item i1, Item i2, unsigned short max))
{
	unsigned short i, j;
	unsigned short weight;

	for (i = 0; i < g->size; i++) {
		for (j = 0; j < i; j++) {
			weight = calc_weight(g->vértices[i]->item, g->vértices[j]->item, g->max_weight);
			if (weight <= g->max_weight) {
				/* Agora o peso entra quadraticamente. */
				e_add(g, i, j, weight*weight);
			}
		}
	}
	g->max_weight *= g->max_weight;
}

/**
 * @brief Função assessora do numero máximo de vértices no grafo.
 *
 * @param g Ponteiro para grafo.
 * @return Numero máximo de vértices no grafo.
 */
unsigned short g_get_size(Graph *g)
{
	return g->size;
}

/**
 * @brief Função assessora do numero de vértices no grafo.
 *
 * @param g Ponteiro para grafo.
 * @return Numero de vértices no grafo.
 */
unsigned short g_get_free(Graph *g)
{
	return g->free;
}

/**
 * @brief Função assessora do peso máximo das arestas no grafo.
 *
 * @param g Ponteiro para grafo
 * @return Peso máximo entre arestas no grafo.
 */
unsigned short g_get_max_weight(Graph *g)
{
	return g->max_weight;
}

/**
 * @brief Função assessora de um vértice i do grafo.
 *
 * @param g Ponteiro para grafo.
 * @param i Índice do vértice.
 *
 * @return Vértice i do grafo.
 */
Vertex *g_get_vertex(Graph *g, unsigned short i)
{
	return g->vértices[i];
}

/**
 * @brief Encontra vértice no grafo.
 * @details Procura um vértice no grafo linearmente.
 *
 * @param g Ponteiro para grafo.
 * @param i1 Item que identifica o vértice a encontrar
 * @param cmp_item Ponteiro para função comparadora de vértices.
 * @return Índice do vértice encontrado ou -1 em caso de insucesso.
 */
int g_find_vertex(Graph *g, Item i1, int (*cmp_item)(Item c1, Item c2))
{
	int i;

	for (i = 0; i < g_get_size(g); i++)
		if (!cmp_item(g->vértices[i]->item, i1))
			return i;

	return -1;
}


/**
 * @brief Inicializar um vértice com item i.
 *
 * @param i Item a inserir no vértice.
 * @return Ponteiro para o novo vértice.
 */
Vertex *v_init(Item i)
{
	Vertex *new_vertex = (Vertex *) emalloc(sizeof(Vertex));
	new_vertex->item = i;
	new_vertex->adj = NULL;

	return new_vertex;
}

/**
 * @brief Função assessora de items no vértice
 *
 * @param v Ponteiro para vértice
 * @return Item do vértice.
 */
Item v_get_item(Vertex *v)
{
	return v->item;
}

/**
 * @brief Função assessora de listas de adjacências.
 *
 * @param v Ponteiro para vértice.
 * @return Lista de adjacências.
 */
Edge *v_get_adj(Vertex *v)
{
	return v->adj;
}

/**
 * @brief Inserir edge na lista de adjacências.
 *
 * @param adj Lista de adjacências.
 * @param index Índice do vértice de destino.
 * @param weight Peso da aresta.
 */
void e_insert(Edge **adj, unsigned short index, unsigned short weight)
{
	Edge *new_edge = (Edge *) emalloc(sizeof(Edge));
	new_edge->index = index;
	new_edge->weight = weight;
	new_edge->next = *adj;
	*adj = new_edge;

	return;
}

/**
 * @brief Cria arestas entre vértices.
 * @details Insere uma aresta em cada vértice da ligação.
 *
 * @param g Ponteiro para grafo.
 * @param i1 Índice do vértice de partida.
 * @param i2 Índice do vértice de destino.
 * @param weight Peso da aresta.
 */
void e_add(Graph *g, unsigned short i1, unsigned short i2, unsigned short weight)
{
	e_insert(&(g->vértices[i1]->adj), i2, weight);
	e_insert(&(g->vértices[i2]->adj), i1, weight);
}

/**
 * @brief Função assessora do peso da aresta.
 *
 * @param e Ponteiro para edge.
 * @return Peso da aresta
 */
unsigned short e_get_weight(Edge *e)
{
	return e->weight;
}

/**
 * @brief Função assessora do índice do vértice de destino da aresta.
 *
 * @param e Ponteiro para aresta.
 * @return Índice do vértice de destino da aresta.
 */
unsigned short e_get_index(Edge *e)
{
	return e->index;
}

/**
 * @brief Função assessora do próximo elemento da lista de adjacências
 *
 * @param l Ponteiro para aresta
 * @return Próximo elemento da lista de adjacências
 */
Edge *e_get_next(Edge *l)
{
	return l->next;
}

/**
 * @brief Liberta a lista de adjacências.
 *
 * @param head Ponteiro para a cabeça da lista de adjacências
 */
void free_adj(Edge *head)
{
	Edge *aux = head;
	Edge *tmp;

	while (aux) {
		tmp = aux->next;
		free(aux);
		aux = tmp;
	}
}
