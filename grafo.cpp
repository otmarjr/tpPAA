/* 
 * File:   grafo.cpp
 * Author: Otmar
 * 
 * Created on 2 de Julho de 2014, 09:41
 */

#include "grafo.h"
#include "helpers.h"
#include "union_find.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <tr1/regex>

struct criterio_ordenacao_arestas_kruskal {

    inline bool operator()(const aresta* x, const aresta* y) {
        int peso_x = -x->peso();
        int peso_y = -y->peso();

        return peso_x < peso_y;
    }
};

grafo::grafo(list<vertice*> vertices) {
    copy(vertices.begin(), vertices.end(), back_inserter(this->V));
}

grafo* grafo::construir_a_partir_colecao_projetos_e_stop_words(colecao_projetos_software &projetos, list<string> &stop_words) {
    list<vertice*> vertices;

    for (map<int, projeto_software*>::const_iterator it = projetos.begin(); it != projetos.end(); ++it) {
        projeto_software *p = it->second;
        vertices.push_back(new vertice(p->identificador()));
    }

    for (list<vertice*>::const_iterator i = vertices.begin(); i != vertices.end(); ++i) {
        int id_i = (*i)->identificador();

        vertice* x = (*i);
        projeto_software *proj_x = projetos[x->identificador()];

        list<vertice*>::const_iterator j = i;


        for (++j; j != vertices.end(); ++j) {
            vertice* y = (*j);
            projeto_software *proj_y = projetos[y->identificador()];
            int id_j = proj_y->identificador();
            int peso_i_j = proj_x->calcular_similiaridade_com_outro_projeto(*proj_y, stop_words);

            if (peso_i_j > 0) {
                x->conectar_a_outro_vertice(*y, peso_i_j);
            }
        }
    }

    return new grafo(vertices);
}

void grafo::salvar_em_formato_pajek(string caminho_arquivo) {

    ofstream f_saida(caminho_arquivo.c_str());

    if (!f_saida.good() || !f_saida.is_open()) {
        f_saida.close();
        ostringstream oss;
        oss << "Ocorreu um erro ao tentar abrir para escrita o arquivo de saída do grafo no caminho '" << caminho_arquivo << "'. Verifique se você tem permissão de escrita no caminho informado e se nehum outro processo está utilizando o arquivo.";
        helpers::levantar_erro_execucao(oss.str());
    } else {

        int quantidade_vertices = this->V.size();

        f_saida << "*Vertices " << quantidade_vertices << std::endl;

        map<int, int> indices_pajek;
        int linha_atual = 1;

        for (list<vertice*>::iterator i = this->V.begin(); i != this->V.end(); ++i) {
            vertice *v = *i;
            indices_pajek[v->identificador()] = linha_atual;
            f_saida << linha_atual << " \"" << v->identificador() << "\"" << std::endl;
            linha_atual++;
        }

        f_saida << "*Edges" << std::endl;


        for (list<vertice*>::iterator i = this->V.begin(); i != this->V.end(); ++i) {
            vertice *v = *i;
            int id_pajek_vertice_i = indices_pajek[v->identificador()];

            list<aresta*> l = v->lista_adjacencia();

            for (list<aresta*>::const_iterator j = l.begin(); j != l.end(); ++j) {
                aresta *a = *j;
                int id_pajek_vertice_j = indices_pajek[a->obter_vertice_da_outra_extremidade(*v).identificador()];

                int peso_aresta = a->peso();

                f_saida << id_pajek_vertice_i << " " << id_pajek_vertice_j << " " << peso_aresta << std::endl;
            }

        }


        f_saida.close();

        if (!f_saida.good()) {
            ostringstream oss;
            oss << "Ocorreu um erro durante a escrita do arquivo de saída do grafo no caminho '" << caminho_arquivo << "'. Verifique se você tem permissão de escrita no caminho informado.";
            helpers::levantar_erro_execucao(oss.str());
        }
    }
}

