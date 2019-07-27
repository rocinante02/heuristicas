#include "readData.h"
#include "CustoIn.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <limits>
#include <random>


using namespace std;

double ** matrizAdj; // matriz de adjacencia
double ** matrizDist; // matriz reorganizada;
int dimension; // quantidade total de vertices
vector<int> cidades;
vector<int> solucaum;
vector<int> candidatos;

void printData();
vector<int> construction(double alfa);
void printSolution(vector<int> anyVector);
void candidates();
bool comp(const CustoIn& a, const CustoIn& b);
vector<CustoIn> calculaCusto(vector<int> listaCandidatos, vector<int> &s);
void swap(vector<int> &solucao);
double calculaDeltaSwap(int i, int j, vector<int> &s);
void reInsertion(vector<int> &solucao);
double custoTotal(vector<int> &solucao);
double calculaDeltaReInsertion(int i, int j, vector<int> &s);
void twoOptN(vector<int> &solucao);
double calculaDeltaTwoOpt(int i, int j, vector<int> &s);
void orOpt2(vector<int> &solucao);
double calculaDeltaOrOpt2(int i, int j, vector<int> &s);
void orOpt3(vector<int> &solucao);
double calculaDeltaOrOpt3(int i, int j, vector<int> &s);
void rvnd(vector<int> &solucao);
vector<int> perturb(vector<int> &solucao);
vector<int> gils_rvnd(int i_max, int i_ils);


int main(int argc, char** argv) {

    
    double valor;
    int i_max = 50;
    int i_ils;
    readData(argc, argv, &dimension, &matrizAdj);
    
    srand((unsigned)time(0));

    if(dimension >= 150){
      i_ils = dimension/2;
    }
    else{
      i_ils = dimension;
    }

    //candidatos = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    //printSolution(candidatos);
    //rvnd(cidades);
    //printSolution(cidades);
    //printSolution(candidatos);
    //perturb(candidatos);
    cidades = gils_rvnd(i_max, i_ils);
    printSolution(cidades);
    valor = custoTotal(cidades);
    cout << "custo: " << custoTotal(cidades) << endl;
    //cidades = construction(0.5);
    //swap(cidades);
    //printData();
    return 0;

    
}

void printData() {
  cout << "dimension: " << dimension << endl;
  for (size_t i = 1; i <= dimension; i++) {
    for (size_t j = 1; j <= dimension; j++) {
      cout << matrizAdj[i][j] << " ";
    }
    cout << endl;
  }
}

vector<int> construction(double alfa){ 
  vector<int> s = {1,1};// lista de cidades da solução inicial
  vector<int> listaCandidatos;
  for(int i = 2; i <= dimension; i++){
    listaCandidatos.push_back(i); // insere todos os nós na lista de candidatos
  }

  for(int j = 0; j < 3; j++){ // tamamho subsequqncia inicial
    int k = rand() % listaCandidatos.size();
    s.insert(s.begin() + 1, listaCandidatos[k]); // adiciona cidade aleatoria a solução
    listaCandidatos.erase(listaCandidatos.begin() + k); // apaga da lista de candidatos oq ja foi pra solução
  }

  vector<CustoIn> custoInsertion = calculaCusto(listaCandidatos, s); // calcula os custo de inserção dos candidatos 
  std::sort(custoInsertion.begin(), custoInsertion.end(), comp); // ordena de forma crescente de acordo com os custos

  int sel;
  while(!listaCandidatos.empty()){
    if(alfa == 0){
      sel = 0;
    }
    else{
      sel = rand() % ((int)std::floor(alfa * (custoInsertion.size() - 1)) + 1); // escolhe um nó dentro de uma faixa definida por alfa
    }

    //cout << "sel: " << sel << endl;

    s.insert(s.begin() + custoInsertion[sel].arestaOut + 1, custoInsertion[sel].noIn); // insere o nó na solucao
    
    for(int i = 0; i < listaCandidatos.size(); i++){
      if(listaCandidatos[i]==custoInsertion[sel].noIn)
        listaCandidatos.erase(listaCandidatos.begin() + i); // exclui o nó da lista de candidatos
    }
    
    custoInsertion.erase(custoInsertion.begin(), custoInsertion.begin() + custoInsertion.size()); // exclui o nó da lista de custos
    custoInsertion = calculaCusto(listaCandidatos, s); // calcula os novos custos de inserção
    std::sort(custoInsertion.begin(), custoInsertion.end(), comp); // ordena os custos

  }

  return s;

}


void printSolution(vector<int> anyVector){ // printa um vetor qualquer
   vector<int>::iterator v = anyVector.begin(); 
   while( v != anyVector.end()) {
      cout << *v << endl;
      v++;
   }
}

 bool comp(const CustoIn& a, const CustoIn& b) // comparação dos custos utilizada para ordenar os objetos
  {
    return a.custo < b.custo;
  }

