/* 
 * File:   projeto_software.h
 * Author: Otmar
 *
 * Created on 1 de Julho de 2014, 15:41
 */

#ifndef PROJETO_SOFTWARE_H
#define	PROJETO_SOFTWARE_H

#include <string>
#include <list>
#include <map>

using namespace std;

class projeto_software;

typedef map<int, projeto_software*> colecao_projetos_software;


 enum dimensoes_similaridade {
        LINGUAGEM_PROGRAMACAO = 1,
        DESENVOLVEDORES_COMUNS = 2,
        PALAVRAS_CHAVE = 4,
        IDADE_PROJETO = 8,
        TODAS = 15,
        NENHUMA = 0
    };

class projeto_software {
public:

    static colecao_projetos_software carregar_lista_do_arquivo(string caminho_arquivo_info_projetos);
    projeto_software(string linha_info_projetos);
    string para_string() const; // Utilizada para depurar defeitos.
    static void desalocar_lista_projeto_software(list<projeto_software*> lista);
    int identificador() const;
    string linguagem_programacao() const;
    string descricao() const;
    bool modificado_ultimo_ano() const;
    const list<string>& membros() const;
    int calcular_similiaridade_com_outro_projeto(const projeto_software &outro_projeto, list<string> &stop_words) const;
    int calcular_similiaridade_com_outro_projeto(const projeto_software &outro_projeto, list<string> &stop_words, dimensoes_similaridade &dimensoes) const;
private:
    int id;
    string linguagem;
    string descricao_projeto;
    bool ultimo_commit_menos_um_ano;
    list<string> logins_membros;
    list<string> palavras_significativas_na_descricao(list<string> &stop_words) const;
};

#endif	/* PROJETO_SOFTWARE_H */