grafo* grafo::construir_a_partir_de_arquivo_pajek(string caminho_arquivo_pajek) {

    list<string> linhas_arquivo = helpers::carregar_linhas_arquivo(caminho_arquivo_pajek);

    list<string>::iterator i = linhas_arquivo.begin();
    istringstream iss(*i);

    string string_atual_arquivo;
    iss >> string_atual_arquivo; // Ignora a parte da 1a linha, que possui apenas o texto *Vertices

    int quantidade_vertices;
    iss >> quantidade_vertices;

    list<vertice*> l;

    map<int, vertice*> vertices_pajek;

    for (++i; distance(linhas_arquivo.begin(), i) <= quantidade_vertices; ++i) {
        string linha = *i;
        stringstream ss(linha);

        int id_pajek;
        ss>>id_pajek;

        int pos = linha.find(' ') + 2;

        string id_projeto_software = linha.substr(pos, linha.size() - pos - 1);

        vertice *v = new vertice(helpers::string_para_inteiro(id_projeto_software));

        vertices_pajek[id_pajek] = v;
        l.push_back(v);
    }

    for (++i; i != linhas_arquivo.end(); ++i) {
        string linha = *i;
        stringstream ss(linha);
        int id_origem, id_destino, peso;

        ss>>id_origem;
        vertice* x = vertices_pajek[id_origem];

        ss>>id_destino;
        vertice* y = vertices_pajek[id_destino];

        ss>>peso;
        x->conectar_a_outro_vertice(*y, peso);

    }
    grafo* g = new grafo(l);

    return g;
}

void grafo::salvar_clusters_projetos_em_arquivo(int quantidade_clusters, string caminho_arquivo_clusters) {
    set<cluster_vertices> clusters = gerar_kruskal_k_clusters(quantidade_clusters);
    
    ofstream f_saida(caminho_arquivo_clusters.c_str());

    if (!f_saida.good() || !f_saida.is_open()) {
        f_saida.close();
        ostringstream oss;
        oss << "Ocorreu um erro ao tentar abrir para escrita o arquivo de saída dos clusters no cmainho '" << caminho_arquivo_clusters << "'. Verifique se você tem permissão de escrita no caminho informado e se nehum outro processo está utilizando o arquivo.";
        helpers::levantar_erro_execucao(oss.str());
    } else {

        int cont_cluster = 0;

        for (set<cluster_vertices>::iterator i = clusters.begin(); i != clusters.end(); ++i) {
            cluster_vertices c = *i;
            cont_cluster++;
            f_saida << '\n' << "Cluster " << cont_cluster << " - " << c.size() << " elementos" << endl;

            stringstream ss;

            for (cluster_vertices::iterator j = c.begin(); j != c.end(); ++j) {
                vertice *v = *j;
                ss << v->identificador() << ',';
            }

            string cluster_str = ss.str();
            cluster_str = helpers::retirar_ultimo_caractere_se_presente(cluster_str, ',');

            f_saida << cluster_str << endl;
        }

        f_saida.close();

        if (!f_saida.good()) {
            ostringstream oss;
            oss << "Ocorreu um erro durante a escrita do arquivo de saída dos clusters no caminho '" << caminho_arquivo_clusters << "'. Verifique se você tem permissão de escrita no caminho informado.";
            helpers::levantar_erro_execucao(oss.str());
        }
    }

}

set<cluster_vertices> grafo::gerar_kruskal_k_clusters(int k) {


    // list<aresta*> arestas;
    list<aresta*> arestas;

    for (list<vertice*>::iterator i = this->V.begin(); i != this->V.end(); ++i) {
        vertice *v = *i;

        list<aresta*> l = v->lista_adjacencia();

        for (list<aresta*>::const_iterator j = l.begin(); j != l.end(); ++j) {
            aresta *a = *j;
            arestas.push_back(a);
        }
    }
    
    arestas.sort(criterio_ordenacao_arestas_kruskal());
    
    union_find *unf = new union_find(this->V);
    set<cluster_vertices> s;

    set<conjunto_arestas> arestas_clusters;
    
    while (unf->total_conjuntos() > k) {
        aresta* menor_aresta = arestas.front();
        

        if (!adicao_de_nova_aresta_forma_ciclo(arestas_clusters, menor_aresta)) {
            vertice* u = menor_aresta->extremidade_x();
            vertice* v = menor_aresta->extremidade_y();

            if (!unf->encontrar(u) != unf->encontrar(v)) {
                // unf->unir(u, v);
            }
        }
    }

    return s;
}

bool grafo::adicao_de_nova_aresta_forma_ciclo(set<conjunto_arestas>& arestas_ja_adicionadas, aresta* nova_aresta) {
    // 1o, encontra o cluster em que a aresta surge conectando:
    
    for (set<conjunto_arestas>::iterator i = arestas_ja_adicionadas.begin();i!= arestas_ja_adicionadas.end();++i){
        conjunto_arestas c  = *i;
        
        bool cluster_possui_intersecao_com_aresta = false;
        
        for (conjunto_arestas::iterator j = c.begin(); j != c.end(); ++j){
            aresta *a = *j;
            if (a->adjacente_a_outra_aresta(*nova_aresta)){
                cluster_possui_intersecao_com_aresta = true;
                break;
            }
        }
        
        if (cluster_possui_intersecao_com_aresta){
            // Como o conjunto de arestas está como uma árvore, um ciclo é detec
            
        }
    }
    
    return false;
}
