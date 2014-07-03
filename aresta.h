/* 
 * File:   aresta.h
 * Author: Otmar
 *
 * Created on 2 de Julho de 2014, 09:14
 */

#ifndef ARESTA_H
#define	ARESTA_H
#include "vertice.h"
#include <utility>
#include <string>

using namespace std;
class aresta {
public:
    aresta(vertice &x, vertice &y, int peso);
    int peso();
    string para_string();
    vertice& obter_vertice_da_outra_extremidade(const vertice &x) const;
private:
    pair<vertice*, vertice*> vertices_extremidades;
    int valor_conexao;
};

#endif	/* ARESTA_H */

