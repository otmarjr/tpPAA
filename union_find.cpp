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
        this->componente[*i] = (*i)->identificador();
        this->tamanho[*i] = 1;
    }
    this->total_conjuntos = S.size();
}

int union_find::encontrar(vertice* u) {
    /*
    registro_union_find *r = componente[u];
    
    while(r->novo_nome_conjunto != NULL){
        r = r->novo_nome_conjunto;
    }
    
    return r->nome_conjunto->identificador();
     * */
    
    return this->componente[u];
}


void union_find::unir(vertice* u, vertice* v) {
    int tam_u = this->tamanho[u];
    int tam_v = this->tamanho[v];
    
    if (tam_u > tam_v){
        this->componente[v] = this->componente[u];
        this->tamanho[u] += tam_v;
        this->tamanho[v] = this->tamanho[u];
    }
    else{
        this->componente[u] = this->componente[v];
        this->tamanho[v] += tam_u;
        this->tamanho[u] = this->tamanho[v];
    }
    this->total_conjuntos--;
}

int union_find::numero_conjuntos() {
    return this->total_conjuntos;
}

set<cluster_vertices> union_find::conjuntos_disjuntos() {
    
    set<cluster_vertices> s;
    
    map<int, cluster_vertices> ja_encontrados;
    
    for (map<vertice*,int>::iterator i = this->componente.begin();i!= this->componente.end();++i){
        int id_conjunto = (*i)->second;
        vertice *v = (*i)->first;
        
        if (ja_encontrados.find(id_conjunto) == ja_encontrados.end()){
            cluster_vertices c;
            c.insert(v);
            ja_encontrados[id_conjunto]=c;
            s.insert(v);
        }
        else{
            cluster_vertices c = ja_encontrados[id_conjunto];
            c.insert(v);
        }
    }
    
    return s;
}

