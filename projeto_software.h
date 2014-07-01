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

using namespace std;

class projeto_software {
public:
    static list<projeto_software> carregar_lista_do_arquivo(string caminho_arquivo_info_projetos);
    projeto_software(string linha_info_projetos);
    string para_string(); // Utilizada para depurar defeitos.
private:
    int id;
    string linguagem_programacao;
    string descricao_projeto;
    bool ultimo_commit_menos_um_ano;
    list<string> logins_membros;
    
};

#endif	/* PROJETO_SOFTWARE_H */

