/* 
 * File:   main.cpp
 * Author: Otmar
 *
 * Created on 30 de Junho de 2014, 10:37
 */

#include "projeto_software.h"
#include <cstdlib>
#include <iostream>

using namespace std;

/*
 * 
 */


static void testar_entradas()
{
    projeto_software* p1 = new projeto_software("34707|Shell|19/x çalışma ortamı|2013-09-10 16:15:09| ");
    projeto_software* p2 = new projeto_software("3586|JavaScript|Rapid Interface Builder (RIB) is a browser-based design tool for quickly prototyping and creating the user interface for web applications. Layout your UI by dropping widgets onto a canvas. Run the UI in an interactive \"Preview mode\". Export the generated |2012-09-17 23:08:15|zhizhangchen, grgustaf, sbryan, bspencer, otcuser");
    projeto_software* p3 = new projeto_software("1431411|C||2014-01-31 00:39:50|bpeel, rib, dlespiau");
    
    cout<<"\nLeitura de p1: "<<p1->para_string();
    cout<<"\nLeitura de p2: "<<p2->para_string();
    cout<<"\nLeitura de p3: "<<p2->para_string();
}
int main(int argc, char** argv) {

    testar_entradas();
    return 0;
}