vector<CustoIn> calculaCusto(vector<int> listaCandidatos, vector<int> &s){
  vector<CustoIn> custoInsertion ((s.size() - 1) * listaCandidatos.size());
  for(int i = 0, j = 1, l = 0; i < s.size()-1; i++, j++){
    for(auto k : listaCandidatos){
      custoInsertion[l].custo = matrizAdj[s[i]][k] +  matrizAdj[s[j]][k] - matrizAdj[s[i]][s[j]]; // custo
      custoInsertion[l].noIn = k; // nó inserido
      custoInsertion[l].arestaOut = i; // posicao de inserção;
      l++;
    }
  }
  return custoInsertion;


}

void swap(vector<int> &solucao){ // faz a troca de posição entre dois nós
  vector<int> s = solucao;
  double delta;
  double menor = std::numeric_limits<double>::infinity();
  //double fs = custoTotal(s);
  int pos_i = -1, pos_j = -1; // guarda as posições para realizar a operação
  for(int i = 1; i < solucao.size() - 1; i++){ // exclui da operação a primeira e a ultima posição do vetor
    for(int j = i + 1; j < solucao.size() - 1; j++){
      //if(i <= j){ // nao repetir swap
        //cout << "i:" << i << "j:" << j << endl;
        delta = calculaDeltaSwap(i,j,s);
        //d = custoTotal(solucao) - fs;
        //cout <<"delta real: " << d << " delta calc: " << delta << endl;
        if(delta <= menor){
            menor = delta;
            pos_i = i;
            pos_j = j;
        }
      //}
    }
  } 
   if(pos_i > 0){ // realiza a operação
    int aux = s[pos_i];
    solucao[pos_i] = s[pos_j];
    solucao[pos_j] = aux;
    
  }
}

 inline double calculaDeltaSwap(int i, int j, vector<int> &s){
  double delta;
    if(i + 1 == j){
      delta = matrizAdj[s[i-1]][s[j]] + matrizAdj[s[i]][s[j+1]] - matrizAdj[s[i-1]][s[j-1]] - matrizAdj[s[i+1]][s[j+1]];
    }
    else{
    delta = matrizAdj[s[i]][s[j-1]] + matrizAdj[s[i]][s[j+1]] + matrizAdj[s[j]][s[i-1]] + matrizAdj[s[j]][s[i+1]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i]][s[i+1]] - matrizAdj[s[j]][s[j-1]] - matrizAdj[s[j]][s[j+1]];
        }
  //cout << "delta swap: " << delta <<endl; 
  return delta; 
}

void reInsertion(vector<int> &solucao){ // reinsere um nó em posição diferente
  vector<int> s = solucao;
  double menor = 0;
  double delta; 
  int pos_i = -1, pos_j = -1;
  for(int i = 1; i < solucao.size() - 1; i++){ // varre a solução exceto o 0 e o final
    for(int j = 1; j < solucao.size() - 1; j++){
      if(i != j){ // exclui a posição inicial do nó
         if(i < j){
            delta = matrizAdj[s[i]][s[j+1]] + matrizAdj[s[i]][s[j]] + matrizAdj[s[i-1]][s[i+1]] - matrizAdj[s[j]][s[j+1]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i]][s[i+1]];
          }
         else{
            delta = matrizAdj[s[i]][s[j-1]] + matrizAdj[s[i]][s[j]] + matrizAdj[s[i-1]][s[i+1]] - matrizAdj[s[j]][s[j-1]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i]][s[i+1]];
          }
        if(delta <= menor){
          menor = delta;
          pos_i = i;
          pos_j = j;
        }
      }
    }
     
  }
  if(pos_i > 0){
    solucao.erase(solucao.begin() + pos_i);
    solucao.insert(solucao.begin() + pos_j, s[pos_i]);
  }  
}

inline double calculaDeltaReInsertion(int i, int j, vector<int> &s){
  double delta;
  if(i <= j){
    delta = matrizAdj[s[i]][s[j+1]] + matrizAdj[s[i]][s[j]] + matrizAdj[s[i-1]][s[i+1]] - matrizAdj[s[j]][s[j+1]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i]][s[i+1]];
            }
  else{
  delta = matrizAdj[s[i]][s[j-1]] + matrizAdj[s[i]][s[j]] + matrizAdj[s[i-1]][s[i+1]] - matrizAdj[s[j]][s[j-1]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i]][s[i+1]];
      }
  return delta;

}

