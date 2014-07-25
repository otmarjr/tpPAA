/* 
 * File:   vertice.cpp
 * Author: Otmar
 * 
 * Created on 2 de Julho de 2014, 09:06
 */

#include "vertice.h"
#include "aresta.h"
#include "helpers.h"
#include <algorithm>
#include <sstream>

vertice::vertice(int id) {
    this->id = id;
}

int vertice::identificador() const {
    return this->id;
}

string vertice::para_string() const {
    const char SEPARADOR_ANTES_LISTA_ADJAECNCIA = ':';
    const char SEPARADOR_ENTRE_ARESTAS = ';';
    const char SEPARADOR_ID_ADJACENTE_E_PESO = ',';

    ostringstream oss;

    oss << this->id << SEPARADOR_ANTES_LISTA_ADJAECNCIA;


    for (map<int, aresta*>::const_iterator i = this->adjacencia.begin(); i != this->adjacencia.end(); ++i) {
        aresta *a = i->second;
        oss << a->obter_vertice_da_outra_extremidade(*this).identificador() << SEPARADOR_ID_ADJACENTE_E_PESO << a->peso();
        oss << SEPARADOR_ENTRE_ARESTAS;
    }

    string string_lista_adjacencia = oss.str();

    // Tira o último ;, se houver:
    helpers::retirar_ultimo_caractere_se_presente(string_lista_adjacencia, SEPARADOR_ENTRE_ARESTAS);


    return string_lista_adjacencia;
}

void vertice::conectar_a_outro_vertice(vertice &y, int peso_aresta) {
    aresta* a = new aresta(*this, y, peso_aresta);
    this->adjacencia[y.identificador()] = a;

    if (!y.adjacente_a(*this)) {
        y.conectar_a_outro_vertice(*this, peso_aresta);
    }
}

bool vertice::adjacente_a(vertice &outro) const {

    return this->adjacencia.find(outro.id) != this->adjacencia.end();
}

list<aresta*> vertice::lista_adjacencia() const {
    list<aresta*> l;

    for (map<int, aresta*>::const_iterator i = this->adjacencia.begin(); i != this->adjacencia.end(); ++i) {
        l.push_back(i->second);
    }

    return l;
}

bool vertice::operator==(const vertice& outro) const {
    return this->id == outro.id;
}
