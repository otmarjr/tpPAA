/* 
 * File:   helpers.h
 * Author: Otmar
 *
 * Created on 1 de Julho de 2014, 18:32
 */

#ifndef HELPERS_H
#define	HELPERS_H
#include <string>
#include <list>
using namespace std;
class helpers {
public:
    static string& trim_string(string &texto);
    static string& trim_string_a_direita(string &texto);
    static string& trim_string_a_esquerda(string &texto);
    static string& normalizar_string_para_minusculas(string& texto); //util para comparacoes de texto.
    static string& retirar_ultimo_caractere_se_presente(string& texto, char caractere);
    static bool strings_sao_equivalentes(string& x, string& y);
    static int contar_numero_strings_em_comum(list<string> &A,
    list<string> &B);
    static void levantar_erro_execucao(string mensagem);
    static list<string> carregar_linhas_arquivo(string caminho_arquivo);
    static bool string_is_int(string texto);
    
};

#endif	/* HELPERS_H */

