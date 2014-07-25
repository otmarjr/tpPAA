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
#include <vector>

struct criterio_ordenacao_arestas_kruskal {

    inline bool operator()(const aresta* x, const aresta* y) {
        int peso_x = -x->peso();
        int peso_y = -y->peso();

        return peso_x < peso_y;
    }
};

constexpr dimensoes_similaridade operator|(dimensoes_similaridade d1, dimensoes_similaridade d2) {
    return dimensoes_similaridade(int(d1) | int(d2));
}

constexpr dimensoes_similaridade operator&(dimensoes_similaridade d1, dimensoes_similaridade d2) {
    return dimensoes_similaridade(int(d1) & int(d2));
}

grafo::grafo(list<vertice*> vertices) {
    copy(vertices.begin(), vertices.end(), back_inserter(this->V));
}

grafo* grafo::construir_a_partir_colecao_projetos_e_stop_words(colecao_projetos_software &projetos, list<string> &stop_words, bool ponderar_devs_em_comum, bool ponderar_lp_comum, bool ponderar_idade_comum, bool ponderar_palavras_chave) {
    list<vertice*> vertices;

    for (map<int, projeto_software*>::const_iterator it = projetos.begin(); it != projetos.end(); ++it) {
        projeto_software *p = it->second;
        vertices.push_back(new vertice(p->identificador()));
    }

    for (list<vertice*>::const_iterator i = vertices.begin(); i != vertices.end(); ++i) {
        vertice* x = (*i);
        projeto_software *proj_x = projetos[x->identificador()];

        list<vertice*>::const_iterator j = i;


        for (++j; j != vertices.end(); ++j) {
            vertice* y = (*j);
            projeto_software *proj_y = projetos[y->identificador()];

            dimensoes_similaridade criterios_dimensoes = dimensoes_similaridade::NENHUMA;

            if (ponderar_devs_em_comum) {
                criterios_dimensoes = criterios_dimensoes | dimensoes_similaridade::DESENVOLVEDORES_COMUNS;
            }

            if (ponderar_lp_comum) {
                criterios_dimensoes = criterios_dimensoes | dimensoes_similaridade::LINGUAGEM_PROGRAMACAO;
            }

            if (ponderar_idade_comum) {
                criterios_dimensoes = criterios_dimensoes | dimensoes_similaridade::IDADE_PROJETO;
            }

            if (ponderar_palavras_chave) {
                criterios_dimensoes = criterios_dimensoes | dimensoes_similaridade::PALAVRAS_CHAVE;
            }

            int peso_i_j = proj_x->calcular_similiaridade_com_outro_projeto(*proj_y, stop_words, criterios_dimensoes);

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

void grafo::salvar_clusters_projetos_em_arquivo(int quantidade_clusters, string caminho_arquivo_clusters, colecao_projetos_software &projetos, list<string> &stop_words) {
    list<cluster_vertices*> clusters = gerar_kruskal_k_clusters(quantidade_clusters);

    ofstream f_saida(caminho_arquivo_clusters.c_str());

    if (!f_saida.good() || !f_saida.is_open()) {
        f_saida.close();
        ostringstream oss;
        oss << "Ocorreu um erro ao tentar abrir para escrita o arquivo de saída dos clusters no cmainho '" << caminho_arquivo_clusters << "'. Verifique se você tem permissão de escrita no caminho informado e se nehum outro processo está utilizando o arquivo.";
        helpers::levantar_erro_execucao(oss.str());
    } else {

        int cont_cluster = 0;

        for (list<cluster_vertices*>::iterator i = clusters.begin(); i != clusters.end(); ++i) {
            cluster_vertices *c = *i;
            cont_cluster++;

            map<string, int> ocorrencias_lps;
            map<string, int> ocorrencias_devs;
            map<bool, int> ocorrencias_idade;
            map<string, int> ocorrencias_palavras_chave;

            for (cluster_vertices::iterator j = c->begin(); j != c->end(); ++j) {
                vertice *v = *j;
                int identificador = v->identificador();
                projeto_software *p = projetos[identificador];

                if (ocorrencias_lps.find(p->linguagem_programacao()) == ocorrencias_lps.end()) {
                    ocorrencias_lps[p->linguagem_programacao()] = 1;
                } else {
                    ocorrencias_lps[p->linguagem_programacao()] += 1;
                }

                if (ocorrencias_idade.find(p->modificado_ultimo_ano()) == ocorrencias_idade.end()) {
                    ocorrencias_idade[p->modificado_ultimo_ano()] = 1;
                } else {
                    ocorrencias_idade[p->modificado_ultimo_ano()] += 1;
                }

                for (list<string>::const_iterator k = p->membros().begin(); k != p->membros().end(); ++k) {
                    if (ocorrencias_devs.find(*k) == ocorrencias_devs.end()) {
                        ocorrencias_devs[*k] = 1;
                    } else {
                        ocorrencias_devs[*k] += 1;
                    }
                }

                list<string> palavras_chave = p->palavras_significativas_na_descricao(stop_words);

                for (list<string>::iterator k = palavras_chave.begin(); k != palavras_chave.end(); ++k) {
                    if (ocorrencias_palavras_chave.find(*k) == ocorrencias_palavras_chave.end()) {
                        ocorrencias_palavras_chave[*k] = 1;
                    } else {
                        ocorrencias_palavras_chave[*k] += 1;
                    }
                }
            }

            stringstream ss_lps;
            stringstream ss_devs;
            stringstream ss_idade;
            stringstream ss_descricao;

            for (map<string, int>::iterator j = ocorrencias_devs.begin(); j != ocorrencias_devs.end(); ++j) {
                string dev = (*j).first;
                ss_devs << dev << ": " << (*j).second << ",";
            }

            for (map<string, int>::iterator j = ocorrencias_lps.begin(); j != ocorrencias_lps.end(); ++j) {
                string lp = (*j).first;
                ss_lps << lp << ": " << (*j).second << ",";
            }

            for (map<string, int>::iterator j = ocorrencias_palavras_chave.begin(); j != ocorrencias_palavras_chave.end(); ++j) {
                string palavra = (*j).first;
                ss_descricao << palavra << ": " << (*j).second << ",";
            }

            for (map<bool, int>::iterator j = ocorrencias_idade.begin(); j != ocorrencias_idade.end(); ++j) {
                bool modificado_ultimo_ano = (*j).first;
                ss_idade << modificado_ultimo_ano << ": " << (*j).second << ",";
            }

            f_saida << '\n' << "Cluster " << cont_cluster << " - " << c->size() << " elementos" << endl;

            stringstream ss;

            for (cluster_vertices::iterator j = c->begin(); j != c->end(); ++j) {
                vertice *v = *j;
                ss << v->identificador() << ',';
            }

            string cluster_str = ss.str();
            cluster_str = helpers::retirar_ultimo_caractere_se_presente(cluster_str, ',');

            f_saida << cluster_str << endl;

            string lista_devs = ss_devs.str();
            helpers::retirar_ultimo_caractere_se_presente(lista_devs, ',');

            string lista_lps = ss_lps.str();
            helpers::retirar_ultimo_caractere_se_presente(lista_lps, ',');

            string lista_palavras_chave = ss_descricao.str();
            helpers::retirar_ultimo_caractere_se_presente(lista_palavras_chave, ',');

            string lista_idade = ss_idade.str();
            helpers::retirar_ultimo_caractere_se_presente(lista_idade, ',');

            f_saida << "\n\t" << " Total desenvolvedores: " << ocorrencias_devs.size() << " Frequência desenvolvedores: " << lista_devs;
            f_saida << "\n\t" << " Total lignagens de programação: " << ocorrencias_lps.size() << " Frequência linguagens de programação: " << lista_lps;
            f_saida << "\n\t" << " Total palavras chave: " << ocorrencias_palavras_chave.size() << " Frequência palavras: " << lista_palavras_chave;
            f_saida << "\n\t" << " Total critérios idade " << ocorrencias_idade.size() << " Frequência projetos com menos de um ano: " << lista_idade;
        }

        f_saida.close();

        if (!f_saida.good()) {
            ostringstream oss;
            oss << "Ocorreu um erro durante a escrita do arquivo de saída dos clusters no caminho '" << caminho_arquivo_clusters << "'. Verifique se você tem permissão de escrita no caminho informado.";
            helpers::levantar_erro_execucao(oss.str());
        }
    }

}

list<cluster_vertices*> grafo::gerar_kruskal_k_clusters(int k) {


    list<cluster_vertices*> clusters;

    list<componente_grafo> componentes_grafo = this->obter_todos_componentes();

    for (list<componente_grafo>::iterator i = componentes_grafo.begin(); i != componentes_grafo.end(); ++i) {

        componente_grafo c = *i;
        list<aresta*> arestas;
        list<vertice*> vertices_clusterizacao;

        for (list<vertice*>::iterator i = c.begin(); i != c.end(); ++i) {
            vertice *v = *i;

            list<aresta*> l = v->lista_adjacencia();

            for (list<aresta*>::const_iterator j = l.begin(); j != l.end(); ++j) {
                aresta *a = *j;
                arestas.push_back(a);

                if (find(vertices_clusterizacao.begin(), vertices_clusterizacao.end(), v) == vertices_clusterizacao.end()) {
                    vertices_clusterizacao.push_back(v);
                }
            }
        }

        arestas.sort(criterio_ordenacao_arestas_kruskal());

        int total_arestas = arestas.size();

        if (total_arestas >= k) {
            union_find *unf = new union_find(vertices_clusterizacao);

            while (unf->total_conjuntos() > k) {
                aresta* menor_aresta = arestas.front();

                arestas.pop_front();

                vertice* u = menor_aresta->extremidade_x();
                vertice* v = menor_aresta->extremidade_y();

                nome_conjunto_vertices nome_u = unf->encontrar(u);
                nome_conjunto_vertices nome_v = unf->encontrar(v);

                if (nome_u != nome_v) {
                    unf->unir(nome_u, nome_v);
                }

                nome_u = unf->encontrar(u);
                nome_v = unf->encontrar(v);
            }
            clusters.merge(unf->clusters());
        } else {
            cluster_vertices *cluster_pequeno;

            for (componente_grafo::iterator i = c.begin(); i != c.end(); ++i) {
                cluster_pequeno->insert(*i);
            }

            clusters.push_back(cluster_pequeno);
        }
    }



    return clusters;
}

componente_grafo grafo::obter_vertices_alcanveis_por_busca_em_largura(vertice* vertice_inicial_busca) {
    componente_grafo c;

    map < vertice*, bool> descobertos;
    vector<set<vertice*> > camadas;

    ESCREVER_TRACE(vertice_inicial_busca->para_string())

    for (list<vertice*>::const_iterator i = this->V.begin(); i != this->V.end(); ++i) {
        descobertos[*i] = false;
    }

    descobertos[vertice_inicial_busca] = true;
    set<vertice*> camada_atual;
    camada_atual.insert(vertice_inicial_busca);
    camadas.push_back(camada_atual);

    int contador_camada = 0;

    while (!camada_atual.empty()) {
        camada_atual.clear();

        for (set<vertice*>::iterator i = camadas[contador_camada].begin(); i != camadas[contador_camada].end(); ++i) {
            vertice *u = *i;
            ESCREVER_TRACE(u->para_string());
            c.push_back(u);

                for (list<aresta*>::const_iterator j = u->lista_adjacencia().begin(); j != u->lista_adjacencia().end(); ++j) {
                aresta *a = *j;
                ESCREVER_TRACE(a->para_string());
                vertice *v = a->extremidade_y();

                if (descobertos[v] == false) {
                    descobertos[v] = true;
                    c.push_back(v);
                    camada_atual.insert(v);
                }
            }

            if (camada_atual.size() > 0) {
                contador_camada++;
                camadas.push_back(camada_atual);
            }
        }
    }


    return c;
}

list<componente_grafo> grafo::obter_todos_componentes() {
    list<componente_grafo> comps;

    list<vertice*> vertices_ja_presentes_em_componentes;

    for (list<vertice*>::const_iterator i = this->V.begin(); i != this->V.end(); ++i) {

        if (find(vertices_ja_presentes_em_componentes.begin(), vertices_ja_presentes_em_componentes.end(), *i) == vertices_ja_presentes_em_componentes.end()) {
            componente_grafo c = this->obter_vertices_alcanveis_por_busca_em_largura(*i);

            comps.push_back(c);

            for (list<vertice*>::const_iterator j = c.begin(); j != c.end(); ++j) {
                vertices_ja_presentes_em_componentes.push_back(*j);
            }
        }

    }
    return comps;
}
