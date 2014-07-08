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
        r->nome_componente = (*i)->identificador();
        r->lider = NULL;
        r->tamanho_componente = 1;
        this->componentes[(*i)->identificador()] = r;
        cluster_vertices *c = new cluster_vertices();
        c->insert(*i);
        this->conjuntos[r] = c;
    }
    this->numero_conjuntos = S.size();
}

int union_find::total_conjuntos() const {
    return this->numero_conjuntos;
}


nome_conjunto_vertices union_find::encontrar(vertice* u) {
    return this->componentes[u->identificador()]->lider->nome_componente;
}


void union_find::unir(nome_conjunto_vertices a, nome_conjunto_vertices b) {
    if (a != b){
        registro_union_find *componente_A = this->componentes[a]->lider;
        registro_union_find *componente_B = this->componentes[b]->lider;
        
        
        if (componente_A->tamanho_componente > componente_B->tamanho_componente){
            cluster_vertices *B = this->conjuntos[componente_B];
            
            for (cluster_vertices::iterator i = B->begin(); i != B->end(); ++i){
                this->componentes[(*i)->identificador()]->lider = componente_A;
            }
            
            componente_A->tamanho_componente += componente_B->tamanho_componente;
        }
        else{
            
            cluster_vertices *A = this->conjuntos[componente_A];
            
            for (cluster_vertices::iterator i = A->begin(); i != A->end(); ++i){
                this->componentes[(*i)->identificador()]->lider = componente_B;
            }
            
            componente_B->tamanho_componente += componente_A->tamanho_componente;
            
        }
        
        this->numero_conjuntos--;
    }
}

list<cluster_vertices*> union_find::clusters() const {
    list<cluster_vertices*> l;
    
    for (map<registro_union_find*, cluster_vertices*>::const_iterator i = this->conjuntos.begin(); i != this->conjuntos.end(); ++i){
        cluster_vertices *c = i->second;
        l.push_back(c);
    }
    
    return l;
}


