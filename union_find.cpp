/* 
 * File:   union_find.cpp
 * Author: Otmar
 * 
 * Created on 7 de Julho de 2014, 10:29
 */

#include <algorithm>

#include "union_find.h"
using namespace std;

union_find::union_find(list<vertice*>& S) {
    for (list<vertice*>::iterator i = S.begin();i!= S.end();++i){
        /*registro_union_find *r = new registro_union_find();
        r->nome_conjunto = *i;
        r->novo_nome_conjunto = NULL;
        r->tamanho_conjunto = 1;*/
        // this->componente[*i] = (*i)->identificador();
        // this->tamanho[*i] = 1;
    }
    this->numero_conjuntos = S.size();
}

int union_find::total_conjuntos() {
    return this->numero_conjuntos;
}


nome_conjunto_vertices union_find::encontrar(vertice* u) {
    /*
    registro_union_find *r = componente[u];
    
    while(r->novo_nome_conjunto != NULL){
        r = r->novo_nome_conjunto;
    }
    
    return r->nome_conjunto->identificador();
     * */
    
    return 0;
}


void unir(nome_conjunto_vertices a, nome_conjunto_vertices b) {
}