void twoOptN(vector<int> &solucao){ // inverte uma subsequencia da solução
  vector<int> s = solucao;
  double delta , menor = 0;
  int pos_i = -1, pos_j;
    for(int i = 1; i < solucao.size() - 2; i++){
      for(int j = i + 1; j < solucao.size() - 1; j++){
        if(j == (i + 1)){
          delta = matrizAdj[s[i-1]][s[j]] + matrizAdj[s[i]][s[j+1]] - matrizAdj[s[i-1]][s[j-1]] - matrizAdj[s[j]][s[j+1]];
        }
        else{
          delta = matrizAdj[s[i-1]][s[j]] + matrizAdj[s[i]][s[j+1]] - matrizAdj[s[i-1]][s[i]] - matrizAdj[s[j]][s[j+1]];
        }
        if(delta <= menor){
          menor = delta;
          pos_i = i;
          pos_j = j;
          
        }
      }
    }
    if(pos_i > 0){
      for(int k = pos_i; k <= pos_j; k++){ // invertendo as posições
        solucao[k] = s[pos_j + pos_i - k];
      }
    }
}

inline double calculaDeltaTwoOpt(int i, int j, vector<int> &s){
  double delta;
  if(j == (i + 1)){
    delta = matrizAdj[s[i-1]][s[j]] + matrizAdj[s[i]][s[j+1]] - matrizAdj[s[i-1]][s[j-1]] - matrizAdj[s[j]][s[j+1]];
  }
  else{
  delta = matrizAdj[s[i-1]][s[j]] + matrizAdj[s[i]][s[j+1]] - matrizAdj[s[i-1]][s[i]] - matrizAdj[s[j]][s[j+1]];
  }
  return delta;
}

void orOpt2(vector<int> &solucao){ // reinsere subsequencia de dois nós em posição diferente
  vector<int> s = solucao;
  double menor = 0;
  double delta;
  int pos_i = -1, pos_j;
  for(int i = 1; i < solucao.size() - 2; i++){
    for(int j = 1; j < solucao.size() - 3; j++){
      if(i != j){
         if(i < j){
            delta = matrizAdj[s[i-1]][s[i+2]] + matrizAdj[s[i]][s[j+1]] + matrizAdj[s[i+1]][s[j+2]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i+1]][s[i+2]] - matrizAdj[s[j+1]][s[j+2]];
          }
          else { 
            delta = matrizAdj[s[i]][s[j-1]] + matrizAdj[s[i+1]][s[j]] + matrizAdj[s[i+2]][s[i-1]] - matrizAdj[s[j]][s[j-1]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i+1]][s[i+2]];
          }
        if(delta <= menor){
          menor = delta;
          pos_i = i;
          pos_j = j;
        } 
      }  
    }    
  }
  if(pos_i > 0){
    solucao.erase(solucao.begin() + pos_i, solucao.begin() + pos_i + 2);
    solucao.insert(solucao.begin() + pos_j, &s[pos_i], &s[pos_i] + 2);
  }
}

inline double calculaDeltaOrOpt2(int i, int j, vector<int> &s){
  double delta;
  if(i < j){
    delta = matrizAdj[s[i-1]][s[i+2]] + matrizAdj[s[i]][s[j+1]] + matrizAdj[s[i+1]][s[j+2]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i+1]][s[i+2]] - matrizAdj[s[j+1]][s[j+2]];
  }
  else { 
    delta = matrizAdj[s[i]][s[j-1]] + matrizAdj[s[i+1]][s[j]] + matrizAdj[s[i+2]][s[i-1]] - matrizAdj[s[j]][s[j-1]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i+1]][s[i+2]];
  }

  return delta;
}

void orOpt3(vector<int> &solucao){ // reinsere subsequencia de tres nós em posição diferente
  vector<int> s = solucao;
  double menor = 0;
  double delta;
  int pos_i = -1, pos_j;

  for(int i = 1; i < solucao.size() - 3; i++){
    for(int j = 1; j < solucao.size() - 4; j++){
      if(i != j){
        if(i < j){
          delta = matrizAdj[s[i-1]][s[i+3]] + matrizAdj[s[i]][s[j+2]] + matrizAdj[s[i+2]][s[j+3]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i+2]][s[i+3]] - matrizAdj[s[j+2]][s[j+3]];
        }
        else { 
          delta = matrizAdj[s[i]][s[j-1]] + matrizAdj[s[i+2]][s[j]] + matrizAdj[s[i-1]][s[i+3]] - matrizAdj[s[j]][s[j-1]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i+2]][s[i+3]];
        }
        if(delta <= menor){
          menor = delta;
          pos_i = i;
          pos_j = j;
          
        } 
      } 
    }
  }
  if(pos_i > 0){
    solucao.erase(solucao.begin() + pos_i, solucao.begin() + pos_i + 3);
    solucao.insert(solucao.begin() + pos_j, &s[pos_i], &s[pos_i] + 3);
  }
}


