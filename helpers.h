/* 
 * File:   helpers.h
 * Author: Otmar
 *
 * Created on 1 de Julho de 2014, 18:32
 */

#ifndef HELPERS_H
#define	HELPERS_H


#ifdef DEBUG
#include <iostream>
#define ESCREVER_TRACE(msg) std::cout<<"Mensagem recebida para depuração: "<<msg<<endl;
#else
#define ESCREVER_TRACE(msg)
#endif

#include <string>
#include <list>
#include <map>
#include <chrono>

using namespace std;



class helpers {
public:
    static string& trim_string(string &texto);
    static string& trim_string_a_direita(string &texto);
    static string& trim_string_a_esquerda(string &texto);
    static string& normalizar_string_para_minusculas(string& texto); //util para comparacoes de texto.
    static string& retirar_ultimo_caractere_se_presente(string& texto, char caractere);
    static string para_caixa_alta(string &texto);
    static string para_caixa_baixa(string &texto);
    static bool strings_sao_equivalentes(string& x, string& y);
    static int contar_numero_strings_em_comum(list<string> &A,
    list<string> &B);
    static void levantar_erro_execucao(string mensagem);
    static void escrever_alerta(string mensagem);
    static list<string> carregar_linhas_arquivo(string caminho_arquivo);
    static bool string_e_inteiro(string texto);
    static int string_para_inteiro(string& texto);
    static char * strptime_port_windows(const char *s, const char *format, struct tm *tm);
    static string sumarizar_entradas_dicionario(map<string, int> dicionario);
    static string sumarizar_entradas_dicionario(map<bool, int> dicionario);
    static long segundos_entre_dois_clocks(clock_t &antes, clock_t &depois);
    static long milissegundos_entre_dois_clocks(clock_t &antes, clock_t &depois);
};




#endif	/* HELPERS_H */

