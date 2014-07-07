/* 
 * File:   union_find.h
 * Author: Otmar
 *
 * Created on 7 de Julho de 2014, 10:29
 */

#ifndef UNION_FIND_H
#define	UNION_FIND_H

#include "vertice.h"
#include <map>
#include <list>
#include <vector>

class union_find {
public:
    union_find(list<vertice*>& S);
    int encontrar(vertice* u); // retorna o nome do componente contendu u
    void unir(vertice* u, vertice* v);
private:

    class registro_union_find {
    public:
        vertice *nome_conjunto;
        registro_union_find *novo_nome_conjunto;
        int tamanho_conjunto;
    };
    map<vertice*, registro_union_find*> componente;
};

#endif	/* UNION_FIND_H */

