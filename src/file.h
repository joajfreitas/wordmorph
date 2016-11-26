#ifndef _FILE_H
#define _FILE_H

#include <stdio.h>

#include "graph.h"

int *find_max_perms(FILE *fpal);

Graph **read_dic(FILE *fdic, int *max_perms);

void solve_pal(FILE *fpal, FILE *fpath, Graph **graphs);

#endif