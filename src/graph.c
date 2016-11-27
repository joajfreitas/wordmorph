#include "graph.h"
#include "utils.h"
#include "bool.h"
#include "heap.h"

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
    unsigned short weight;
    unsigned short index;
};

/* free - posição livre do array de Vertices
 * size - tamanho total do Grafo
 * max_weight - peso maximo de uma ligação no grafo
 */
struct _Graph {
    Vertex **vertices;
    unsigned short free;
    unsigned short size;
    unsigned short max_weight;
};


/* Graph functions */
Graph *g_init(unsigned short size, unsigned short max_weight)
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
        l_free(aux->adj, free);
        free_item(aux->item);
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
 * TODO: É mesmo necessario criar duas edges sempre que se faz uma ligação?
 */
void g_update_links(Graph *g, unsigned short (*calc_weight)(Item i1, Item i2, unsigned short max))
{
    unsigned short i, j;
    unsigned short weight;

    for (i = 0; i < g->free; i++) {
        for (j = 0; j < i; j++) {
			weight = calc_weight(g->vertices[i]->item, g->vertices[j]->item, g->max_weight);
            if (weight - g->max_weight <= 0) {
				/*printf("vertice: %s, edge: %s\n", (char *) g->vertices[i]->item, (char *) g->vertices[j]->item);*/
                e_add(g, i, j, weight);
            }
        }
	}
}

#define POT_DIST (wt[v] + edge->weight)
#define MAX_WT 1 /* TODO */

/*Implementação do algoritmo de Djikstra*/
/* st: árvore de caminhos mais curtos de src para todos os outros vértices */
void g_find_path(Graph *g, int src, int *st, int *wt, bool (*cmp)(Item i1, Item i2))
{
    int v; /* Index dum vértice */
	int v_adj; /* Index dum vértice adjacente a v */
    Edge *edge; /* Aresta de v para v_adj */

    h_init(g->free);

	/* Encher a Heap com todos os vértices */
    for (v = 0; v < g->free; v++) {
        st[v] = -1;
        wt[v] = MAX_WT;
        h_insert(&v, cmp);
    }

    wt[src] = 0;
    fixup(src, cmp);
    while (!h_empty()) {
        if (wt[v = *((int *) h_delmax())] != MAX_WT) {
            for (edge = (Edge *) g->vertices[v]->adj; edge != NULL; edge = (Edge *) l_get_next(g->vertices[v]->adj)) {
                if (POT_DIST < wt[v_adj = edge->index]) {
                    wt[v_adj] = POT_DIST;
                    fixup(v_adj, cmp);
                    st[v_adj] = v;
                }
            }
        }
    }
}

unsigned short g_get_size(Graph *g)
{
    if (g == NULL) {
        fprintf(stderr, "Erro: grafo não existe!\n");
        exit(EXIT_FAILURE);
    }

    return g->size;
}

unsigned short g_get_free(Graph *g)
{
    if (g == NULL) {
        fprintf(stderr, "Erro: grafo não existe!\n");
        /*exit(EXIT_FAILURE);*/
    }

    return g->free;
}

/* Vertex functions */
Vertex *v_init(Item i)
{
    Vertex *new_vertex = (Vertex *) ecalloc(1, sizeof(Vertex));
    new_vertex->item = i;
    new_vertex->adj = l_init();

    return new_vertex;
}

Vertex *v_get(Graph *g, unsigned short index)
{
    if (g == NULL) {
        fprintf(stderr, "Erro: grafo não existe!\n");
        exit(EXIT_FAILURE);
    }
    else if (index > g->size) {
        fprintf(stderr, "Erro: index está fora dos limites do grafo!\n");
        exit(EXIT_FAILURE);
    }

    return g->vertices[index];
}


Item v_get_item(Vertex *v)
{
    if (v == NULL) {
        fprintf(stderr, "Erro: nó não existe!\n");
        exit(EXIT_FAILURE);
    }
    else if (v->item == NULL) {
        fprintf(stderr, "Erro: item não existe!\n");
        exit(EXIT_FAILURE);
    }

    return v->item;
}

List *v_get_adj(Vertex *v)
{
    if (v == NULL) {
        fprintf(stderr, "Erro: lista não inicializada\n");
        exit(EXIT_FAILURE);
    }

    return v->adj;
}


/* Edge functions */
/* Initializes a single edge */
Edge *e_init(unsigned short index, unsigned short weight)
{
    Edge *new_edge = (Edge *) emalloc(sizeof(Edge));
    new_edge->index = index;
    new_edge->weight = weight;

    return new_edge;
}

/* Adds edges in both vertices */
void e_add(Graph *g, unsigned short i1, unsigned short i2, unsigned short weight)
{
    Edge *l1 = e_init(i2, weight);
    Edge *l2 = e_init(i1, weight);

    l_insert(&(g->vertices[i1]->adj), l1);
    l_insert(&(g->vertices[i2]->adj), l2);
}

unsigned short e_get_weight(Edge *e)
{
    if (e == NULL) {
        fprintf(stderr, "Erro: ligação não existe!\n");
        exit(EXIT_FAILURE);
    }
    return e->weight;
}

unsigned short e_get_index(Edge *e)
{
    if (e == NULL) {
        fprintf(stderr, "Erro: ligação não existe\n");
        exit(EXIT_FAILURE);
    }
    return e->index;
}

bool e_cmp_edges(Edge *e1, Edge *e2)
{
    if (e1 == NULL || e2 == NULL) {
        fprintf(stderr, "Erro: ligação não existe\n");
        exit(EXIT_FAILURE);
    }
    return (e1->weight > e2->weight);
}

void v_adj_print(Graph *g, Vertex *v)
{
    List *aux = v->adj;
    while (aux != NULL) {
        printf("adj: %s\n", (char *) g->vertices[e_get_index((Edge *) l_get_item(aux))]->item);
        aux = l_get_next(aux);
    }
}
