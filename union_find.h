/* 
 * File:   union_find.h
 * Author: Otmar
 *
 * Created on 7 de Julho de 2014, 10:29
 */

#ifndef UNION_FIND_H
#define	UNION_FIND_H

#include "vertice.h"
#include <list>
#include <set>
#include <map>
#include "vertice.h"

typedef int nome_conjunto_vertices;
typedef set<vertice*> cluster_vertices;

class union_find {
public:
    union_find(list<vertice*>& S);
    nome_conjunto_vertices encontrar(vertice* u); // retorna o nome do componente contendu u
    void unir(nome_conjunto_vertices a, nome_conjunto_vertices b);
    int total_conjuntos();
private:
    int numero_conjuntos;
};

#endif	/* UNION_FIND_H */

