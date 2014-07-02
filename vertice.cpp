/* 
 * File:   vertice.cpp
 * Author: Otmar
 * 
 * Created on 2 de Julho de 2014, 09:06
 */

#include "vertice.h"
#include "aresta.h"
#include <algorithm>
#include <sstream>

vertice::vertice(int id) {
    this->id = id;
}

int vertice::identificador()
{
    return this->id;
}

string vertice::para_string()
{
    ostringstream oss;
    
    oss<<this->id;
    
    return oss.str();
}
void vertice::conectar_a_outro_vertice(vertice &y, int peso_aresta) {
    aresta* a = new aresta(*this, y, peso_aresta);
    this->adjacencia[y.identificador()] = a;

    if (!y.adjacente_a(*this)){
        y.conectar_a_outro_vertice(*this,peso_aresta);
    }
}

bool vertice::adjacente_a(vertice &outro){
    
    return this->adjacencia.find(outro.id) != this->adjacencia.end();
}

