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
#include <list>
#include <unordered_map>

map<int, map<int, int> > ocorrencias_empate_cada_no_cabeca_por_peso;

struct criterio_ordenacao_arestas_kruskal {

    bool operator()(const aresta* x, const aresta* y) {
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

    for (list<vertice*>::const_iterator i = this->V.begin(); i != this->V.end(); ++i) {
        this->A.merge((*i)->lista_adjacencia());
    }
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
    auto antes_clusterizacao = chrono::high_resolution_clock::now();
    auto instance_cpu_antes = clock();
    list<cluster_vertices*> clusters = gerar_kruskal_k_clusters(quantidade_clusters);
    auto depois_clusterizacao = chrono::high_resolution_clock::now();
    auto instante_cpu_depois = clock();

    this->total_milissegundos_execucao_tempo_de_parede = (chrono::duration_cast<chrono::milliseconds>(depois_clusterizacao - antes_clusterizacao).count());
    this->total_milissegundos_execucao_tempo_de_cpu = helpers::milissegundos_entre_dois_clocks(instance_cpu_antes, instante_cpu_depois);

    
    
    
    int mediana_arestas = 0; 
    
    if (this->total_arestas_em_clusters > 1){
        for (int i=0;i<total_clusters;++i) cout<<total_arestas_por_cluster[i]<<" ";
        cout<<std::endl;
        sort(this->total_arestas_por_cluster.begin(), this->total_arestas_por_cluster.end());
        for (int i=0;i<total_clusters;++i) cout<<total_arestas_por_cluster[i]<<" ";
        cout<<std::endl;
        mediana_arestas = this->total_arestas_por_cluster[this->total_clusters/2];
    }
    
    int mediana_vertices = 0; 
    
    if (this->total_vertices_em_clusters > 1){
        for (int i=0;i<total_clusters;++i) cout<<total_vertices_por_cluster[i]<<" ";
        cout<<std::endl;
        sort(this->total_vertices_por_cluster.begin(), this->total_vertices_por_cluster.end());
        for (int i=0;i<total_clusters;++i) cout<<total_vertices_por_cluster[i]<<" ";
        cout<<std::endl;
        mediana_vertices = this->total_vertices_por_cluster[this->total_clusters/2];
    }
    
    ofstream f_saida(caminho_arquivo_clusters.c_str());

    if (!f_saida.good() || !f_saida.is_open()) {
        f_saida.close();
        ostringstream oss;
        oss << "Ocorreu um erro ao tentar abrir para escrita o arquivo de saída dos clusters no cmainho '" << caminho_arquivo_clusters << "'. Verifique se você tem permissão de escrita no caminho informado e se nehum outro processo está utilizando o arquivo.";
        helpers::levantar_erro_execucao(oss.str());
    } else {

        list<cluster_vertices*> outliers;
        list<cluster_vertices*> clusters_com_tamanho_minimo;


        for (list<cluster_vertices*>::iterator i = clusters.begin(); i != clusters.end(); ++i) {
            cluster_vertices *c = *i;

            if (this->conjunto_forma_outlier(c->size(), quantidade_clusters)) {
                outliers.push_back(c);
            } else {
                clusters_com_tamanho_minimo.push_back(c);
            }
        }

        auto imprimir = [&f_saida] (char caractere, int comprimento) {
            for (auto i = 0; i < comprimento; ++i) f_saida << caractere;
            f_saida << std::endl;
        };

        f_saida << "Análise da clusterização" << std::endl;

        imprimir('=', 50);

        f_saida << "Estatísticas globais " << std::endl;

        imprimir('-', 50);

        f_saida << "Total de vértices (n): " << this->V.size();
        f_saida << std::endl << "Total de arestas (m): " << this->A.size() / 2; // grafo não direcionado
        f_saida << std::endl << "Total de componentes conectados do grafo: " << this->todos_componentes_grafo.size();
        f_saida << std::endl << "Total de clusters gerados: "<<this->total_clusters;
        f_saida << std::endl << "Total de vértices em clusters: "<<this->total_vertices_em_clusters;
        f_saida << std::endl << "Total de arestas em clusters: "<<this->total_arestas_em_clusters;
        f_saida << std::endl << "Máximo nº vértices em um cluster: " << this->max_vertices_em_um_cluster;
        f_saida << std::endl << "Média de vértices por cluster: "<<(this->total_vertices_em_clusters/this->total_clusters);
        f_saida << std::endl << "Mediana nº vértices por cluster: "<<mediana_vertices;
        f_saida << std::endl << "Menor nº vértices em um cluster: " << this->min_vertices_em_um_cluster;
        
        f_saida << std::endl << "Máximo nº arestas em um cluster: " << this->max_arestas_em_um_cluster;
        f_saida << std::endl << "Média de arestas por cluster: "<<(this->total_arestas_em_clusters/this->total_clusters);
        f_saida << std::endl << "Mediana nº arestas por cluster: "<<mediana_arestas;
        f_saida << std::endl << "Menor nº arestas em um cluster: " << this->min_arestas_em_um_cluster;
        

        f_saida << std::endl << std::endl << "Tempos de execução" << std::endl;

        imprimir('-', 50);

        f_saida << "Milissegundos de tempo de parede decorridos com execução: " << this->total_milissegundos_execucao_tempo_de_parede;
        f_saida << std::endl << "Milissegundos de tempo de CPU decorridos com execução: " << this->total_milissegundos_execucao_tempo_de_cpu;
        f_saida << std::endl << "Milissegundos de tempo de CPU gastos com busca de componentes via busca em largura: " << this->total_milisegundos_carga_componentes;
        f_saida << std::endl << "Milissegundos de tempo de CPU gastos com balanceamento da ordenação: " << this->total_milissegundos_balanceamento;
        f_saida << std::endl << "Milissegundos de tempo de CPU gastos com clusterização: " << this->total_milissegundos_execucao_clusterizacao;


        map<string, int> ocorrencias_globais_lps = this->coletar_estatisticas_linguagens_projetos(projetos, this->V);
        map<string, int> ocorrencias_globais_devs = this->coletar_estatisticas_devs_projetos(projetos, this->V);
        map<bool, int> ocorrencias_globais_idade = this->coletar_estatisticas_ultimo_commit_projetos(projetos, this->V);
        map<string, int> ocorrencias_globais_palavras_chave = this->coletar_estatisticas_palavras_chave_projetos(projetos, this->V, stop_words);

        f_saida << std::endl << std::endl << "Detalhes dos projetos do grafo" << std::endl;
        imprimir('-', 50);
        string lista_todos_devs = helpers::sumarizar_entradas_dicionario(ocorrencias_globais_devs);
        f_saida << std::endl << "\t" << " Total desenvolvedores: " << ocorrencias_globais_devs.size() << std::endl << "\t\t Frequência de cada desenvolvedor: " << lista_todos_devs;

        string lista_todas_lps = helpers::sumarizar_entradas_dicionario(ocorrencias_globais_lps);
        f_saida << std::endl << "\t" << " Total linguagens de programação: " << ocorrencias_globais_lps.size() << std::endl << "\t\t Frequência de cada linguagem de programação: " << lista_todas_lps;

        string lista_todas_palavras_chave = helpers::sumarizar_entradas_dicionario(ocorrencias_globais_palavras_chave);
        f_saida << std::endl << "\t" << " Total palavras chave: " << ocorrencias_globais_palavras_chave.size() << std::endl << "\t\t Frequência de cada palavra chave: " << lista_todas_palavras_chave;

        string lista_todas_idades = helpers::sumarizar_entradas_dicionario(ocorrencias_globais_idade);
        f_saida << std::endl << "\t" << " Total critérios idade: " << ocorrencias_globais_idade.size() << std::endl << "\t\t Frequência desde último commit no projeto: " << lista_todas_idades;

        f_saida << std::endl << std::endl;
        imprimir('=', 50);
        f_saida << "Resultados da Clusterização" << std::endl;
        imprimir('-', 50);

        int cont = 0;
        for (list<componente_grafo*>::iterator i = this->todos_componentes_grafo.begin(); i != this->todos_componentes_grafo.end(); ++i) {
            int total_arestas = aresta::total_de_arestas_na_lista(**i, false);
            imprimir('-', 50);
            f_saida << "Componente " << ++cont << " - Total de " << (*i)->size() << " vértice(s) e " << total_arestas << " aresta(s)." << std::endl;
            imprimir('-', 50);

            f_saida << std::endl << "Tempos de execução" << std::endl;
            imprimir('-', 50);
            f_saida << "Milissegundos de tempo de CPU gastos com busca de componentes via busca em largura: " << this->milissegundos_busca_em_largura_por_componente[*i];
            f_saida << std::endl << "Milissegundos de tempo de CPU gastos com balanceamento da ordenação deste componente : " << this->milissegundos_balanceamento_ordenacao_por_componente[*i];
            f_saida << std::endl << "Milissegundos de tempo de CPU gastos com clusterização deste componente: " << this->milissegundos_clusterizacao_por_componente[*i];

            f_saida << std::endl << "Vértices no componente: ";

            int cont_vertice = 0;

            for (componente_grafo::iterator j = (*i)->begin(); j != (*i)->end(); ++j) {
                f_saida << ++cont_vertice << ") " << (*j)->identificador() << " ";
            }

            f_saida << std::endl;
            imprimir('-', 50);

            f_saida << std::endl << "Detalhes dos projetos do componente " << cont << std::endl;
            imprimir('-', 50);

            map<string, int> lps_componente = this->coletar_estatisticas_linguagens_projetos(projetos, **i);
            map<string, int> devs_componente = this->coletar_estatisticas_devs_projetos(projetos, **i);
            map<bool, int> idades_componente = this->coletar_estatisticas_ultimo_commit_projetos(projetos, **i);
            map<string, int> palavras_chave_componente = this->coletar_estatisticas_palavras_chave_projetos(projetos, **i, stop_words);

            string texto_todos_devs = helpers::sumarizar_entradas_dicionario(devs_componente);
            f_saida << "\t" << " Total desenvolvedores: " << devs_componente.size() << std::endl << "\t\t\t Frequência de cada desenvolvedor: " << texto_todos_devs;

            string texto_todas_lps = helpers::sumarizar_entradas_dicionario(lps_componente);
            f_saida << std::endl << "\t" << " Total linguagens de programação: " << lps_componente.size() << std::endl << "\t\t\t Frequência de cada linguagem de programação: " << texto_todas_lps;

            string texto_todas_keywords = helpers::sumarizar_entradas_dicionario(palavras_chave_componente);
            f_saida << std::endl << "\t" << " Total palavras chave: " << palavras_chave_componente.size() << std::endl << "\t\t\t Frequência de cada palavra chave: " << texto_todas_keywords;

            string texto_todas_idades = helpers::sumarizar_entradas_dicionario(idades_componente);
            f_saida << std::endl << "\t" << " Total critérios idade: " << idades_componente.size() << std::endl << "\t\t\t Frequência projetos com commit há menos de um ano: " << texto_todas_idades;

            list<cluster_vertices*> clusters_comp = this->clusters_dos_componentes[*i];

            f_saida << std::endl << std::endl << "Clusters do componente " << cont << " ";

            bool conjunto_de_outliers = this->conjunto_forma_outlier(total_arestas, quantidade_clusters);

            if (conjunto_de_outliers) {
                f_saida << " - Nenhum, pois não possui número de arestas suficiente." << std::endl;
            } else {
                f_saida << " - " << clusters_comp.size() << " gerados.";

                f_saida << std::endl;
                imprimir('-', 50);
                int contador_clusters = 0;

                for (list<cluster_vertices*>::iterator j = clusters_comp.begin(); j != clusters_comp.end(); ++j) {
                    cluster_vertices *cluster = *j;
                    list<vertice*> lista(cluster->begin(), cluster->end());
                    int total_arestas_cluster_internas = aresta::total_de_arestas_na_lista(lista, false);
                    int total_arestas_cluster_externas = aresta::total_de_arestas_na_lista(lista, true);

                    f_saida << std::endl << "\tCluster " << ++contador_clusters << " - " << cluster->size() << " vértice(s) e " << total_arestas_cluster_internas << " aresta(s) interna(s) e " << (total_arestas_cluster_externas - total_arestas_cluster_internas) << " externa(s)." << std::endl;

                    f_saida << '\t';
                    imprimir('-', 50);

                    f_saida << "\t\tVértices no cluster: ";

                    cont_vertice = 0;
                    for (cluster_vertices::iterator k = cluster->begin(); k != cluster->end(); ++k) {
                        f_saida << ++cont_vertice << ") " << (*k)->identificador() << " ";
                    }

                    map<string, int> lps_cluster = this->coletar_estatisticas_linguagens_projetos(projetos, lista);
                    map<string, int> devs_cluster = this->coletar_estatisticas_devs_projetos(projetos, lista);
                    map<bool, int> idades_cluster = this->coletar_estatisticas_ultimo_commit_projetos(projetos, lista);
                    map<string, int> palavras_cluster = this->coletar_estatisticas_palavras_chave_projetos(projetos, lista, stop_words);

                    string texto_todos_devs = helpers::sumarizar_entradas_dicionario(devs_cluster);
                    f_saida << std::endl << "\t\t" << " Total desenvolvedores: " << devs_cluster.size() << std::endl << "\t\t\t Frequência de cada desenvolvedor: " << texto_todos_devs;

                    string texto_todas_lps = helpers::sumarizar_entradas_dicionario(lps_cluster);
                    f_saida << std::endl << "\t\t" << " Total linguagens de programação: " << lps_cluster.size() << std::endl << "\t\t\t Frequência de cada linguagem de programação: " << texto_todas_lps;

                    string texto_todas_keywords = helpers::sumarizar_entradas_dicionario(palavras_cluster);
                    f_saida << std::endl << "\t\t" << " Total palavras chave: " << palavras_cluster.size() << std::endl << "\t\t\t Frequência de cada palavra chave: " << texto_todas_keywords;

                    string texto_todas_idades = helpers::sumarizar_entradas_dicionario(idades_cluster);
                    f_saida << std::endl << "\t\t" << " Total critérios idade: " << idades_cluster.size() << std::endl << "\t\t\t Frequência projetos com commit há menos de um ano: " << texto_todas_idades;
                    f_saida << std::endl;
                }
            }
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

    auto antes_carga_componentes = clock();

    this->carregar_todos_componentes_grafo();

    auto depois_carga_componentes = clock();

    this->total_milisegundos_carga_componentes = helpers::milissegundos_entre_dois_clocks(antes_carga_componentes, depois_carga_componentes);

    this->total_milissegundos_balanceamento = 0;
    this->total_milissegundos_execucao_clusterizacao = 0;

    this->total_arestas_em_clusters = 0;
    this->total_vertices_em_clusters = 0;
    this->total_clusters = 0;

    for (list<componente_grafo*>::iterator i = this->todos_componentes_grafo.begin(); i != this->todos_componentes_grafo.end(); ++i) {

        componente_grafo *c = *i;
        list<aresta*> arestas = this->lista_de_arestas_no_componente(c);
        list<vertice*> vertices_clusterizacao = this->lista_de_vertices_no_componente(c);

        long segundos_clusterizacao_deste_componente = 0;
        long segundos_balanceamento_ordenacao_deste_componente = 0;

        bool componente_possui_tamanho_minimo_para_clusterizar = !this->conjunto_forma_outlier(arestas.size(), k);


        if (componente_possui_tamanho_minimo_para_clusterizar) {

            this->total_clusters += k;

            // Evita que alguns vértices sejam descobertos apenas no final, caso
            // comum caso ocorram muitos empates e as arestas fiquem ordenadas pelo
            // vértice de origem. Isto induz k-1 clusters de tamanho 1.
            auto instante_antes_ordenacao = clock();
            list<aresta*> arestas_balanceadas = this->balancear_ocorrencias_vertices_em_empates_na_lista_ordenada(arestas);
            auto instante_depois_ordenacao = clock();
            segundos_balanceamento_ordenacao_deste_componente = helpers::milissegundos_entre_dois_clocks(instante_antes_ordenacao, instante_depois_ordenacao);

            auto instante_antes_clusterizacao = clock();


            union_find *unf = new union_find(vertices_clusterizacao);



            while (unf->total_conjuntos() > k) {

                aresta* menor_aresta = arestas_balanceadas.front();

                arestas_balanceadas.pop_front();

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

            auto instante_depois_clusterizacao = clock();

            segundos_clusterizacao_deste_componente = helpers::milissegundos_entre_dois_clocks(instante_antes_clusterizacao, instante_depois_clusterizacao);
            list<cluster_vertices*> clusters_componente = unf->clusters();
            


            for (list<cluster_vertices*>::iterator ck = clusters_componente.begin(); ck != clusters_componente.end(); ++ck) {
                cluster_vertices *cluster = *ck;
                
                int total_vertices_neste_cluster = cluster->size();
                int total_arestas_neste_cluster = aresta::total_de_arestas_no_cluster(cluster);
                
                if (total_arestas_neste_cluster > this->max_arestas_em_um_cluster) {
                    this->max_arestas_em_um_cluster = total_arestas_neste_cluster;
                }

                if (total_vertices_neste_cluster > this->max_vertices_em_um_cluster) {
                    this->max_vertices_em_um_cluster = total_vertices_neste_cluster;
                }

                if (total_arestas_neste_cluster < this->min_arestas_em_um_cluster) {
                    this->min_arestas_em_um_cluster = total_arestas_neste_cluster;
                }

                if (total_vertices_neste_cluster < this->min_vertices_em_um_cluster) {
                    this->min_vertices_em_um_cluster = total_vertices_neste_cluster;
                }
                this->total_arestas_por_cluster.push_back(total_arestas_neste_cluster);
                this->total_vertices_por_cluster.push_back(total_vertices_neste_cluster);
                
                this->total_arestas_em_clusters += total_arestas_neste_cluster;
                this->total_vertices_em_clusters += total_vertices_neste_cluster;
            }
            
            this->clusters_dos_componentes[c] = clusters_componente;
            clusters.merge(clusters_componente);

        } else {
            cluster_vertices *cluster_pequeno = new cluster_vertices();

            for (componente_grafo::iterator j = c->begin(); j != c->end(); ++j) {
                cluster_pequeno->insert(*j);
            }

            clusters.push_back(cluster_pequeno);
            list<cluster_vertices*> cluster;
            cluster.push_back(cluster_pequeno);
            this->clusters_dos_componentes[c] = cluster;
        }

        this->total_milissegundos_execucao_clusterizacao += segundos_clusterizacao_deste_componente;
        this->milissegundos_clusterizacao_por_componente.insert(make_pair(c, segundos_clusterizacao_deste_componente));

        this->milissegundos_balanceamento_ordenacao_por_componente.insert(make_pair(c, segundos_balanceamento_ordenacao_deste_componente));
        this->total_milissegundos_balanceamento += segundos_balanceamento_ordenacao_deste_componente;
    }


    return clusters;
}

componente_grafo* grafo::obter_vertices_alcancaveis_por_busca_em_largura(vertice* vertice_inicial_busca) {
    componente_grafo* c = new componente_grafo();

    unordered_map < vertice*, bool> descobertos;
    vector<set<vertice*> > camadas;

    for (list<vertice*>::const_iterator i = this->V.begin(); i != this->V.end(); ++i) {
        descobertos[*i] = false;
    }

    descobertos[vertice_inicial_busca] = true;
    set<vertice*> camada_atual;
    camada_atual.insert(vertice_inicial_busca);
    camadas.push_back(camada_atual);

    int contador_camada = 0;

    lista_arestas arestas_componente;
    unordered_map<int, bool> arestas_encontradas;

    while (!camada_atual.empty()) {
        camada_atual.clear();

        for (set<vertice*>::iterator i = camadas[contador_camada].begin(); i != camadas[contador_camada].end(); ++i) {
            vertice *u = *i;
            c->push_back(u);
            this->componentes_dos_vertices.insert(make_pair(u, c));

            list<aresta*> adj_u = u->lista_adjacencia();

            for (list<aresta*>::const_iterator j = adj_u.begin(); j != adj_u.end(); ++j) {
                aresta *a = *j;

                if (arestas_encontradas.count(a->identificador_aresta()) == 0) {
                    arestas_encontradas.insert(make_pair(a->identificador_aresta(), true));
                    arestas_componente.push_back(a);
                }

                vertice *v = a->extremidade_y();

                if (descobertos[v] == false) {
                    descobertos[v] = true;
                    camada_atual.insert(v);
                }
            }
        }

        if (camada_atual.size() > 0) {
            contador_camada++;
            camadas.push_back(camada_atual);
        }

    }

    this->arestas_dos_componentes.insert(make_pair(c, arestas_componente));
    return c;
}

void grafo::carregar_todos_componentes_grafo() {
    this->todos_componentes_grafo.clear();

    for (list<vertice*>::const_iterator i = this->V.begin(); i != this->V.end(); ++i) {

        if (this->componentes_dos_vertices.count(*i) == 0) {
            auto antes_busca_largura_componente = clock();
            componente_grafo *c = this->obter_vertices_alcancaveis_por_busca_em_largura(*i);
            auto depois_busca_largura_componente = clock();
            long segundos_busca_largura = helpers::milissegundos_entre_dois_clocks(antes_busca_largura_componente, depois_busca_largura_componente);
            this->milissegundos_busca_em_largura_por_componente.insert(make_pair(c, segundos_busca_largura));
            this->todos_componentes_grafo.push_back(c);
        }
    }
}

bool grafo::conjunto_forma_outlier(int tamanho_conjunto, int quantidade_clusters) {
    return tamanho_conjunto < quantidade_clusters;
}

map<string, int> grafo::coletar_estatisticas_devs_projetos(colecao_projetos_software& projetos, list<vertice*> &vertices) {
    map<string, int> estatisticas;
    for (list<vertice*>::iterator i = vertices.begin(); i != vertices.end(); ++i) {
        projeto_software *p = projetos[(*i)->identificador()];

        for (list<string>::const_iterator k = p->membros().begin(); k != p->membros().end(); ++k) {
            if (estatisticas.find(*k) == estatisticas.end()) {
                estatisticas[*k] = 1;
            } else {
                estatisticas[*k] += 1;
            }
        }
    }

    return estatisticas;
}

map<string, int> grafo::coletar_estatisticas_linguagens_projetos(colecao_projetos_software& projetos, list<vertice*> &vertices) {
    map<string, int> estatisticas;
    for (list<vertice*>::iterator i = vertices.begin(); i != vertices.end(); ++i) {

        int id = (*i)->identificador();

        if (projetos.count(id) == 0) {
            stringstream ss;
            ss << "As estatísticas não são precisas porque o projeto " << id << " não foi encontrado no arquivo de informações dos projetos.";
            helpers::escrever_alerta(ss.str());
            continue;
        }

        projeto_software *p = projetos[(*i)->identificador()];

        if (estatisticas.find(p->linguagem_programacao()) == estatisticas.end()) {
            estatisticas[p->linguagem_programacao()] = 1;
        } else {
            estatisticas[p->linguagem_programacao()] += 1;
        }
    }

    return estatisticas;
}

map<string, int> grafo::coletar_estatisticas_palavras_chave_projetos(colecao_projetos_software& projetos, list<vertice*> &vertices, list<string> &stop_words) {

    map<string, int> estatisticas;
    for (list<vertice*>::iterator i = vertices.begin(); i != vertices.end(); ++i) {

        int id = (*i)->identificador();

        if (projetos.count(id) == 0) {
            stringstream ss;
            ss << "As estatísticas não são precisas porque o projeto " << id << " não foi encontrado no arquivo de informações dos projetos.";
            helpers::escrever_alerta(ss.str());
            continue;
        }

        projeto_software *p = projetos[(*i)->identificador()];

        list<string> palavras_chave = p->palavras_significativas_na_descricao(stop_words);

        for (list<string>::iterator k = palavras_chave.begin(); k != palavras_chave.end(); ++k) {
            if (estatisticas.find(*k) == estatisticas.end()) {
                estatisticas[*k] = 1;
            } else {
                estatisticas[*k] += 1;
            }
        }
    }

    return estatisticas;
}

map<bool, int> grafo::coletar_estatisticas_ultimo_commit_projetos(colecao_projetos_software& projetos, list<vertice*> &vertices) {
    map<bool, int> estatisticas;
    for (list<vertice*>::iterator i = vertices.begin(); i != vertices.end(); ++i) {

        int id = (*i)->identificador();
        if (projetos.count(id) == 0) {
            stringstream ss;
            ss << "As estatísticas não são precisas porque o projeto " << id << " não foi encontrado no arquivo de informações dos projetos.";
            helpers::escrever_alerta(ss.str());
            continue;
        }

        projeto_software *p = projetos[(*i)->identificador()];

        if (estatisticas.find(p->modificado_ultimo_ano()) == estatisticas.end()) {
            estatisticas[p->modificado_ultimo_ano()] = 1;
        } else {
            estatisticas[p->modificado_ultimo_ano()] += 1;
        }
    }

    return estatisticas;
}

aresta* grafo::aresta_equivalente_sentido_oposto(aresta* a) {
    return a->extremidade_y()->aresta_para_vertice(*(a->extremidade_x()));
}

// evita que a árvore geradora tenha clusters de tamanho 1 caso alguns vértices
// só apareçam pela primeira vez nas últimas arestas.

list<aresta*> grafo::balancear_ocorrencias_vertices_em_empates_na_lista_ordenada(lista_arestas& arestas) {
    arestas.sort(criterio_ordenacao_arestas_kruskal());

    // Contabiliza a distribuição de pesos diferentes:
    int ultimo_peso = 0;
    map<vertice*, map<int, lista_arestas> > arestas_partindo_vertice;
    map<int, map<vertice*, lista_arestas> > pesos_e_vertices;

    map<int, int> distribuicao_pesos_quantidades_arestas;
    map<int, lista_arestas> distribuicao_pesos_arestas;
    map<int, list<vertice*> > distribuicao_pesos_vertices;

    for (list<aresta*>::const_iterator j = arestas.begin(); j != arestas.end(); ++j) {
        aresta *a = *j;

        if (a->peso() != ultimo_peso) {
            ultimo_peso = a->peso();
            list<aresta*> l;
            list<vertice*> lv;

            l.push_back(a);
            lv.push_back(a->extremidade_x());
            lv.push_back(a->extremidade_y());
            distribuicao_pesos_quantidades_arestas[ultimo_peso] = 1;
            distribuicao_pesos_arestas.insert(make_pair(ultimo_peso, l));
            distribuicao_pesos_vertices.insert(make_pair(ultimo_peso, lv));

        } else {
            distribuicao_pesos_quantidades_arestas[ultimo_peso] += 1;
            distribuicao_pesos_arestas[ultimo_peso].push_back(a);

            if (find(distribuicao_pesos_vertices[ultimo_peso].begin(), distribuicao_pesos_vertices[ultimo_peso].end(), a->extremidade_x()) == distribuicao_pesos_vertices[ultimo_peso].end()) {
                distribuicao_pesos_vertices[ultimo_peso].push_back(a->extremidade_x());
            }

            if (find(distribuicao_pesos_vertices[ultimo_peso].begin(), distribuicao_pesos_vertices[ultimo_peso].end(), a->extremidade_y()) == distribuicao_pesos_vertices[ultimo_peso].end()) {
                distribuicao_pesos_vertices[ultimo_peso].push_back(a->extremidade_y());
            }
        }

        if (arestas_partindo_vertice.count(a->extremidade_x()) == 0) {
            lista_arestas la;
            la.push_back(a);
            map<int, lista_arestas> m;
            m.insert(make_pair(a->peso(), la));

            arestas_partindo_vertice[a->extremidade_x()] = m;
        } else {

            map<int, lista_arestas> m = arestas_partindo_vertice[a->extremidade_x()];

            if (m.count(a->peso()) == 0) {
                lista_arestas la;
                la.push_back(a);
                m.insert(make_pair(a->peso(), la));
            } else {
                m[a->peso()].push_back(a);
            }

            arestas_partindo_vertice[a->extremidade_x()] = m;
        }

        if (pesos_e_vertices[a->peso()].count(a->extremidade_x()) == 0) {
            map<vertice*, lista_arestas> m;
            lista_arestas la;
            la.push_back(a);
            pesos_e_vertices[a->peso()].insert(make_pair(a->extremidade_x(), la));
        } else {
            pesos_e_vertices[a->peso()][a->extremidade_x()].push_back(a);
        }
    }

    list<aresta*> reordenacao;

    for (map<int, lista_arestas>::reverse_iterator j = distribuicao_pesos_arestas.rbegin(); j != distribuicao_pesos_arestas.rend(); ++j) {
        lista_arestas arestas_com_peso_atual = j->second;

        list<vertice*> vertices_das_arestas = distribuicao_pesos_vertices[j->first];

        int peso_atual = j->first;

        if (vertices_das_arestas.size() != 2 * arestas_com_peso_atual.size()) {

            list<aresta*> rearranjo;

            bool ainda_ha_aresta_pra_reordenar = true;

            while (ainda_ha_aresta_pra_reordenar) {
                ainda_ha_aresta_pra_reordenar = false;

                for (map<vertice*, lista_arestas>::iterator k = pesos_e_vertices[peso_atual].begin(); k != pesos_e_vertices[peso_atual].end(); ++k) {


                    if (k->second.size() > 0) {
                        rearranjo.push_back(k->second.front());
                        k->second.pop_front();
                    }

                    ainda_ha_aresta_pra_reordenar = ainda_ha_aresta_pra_reordenar || k->second.size() > 0;
                }

            }

            copy(rearranjo.begin(), rearranjo.end(), back_inserter(reordenacao));
        } else {
            copy(arestas_com_peso_atual.begin(), arestas_com_peso_atual.end(), back_inserter(reordenacao));
        }
    }

    return reordenacao;
}

list<aresta*> grafo::lista_de_arestas_no_componente(componente_grafo* c) {
    return this->arestas_dos_componentes[c];
}

list<vertice*> grafo::lista_de_vertices_no_componente(componente_grafo* c) {

    list<vertice*> vertices;

    for (list<vertice*>::iterator i = c->begin(); i != c->end(); ++i) {
        vertice *v = *i;

        if (find(vertices.begin(), vertices.end(), v) == vertices.end()) {
            vertices.push_back(v);
        }
    }

    return vertices;
}
