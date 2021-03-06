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


class union_find {
public:
    union_find(list<vertice*>& S);
    nome_conjunto_vertices encontrar(vertice* u); // retorna o nome do componente contendu u
    void unir(nome_conjunto_vertices a, nome_conjunto_vertices b);
    int total_conjuntos() const;
    list<cluster_vertices*> clusters() const;
private:
    class registro_union_find{
    public:
        int tamanho_componente;
        registro_union_find* lider;
        nome_conjunto_vertices nome_componente;
    };

    map<nome_conjunto_vertices, registro_union_find*> conjuntos_dos_vertices;
    map<registro_union_find*, cluster_vertices*> vertices_dos_conjuntos;
    map<nome_conjunto_vertices, cluster_vertices*> clusters_vertices;
};

#endif	/* UNION_FIND_H */

