/* 
 * File:   aresta.cpp
 * Author: Otmar
 * 
 * Created on 2 de Julho de 2014, 09:14
 */

#include "aresta.h"
#include <sstream>

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
string aresta::para_string() const
{
    ostringstream oss;
    
    oss<<"[{"<<this->extremidade_x()->para_string();
    oss<<","<<this->extremidade_y()->para_string()<<"}";
    oss<<","<<this->peso()<<"]";
    
    return oss.str();
}

const vertice& aresta::obter_vertice_da_outra_extremidade(const vertice &x) const {
    if (x == *(this->extremidade_x())){
        return *(this->extremidade_y());
    }
    else{
        return *(this->extremidade_x());
    }
        
}

vertice* aresta::extremidade_x() const {
    return this->vertices_extremidades.first;
}

vertice* aresta::extremidade_y() const {
    return this->vertices_extremidades.second;
}

bool aresta::adjacente_a_outra_aresta(aresta& outra_aresta) const {
    if (this->extremidade_x() == outra_aresta.extremidade_x() || this->extremidade_x() == outra_aresta.extremidade_y()){
        return true;
    }
    
    if (this->extremidade_y() == outra_aresta.extremidade_x() || this->extremidade_y() == outra_aresta.extremidade_y()){
        return true;
    }
    
    return false;
}


