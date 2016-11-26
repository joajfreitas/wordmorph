#include "graph.h"
#include "utils.h"
#include "bool.h"

/*
 * Um grafo é uma array de Vertices.
 * Cada Vertex guarda um Item de um tipo não especificado e uma lista
 * simplesmente ligada de Edges.
 *
 * Os Edges definem uma ligação e têm informação sobre o Vertex de chegada,
 * através do seu indice no array de Vertex, e sobre o peso da ligação.
 */

struct _Vertex {
    Item item;
    List *adj;
};

struct _Edge {
    unsigned int weight;
    unsigned int index;
};

/* free - posição livre do array de Vertices
 * size - tamanho total do Grafo
 * mas_weight - peso maximo de uma ligação no grafo
 */
struct _Graph {
    Vertex **vertices;
    unsigned int free;
    unsigned int size;
    unsigned int max_weight;
};


/* Graph functions */
Graph *g_init(unsigned int size, unsigned int max_weight)
{
    Graph *g = (Graph *) emalloc(sizeof(Graph));
    g->vertices = (Vertex **) emalloc(sizeof(Vertex *) * size);
    g->free = 0;
    g->size = size;
    g->max_weight = max_weight;

    return g;
}

void g_free(Graph *g, void (free_item)(Item item))
{
    unsigned int i;
    Vertex *aux;

    for (i = 0; i < g->size; i++) {
        aux = g->vertices[i];
        l_free(aux->adj, free_item);
        free(aux);
    }

    free(g->vertices);
    free(g);
}

void g_insert(Graph *g, Item i)
{
    Vertex *new_vertex;

    if (g->free == g->size) {
        puts("Erro: grafo está cheio.");
        exit(EXIT_FAILURE);
    }

    new_vertex = v_init(i);
    g->vertices[g->free] = new_vertex;
    g->free++;
}

/* Creates links between Vertices in the Graph
 * Warning: O(v^2)
 */
void g_update_links(Graph *g, unsigned int (*calc_weight)(Item i1, Item i2, unsigned int max))
{
    unsigned int i, j;
    unsigned int weight;

    for (i = 0; i < g->free; i++) {
        for (j = 0; j < i; j++) {
			weight = calc_weight(g->vertices[i]->item, g->vertices[j]->item, g->max_weight);
            if (weight <= g->max_weight) {
                e_add(g, i, j, weight);
            }
        }
	}
}


/* Vertex functions */
Vertex *v_init(Item i)
{
    Vertex *new_vertex = (Vertex *) ecalloc(1, sizeof(Vertex));
    new_vertex->item = i;
    new_vertex->adj = l_init();

    return new_vertex;
}

Vertex *v_get(Graph *g, unsigned int index)
{
    if (g == NULL) {
        fprintf(stderr, "Erro: grafo não existe!");
        exit(EXIT_FAILURE);
    }
    else if (index > g->size) {
        fprintf(stderr, "Erro: index está fora dos limites do grafo!");
        exit(EXIT_FAILURE);
    }

    return g->vertices[index];
}


Item v_get_item(Vertex *v)
{
    if (v == NULL) {
        fprintf(stderr, "Erro: nó não existe!");
        exit(EXIT_FAILURE);
    }
    else if (v->item == NULL) {
        fprintf(stderr, "Erro: item não existe!");
        exit(EXIT_FAILURE);
    }

    return v->item;
}

List *v_get_adj(Vertex *v)
{
    if (v == NULL) {
        fprintf(stderr, "Erro: lista não inicializada");
        exit(EXIT_FAILURE);
    }

    return v->adj;
}


/* Link functions */
/* Initializes a single edge */
Edge *e_init(unsigned int index, unsigned int weight)
{
    Edge *new_edge = (Edge *) emalloc(sizeof(Edge));
    new_edge->index = index;
    new_edge->weight = weight;

    return new_edge;
}

/* Adds edges in both vertices */
void e_add(Graph *g, unsigned int i1, unsigned int i2, unsigned int weight)
{
    Edge *l1 = e_init(i2, weight);
    Edge *l2 = e_init(i1, weight);

    l_insert(&(g->vertices[i1]->adj), l1);
    l_insert(&(g->vertices[i2]->adj), l2);
}

unsigned int e_get_weight(Edge *e)
{
    if (e == NULL) {
        fprintf(stderr, "Erro: ligação não existe!");
        exit(EXIT_FAILURE);
    }
    return e->weight;
}

unsigned int e_get_index(Edge *e)
{
    if (e == NULL) {
        fprintf(stderr, "Erro: ligação não existe");
        exit(EXIT_FAILURE);
    }
    return e->index;
}

bool e_cmp_edges(Edge *e1, Edge *e2)
{
    if (e1 == NULL || e2 == NULL) {
        fprintf(stderr, "Erro: ligação não existe");
        exit(EXIT_FAILURE);
    }
    return (e1->weight > e2->weight);
}
