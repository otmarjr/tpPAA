/* 
 * File:   grafo.h
 * Author: Otmar
 *
 * Created on 2 de Julho de 2014, 09:41
 */

#ifndef GRAFO_H
#define	GRAFO_H

#include <list>
#include <map>
#include <set>
#include "vertice.h"
#include "aresta.h"
#include "projeto_software.h"
#include "union_find.h"
using namespace std;

typedef set<aresta*> conjunto_arestas;

class grafo {
public:
    grafo(list<vertice*> verticess);
    
    // A partir da coleção de projetos de software carregada em memória, transforma-os em um grafo.
    static grafo* construir_a_partir_colecao_projetos_e_stop_words(colecao_projetos_software &projetos, list<string> &stop_words); 
    static grafo* construir_a_partir_de_arquivo_pajek(string caminho_arquivo_pajek);
    
    void salvar_em_formato_pajek(string caminho_arquivo);
    void salvar_clusters_projetos_em_arquivo(int quantidade_clusters, string caminho_arquivo_clusters);
private:
    list<vertice*> V;
    list<cluster_vertices*> gerar_kruskal_k_clusters(int k);
    bool adicao_de_nova_aresta_forma_ciclo(set<conjunto_arestas>& conjuntos_arestas_ja_adicionadas, aresta* nova_aresta);
};

#endif	/* GRAFO_H */

