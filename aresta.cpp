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

int aresta::peso() {
    int peso = this->valor_conexao;
    return peso;
}

// Representa esta aresa {x,y} com peso p no formato [{x,y},p]
string aresta::para_string()
{
    ostringstream oss;
    
    oss<<"[{"<<((vertice*)this->vertices_extremidades.first)->para_string();
    oss<<","<<((vertice*)this->vertices_extremidades.second)->para_string()<<"}";
    oss<<","<<this->peso()<<"]";
    
    return oss.str();
}

vertice& aresta::obter_vertice_da_outra_extremidade(const vertice &x) const {
    if (&x == this->vertices_extremidades.first){
        return *(this->vertices_extremidades.second);
    }
    else{
        return *(this->vertices_extremidades.first);
    }
        
}
        