inline double calculaDeltaOrOpt3(int i, int j, vector<int> &s){
  double delta;
  if(i < j){
    delta = matrizAdj[s[i-1]][s[i+3]] + matrizAdj[s[i]][s[j+2]] + matrizAdj[s[i+2]][s[j+3]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i+2]][s[i+3]] - matrizAdj[s[j+2]][s[j+3]];
  }
  else { 
    delta = matrizAdj[s[i]][s[j-1]] + matrizAdj[s[i+2]][s[j]] + matrizAdj[s[i-1]][s[i+3]] - matrizAdj[s[j]][s[j-1]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i+2]][s[i+3]];
  }

  return delta;
}

void rvnd(vector<int> &solucao){
  vector<int> s = solucao;
  vector<int> nLista = {0,1,2,3,4}; // lista de estruturas
  double custo = custoTotal(s); // custo inicial
  double custoMod; 
  int sel, pos;

  while(!nLista.empty()){ // roda enquanto existirem estruturas de vizinhança na lista

    int k = rand() % nLista.size();
    
    switch(nLista[k]){
      case 0:
        swap(solucao);
        break;

      case 1:
        reInsertion(solucao);
        break;
      
      case 2:
        twoOptN(solucao);
        break;

      case 3:
        orOpt2(solucao);
        break;

      case 4:
        orOpt3(solucao);
        break;
    }
    
    custoMod = custoTotal(solucao); // calcula o custo do Movimento

    if(custo > custoMod){ // movimento melhorou o custo
      custo = custoMod; 
      s = solucao;
    }
    else { // nao melhorou, exclui o movimento da lista
      solucao = s;
      nLista.erase(nLista.begin() + k);
    }

  }
}


vector<int> perturb(vector<int> &solucao){ 
  vector<int> s = solucao;

  int tam_max = ceil(((double)dimension)/10); // tamanho maximo das subsequencias                             
  int inicio1, fim1, inicio2, fim2;                 
  
  inicio1 = (rand() % ((dimension - (2 * tam_max)) - 1 + 1)) + 1; // posicao minima = 1, posicao maxima = final - 2*tmax
  fim1 = (rand() % ((inicio1 + (tam_max - 1)) - (inicio1 + 1) + 1)) + (inicio1 + 1); // minima = inicio+1, maxima = inicio1 + tmax - 1;
  inicio2 = (rand() % ((dimension - tam_max) - (fim1 + 1) + 1) + (fim1 + 1)); // minima = fim1 + 1, maxima = final - tmax;
  fim2 = (rand() % ((inicio2 + (tam_max - 1)) - (inicio2 + 1) + 1) + (inicio2 + 1)); // minima = inicio2 + 1, maxima = inicio2 + tmax - 1;


  int d1 = fim1 - inicio1; // tamanho da primeira subsequencia, usado para corrigir erros na inserção
  int d2 = fim2 - inicio2; // tamanho da segunda subsequencia, usado pra corrigir erros na inserção


  //cout << "inicio1: " << ini1 << "fim1: " << fim1 << "inicio2: " << ini2 << "fim2: " << fim2 << endl;

  s.erase(s.begin() + inicio2, s.begin() + fim2 + 1); // apaga primeira subsequencia
  s.erase(s.begin() + inicio1, s.begin() + fim1 + 1); // apaga segunda subsequencia
  s.insert(s.begin() + inicio1, &solucao[inicio2], &solucao[fim2] + 1);  // inclui a segunda subsequencia na posicao da primeira
  s.insert(s.begin() + inicio2 + (-1*(d1 - d2)) , &solucao[inicio1], &solucao[fim1] + 1); // inclui a segunda subsequencia na posicao da segunda

    
    
  return s;
  
}



vector<int> gils_rvnd(int i_max, int i_ils){
  vector<int> sl, s, sf; // s', s, s*
  double ff = std::numeric_limits<double>::infinity(); // custo final
  double alfa , fs1; 
  int iter_ils;
  for(int i = 1; i < i_max; i++){
    alfa = (double)rand() / RAND_MAX; // gera aleatorio entre 0 e 1
    s = construction(alfa); // constroi solucao inicial
    sl = s;
    iter_ils = 0;
    while(iter_ils < i_ils){
      rvnd(s); // explora as estruturas de vizinhança
      fs1 = custoTotal(sl);
      if(custoTotal(s) < fs1){
        sl = s;
        iter_ils = 0;
      }
      s = perturb(sl); // perturba a solução evitando resultado ótimo local
      iter_ils = iter_ils+1;
    }
    if(fs1 < ff){
      sf = sl;
      ff = fs1;
    }
  }
  return sf; // retorna a melhor solucao
}


inline double custoTotal(vector<int> &solucao){ // explora a matriz e retorna o custo de uma solucao
  double custo = 0;
  for(int i = 0, j = 1; i < solucao.size() - 1; i++, j++){
     custo = custo + matrizAdj[solucao[i]][solucao[j]];
  }
  return custo;
}

