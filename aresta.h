/* 
 * File:   aresta.h
 * Author: Otmar
 *
 * Created on 2 de Julho de 2014, 09:14
 */

#ifndef ARESTA_H
#define	ARESTA_H
#include "vertice.h"
#include "grafo.h"
#include <utility>
#include <string>

using namespace std;
class aresta {
public:
    aresta(vertice &x, vertice &y, int peso);
    int peso() const;
    string para_string() const;
    const vertice& obter_vertice_da_outra_extremidade(const vertice &x) const;
    vertice* extremidade_x() const;
    vertice* extremidade_y() const;
    static int total_de_arestas_na_lista(list<vertice*> &l, bool contar_arestas_para_vertices_fora_do_conjunto);
private:
    pair<vertice*, vertice*> vertices_extremidades;
    int valor_conexao;
    
};

#endif	/* ARESTA_H */

