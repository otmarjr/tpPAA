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
#include <list>

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

    oss << "[{" << this->extremidade_x()->identificador();
    oss << "," << this->extremidade_y()->identificador() << "}";
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

int aresta::total_de_arestas_no_cluster(cluster_vertices* c) {

    unordered_map<int, int> arestas_adicionadas;

    for (cluster_vertices::iterator i = c->begin(); i != c->end(); ++i) {
        list<aresta*> adj = (*i)->lista_adjacencia();

        for (list<aresta*>::const_iterator j = adj.begin(); j != adj.end(); ++j) {
            aresta *a = *j;
            
            if (arestas_adicionadas.count(a->identificador_aresta()) == 0){
                if (find(c->begin(), c->end(), a->extremidade_y()) != c->end()){
                    arestas_adicionadas[a->identificador_aresta()] = a->identificador_aresta();
                }
            }
        }
    }

    return arestas_adicionadas.size();
}

int aresta::total_de_arestas_na_lista(list<vertice*>& l, bool contar_arestas_para_vertices_fora_do_conjunto) {
    int total = 0;

    list<pair<int, int> > pares_do_conjunto_adicionados;

    for (list<vertice*>::iterator i = l.begin(); i != l.end(); ++i) {
        list<aresta*> adj = (*i)->lista_adjacencia();

        for (list<aresta*>::const_iterator j = adj.begin(); j != adj.end(); ++j) {
            aresta *a = *j;
            vertice *v = a->extremidade_y();
            if (find(l.begin(), l.end(), v) != l.end() || contar_arestas_para_vertices_fora_do_conjunto) {
                pair<int, int> par_x_y = make_pair((*i)->identificador(), v->identificador());
                pair<int, int> par_y_x = make_pair(v->identificador(), (*i)->identificador());

                if (find(pares_do_conjunto_adicionados.begin(), pares_do_conjunto_adicionados.end(), par_y_x) == pares_do_conjunto_adicionados.end()) {
                    pares_do_conjunto_adicionados.push_back(par_x_y);
                }
            }
        }
    }

    total = pares_do_conjunto_adicionados.size();
    return total;
}

bool aresta::operator==(const aresta& outra) const {
    return this->igual_mesmo_sentido(outra) || this->igual_sentido_oposto(outra);
}

bool aresta::igual_mesmo_sentido(const aresta& outra) const {
    return outra.extremidade_x() == this->extremidade_x() && outra.extremidade_y() == this->extremidade_y() && this->peso() == outra.peso();
}

bool aresta::igual_sentido_oposto(const aresta& outra) const {
    return outra.extremidade_y() == this->extremidade_x() && outra.extremidade_x() == this->extremidade_y() && this->peso() == outra.peso();
}

int aresta::identificador_aresta() const {
    return this->extremidade_x()->identificador() * this->extremidade_y()->identificador();
}

