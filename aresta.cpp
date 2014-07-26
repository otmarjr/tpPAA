/* 
 * File:   aresta.cpp
 * Author: Otmar
 * 
 * Created on 2 de Julho de 2014, 09:14
 */

#include "aresta.h"
#include "helpers.h"
#include <sstream>
#include <algorithm>

aresta::aresta(vertice &x, vertice &y, int peso) {
    this->vertices_extremidades.first = &x;
    this->vertices_extremidades.second = &y;
    this->valor_conexao = peso;
}

int aresta::peso() const {
    int peso = this->valor_conexao;
    return peso;
}

// Representa esta aresa {x,y} com peso p no formato [{x,y},p]

string aresta::para_string() const {
    ostringstream oss;

    oss << "[{" << this->extremidade_x()->para_string();
    oss << "," << this->extremidade_y()->para_string() << "}";
    oss << "," << this->peso() << "]";

    return oss.str();
}

const vertice& aresta::obter_vertice_da_outra_extremidade(const vertice &x) const {
    if (x == *(this->extremidade_x())) {
        return *(this->extremidade_y());
    } else {
        return *(this->extremidade_x());
    }

}

vertice* aresta::extremidade_x() const {
    return this->vertices_extremidades.first;
}

vertice* aresta::extremidade_y() const {
    return this->vertices_extremidades.second;
}

int aresta::total_de_arestas_na_lista(list<vertice*>& l) {
    int total = 0;

    for (list<vertice*>::iterator i = l.begin(); i != l.end(); ++i) {
        ESCREVER_TRACE((*i)->lista_adjacencia().size());
        ESCREVER_TRACE((*i)->identificador());
        ESCREVER_TRACE((*i)->para_string());
        for (list<aresta*>::const_iterator j = (*i)->lista_adjacencia().begin(); j!= (*i)->lista_adjacencia().end();++j){
            aresta *a = (*j);
            vertice *v = a->extremidade_y();
            
            ESCREVER_TRACE(v->identificador());
            if (find(l.begin(), l.end(), v) != l.end()){
                total+=1;
            }
        }
    }
    total = total / 2; // O grafo é não direcionado
    return total;
}
