/* 
 * File:   union_find.cpp
 * Author: Otmar
 * 
 * Created on 7 de Julho de 2014, 10:29
 */

#include <algorithm>

#include "union_find.h"
#include "helpers.h"
using namespace std;

union_find::union_find(list<vertice*>& S) {
    for (list<vertice*>::iterator i = S.begin();i!= S.end();++i){
        registro_union_find *r = new registro_union_find();
        r->nome_componente = (*i)->identificador();
        r->lider = r;
        r->tamanho_componente = 1;
        this->conjuntos_dos_vertices[(*i)->identificador()] = r;
        cluster_vertices *c = new cluster_vertices();
        c->insert(*i);
        this->vertices_dos_conjuntos[r] = c;
        this->clusters_vertices[(*i)->identificador()] = c;
    }
}

int union_find::total_conjuntos() const {
    return this->clusters_vertices.size();
}


nome_conjunto_vertices union_find::encontrar(vertice* u) {
    nome_conjunto_vertices id = u->identificador();
    return this->conjuntos_dos_vertices[id]->lider->nome_componente;
}


void union_find::unir(nome_conjunto_vertices a, nome_conjunto_vertices b) {
    if (a != b){
        registro_union_find *componente_A = this->conjuntos_dos_vertices[a]->lider;
        registro_union_find *componente_B = this->conjuntos_dos_vertices[b]->lider;
        
        
        if (componente_A->tamanho_componente > componente_B->tamanho_componente){
            cluster_vertices *B = this->vertices_dos_conjuntos[componente_B];
            
            for (cluster_vertices::iterator i = B->begin(); i != B->end(); ++i){
                this->conjuntos_dos_vertices[(*i)->identificador()]->lider = componente_A;
            }
            
            componente_A->tamanho_componente += componente_B->tamanho_componente;
            
            for (cluster_vertices::iterator i = this->clusters_vertices[b]->begin(); i!= this->clusters_vertices[b]->end();++i){
                ESCREVER_TRACE((*i)->identificador());
            }
            
            ESCREVER_TRACE(this->clusters_vertices[a]->size());
            this->clusters_vertices[a]->insert(this->clusters_vertices[b]->begin(), this->clusters_vertices[b]->end());
            ESCREVER_TRACE(this->clusters_vertices[a]->size());
            
            ESCREVER_TRACE(this->clusters_vertices.count(b));
            this->clusters_vertices.erase(b);
            ESCREVER_TRACE(this->clusters_vertices.count(b));
        }
        else{
            
            cluster_vertices *A = this->vertices_dos_conjuntos[componente_A];
            
            for (cluster_vertices::iterator i = A->begin(); i != A->end(); ++i){
                this->conjuntos_dos_vertices[(*i)->identificador()]->lider = componente_B;
            }
            
            componente_B->tamanho_componente += componente_A->tamanho_componente;
            this->conjuntos_dos_vertices[a] = this->conjuntos_dos_vertices[b];
            this->clusters_vertices[b]->insert(this->clusters_vertices[a]->begin(), this->clusters_vertices[a]->end());
            this->clusters_vertices.erase(a);
        }
        
    }
}

list<cluster_vertices*> union_find::clusters() const {
    list<cluster_vertices*> l;
    
    for (map<nome_conjunto_vertices, cluster_vertices*>::const_iterator i = this->clusters_vertices.begin(); i != this->clusters_vertices.end(); ++i){
        cluster_vertices *c = i->second;
        l.push_back(c);
    }
    
    return l;
}


