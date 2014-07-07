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
        registro_union_find *r = new registro_union_find();
        r->nome_conjunto = *i;
        r->novo_nome_conjunto = NULL;
        r->tamanho_conjunto = 1;
        this->componente[*i] = r;
    }
}

int union_find::encontrar(vertice* u) {
    registro_union_find *r = componente[u];
    
    while(r->novo_nome_conjunto != NULL){
        r = r->novo_nome_conjunto;
    }
    
    return r->nome_conjunto->identificador();
}


void union_find::unir(vertice* u, vertice* v) {
    int tam_u = this->componente[u]->tamanho_conjunto;
    int tam_v = this->componente[v]->tamanho_conjunto;
    
    if (tam_u > tam_v){
        this->componente[v]->novo_nome_conjunto = this->componente[u];
        this->componente[u]->tamanho_conjunto += tam_v;
    }
    else{
        this->componente[u]->novo_nome_conjunto = this->componente[v];
        this->componente[v]->tamanho_conjunto += tam_u;
    }
}



