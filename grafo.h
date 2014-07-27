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
typedef list<vertice*> componente_grafo;
typedef list<aresta*> lista_arestas;

class grafo {
public:
    grafo(list<vertice*> verticess);
    
    // A partir da coleção de projetos de software carregada em memória, transforma-os em um grafo.
    static grafo* construir_a_partir_colecao_projetos_e_stop_words(colecao_projetos_software &projetos, list<string> &stop_words, bool ponderar_devs_em_comum, bool ponderar_lp_comum, bool ponderar_idade_comum, bool ponderar_palavras_chave); 
    static grafo* construir_a_partir_de_arquivo_pajek(string caminho_arquivo_pajek);
    
    void salvar_em_formato_pajek(string caminho_arquivo);
    void salvar_clusters_projetos_em_arquivo(int quantidade_clusters, string caminho_arquivo_clusters, colecao_projetos_software &projetos, list<string> &stop_words);
private:
    list<vertice*> V;
    list<aresta*> A;
    list<cluster_vertices*> gerar_kruskal_k_clusters(int k);
    void carregar_todos_componentes_grafo();
    componente_grafo* obter_vertices_alcancaveis_por_busca_em_largura(vertice *vertice_inicial_busca);
    bool conjunto_forma_outlier(int tamanho_conjunto, int quantidade_clusters);
    list<componente_grafo*> todos_componentes_grafo;
    map<componente_grafo*, list<cluster_vertices*> > clusters_dos_componentes;
    map<string, int> coletar_estatisticas_linguagens_projetos(colecao_projetos_software &projetos, list<vertice*> &vertices);
    map<string, int> coletar_estatisticas_devs_projetos(colecao_projetos_software &projetos, list<vertice*> &vertices);
    map<string, int> coletar_estatisticas_palavras_chave_projetos(colecao_projetos_software &projetos, list<vertice*> &vertices, list<string> &stop_words);
    map<bool, int> coletar_estatisticas_ultimo_commit_projetos(colecao_projetos_software &projetos, list<vertice*> &vertices);
    aresta* aresta_equivalente_sentido_oposto(aresta* a);
};

#endif	/* GRAFO_H */

