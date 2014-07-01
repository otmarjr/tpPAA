/* 
 * File:   helpers.h
 * Author: Otmar
 *
 * Created on 1 de Julho de 2014, 18:32
 */

#ifndef HELPERS_H
#define	HELPERS_H
#include <string>
using namespace std;
class helpers {
public:
    static string& trim_string(string &texto);
    static string& trim_string_a_direita(string &texto);
    static string& trim_string_a_esquerda(string &texto);
    static void levantar_erro_execucao(string mensagem);
private:

};

#endif	/* HELPERS_H */

