#include "readData.h"
#include "CustoIn.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <cmath>

using namespace std;

double ** matrizAdj; // matriz de adjacencia
double ** matrizDist; // matriz reorganizada;
int dimension; // quantidade total de vertices
vector<int> cidades;
vector<int> candidatos;

void printData();
vector<int> construction(int tamanhoInicial);
void printSolution(vector<int> anyVector);
void candidates();
bool comp(const CustoIn& a, const CustoIn& b);
vector<CustoIn> calculaCusto(vector<int> listaCandidatos, vector<int> s);
vector<int> swap(vector<int> solucao);
double calculaDeltaSwap(int i, int j, vector<int> s);
vector<int> reInsertion(vector<int>solucao);
double custoTotal(vector<int>solucao);
double calculaDeltaReInsertion(int i, int j, vector<int> s);
vector<int> twoOptN(vector<int> solucao);
double calculaDeltaTwoOpt(int i, int j, vector<int> s);
vector<int> orOpt2(vector<int> solucao);
double calculaDeltaOrOpt2(int i, int j, vector<int> s);
vector<int> orOpt3(vector<int> solucao);
double calculaDeltaOrOpt3(int i, int j, vector<int> s);
vector<int> rvnd(vector<int> solucao);
vector<int> perturb(vector<int> solucao);
vector<int> gils_rvnd(int i_max, int i_ils);

int main(int argc, char** argv) {

    srand((unsigned)time(0));
    double valor;
    int i_max = 50;
    int i_ils;
    readData(argc, argv, &dimension, &matrizAdj);
    
    if(dimension >= 150){
      i_ils = dimension/2;
    }
    else{
      i_ils = dimension;
    }

    cidades = gils_rvnd(i_max, i_ils);
    printSolution(cidades);
    valor = custoTotal(cidades);
    cout << "custo: " << custoTotal(cidades) << endl;

    //printData();
    //vector<int> teste = construction(7);
    //printSolution(teste);
    //cout << "custo: " << custoTotal(teste) << endl;
    //cidades = swap(teste);
    //valor = custoTotal(teste);
    //cidades = reInsertion(teste);
    //vector<int> tora = {1,2,3,4,5,6,7,8,9,10,1};
   // printSolution(cidades);
    //cidades = perturb(teste);
    //printSolution(cidades);
    //candidatos = rvnd(teste);
    //printSolution(candidatos);
    //cout << "custo: " << custoTotal(candidatos) << endl;
    
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

vector<int> construction(int tamanhoInicial){ 
  vector<int> s = {1,1};// lista de cidades da solução inicial
  vector<int> listaCandidatos;
  for(int i = 2; i <= dimension; i++){
    listaCandidatos.push_back(i);
  }

  for(int j = 0; j < tamanhoInicial; j++){
    int k = rand() % listaCandidatos.size();
    s.insert(s.begin() + 1, listaCandidatos[k]); // adiciona cidade aleatoria a solução
    listaCandidatos.erase(listaCandidatos.begin() + k); // apaga da lista de candidatos oq ja foi pra solução
  }


 vector<CustoIn> custoInsertion = calculaCusto(listaCandidatos, s);

  
  std::sort(custoInsertion.begin(), custoInsertion.end(), comp);

  int sel;
  while(!listaCandidatos.empty()){
    if(custoInsertion.size() > 3){
    sel = rand() % 2; }
    else {
      sel = rand() % custoInsertion.size();
      cout << sel << endl;
    }
    
    s.insert(s.begin() + custoInsertion[sel].arestaOut + 1, custoInsertion[sel].noIn); 
    
    for(int i = 0; i < listaCandidatos.size(); i++){
      if(listaCandidatos[i]==custoInsertion[sel].noIn)
        listaCandidatos.erase(listaCandidatos.begin() + i);
    }
    
    custoInsertion.erase(custoInsertion.begin(), custoInsertion.begin() + custoInsertion.size());
    custoInsertion = calculaCusto(listaCandidatos, s);
    std::sort(custoInsertion.begin(), custoInsertion.end(), comp);

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

vector<CustoIn> calculaCusto(vector<int> listaCandidatos, vector<int> s){
  vector<CustoIn> custoInsertion ((s.size() - 1) * listaCandidatos.size());
  for(int i = 0, j = 1, l = 0; i < s.size()-1; i++, j++){
    for(auto k : listaCandidatos){
      custoInsertion[l].custo = matrizAdj[s[i]][k] +  matrizAdj[s[j]][k] - matrizAdj[s[i]][s[j]]; 
      custoInsertion[l].noIn = k;
      custoInsertion[l].arestaOut = i;
      l++;
    }
  }
  return custoInsertion;


}

vector<int> swap(vector<int> solucao){
  vector<int> solCopy = solucao;
  vector<int> melhor;
  double d, d2;
  double menor;
  for(int i = 1; i < solucao.size() - 1; i++){ // excluir da operação a primeira e a ultima posição do vetor
    for(int j = 1; j < solucao.size() - 1; j++){ 
      if(i <= j){ // nao repetir swap
        int aux = solucao[i];
        solCopy[i] = solucao[j];
        solCopy[j] = aux;
        d2 = calculaDeltaSwap(i,j, solucao);
        //d = custoTotal(solCopy) - fs;
        //cout <<"i:"<< i << "  j:"<< j << "  delta = " << d << endl;
        if(d <= menor || (i == 1 && j == 1)){
          menor = d;
          melhor = solCopy;
        }
        solCopy = solucao;
      }
    }
  }
  return melhor;

}

double calculaDeltaSwap(int i, int j, vector<int> s){
  double delta;
  if(i == 1 && j < 4){
    delta = matrizAdj[s[i-1]][s[j]] + matrizAdj[s[i]][s[j+1]] - matrizAdj[s[i-1]][s[i]] - matrizAdj[s[j]][s[j+1]];
  }
  else{
    if(i + 1 == j){
      delta = matrizAdj[s[i-1]][s[j]] + matrizAdj[s[i]][s[j+1]] - matrizAdj[s[i-1]][s[j-1]] - matrizAdj[s[j]][s[j+1]];
    }
    else{
    delta = matrizAdj[s[i]][s[j-1]] + matrizAdj[s[i]][s[j+1]] + matrizAdj[s[j]][s[i-1]] + matrizAdj[s[j]][s[i+1]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i]][s[i+1]] - matrizAdj[s[j]][s[j-1]] - matrizAdj[s[j]][s[j+1]];
        }
  }
  //cout << "delta swap: " << delta <<endl; 
  return delta; 
}

vector<int> reInsertion(vector<int>solucao){
  vector<int> solCopy = solucao;
  double menor = 0;
  vector<int> melhor = solucao;
  double d, d2; 
  for(int i = 1; i < solucao.size() - 1; i++){
    for(int j = 1; j < solucao.size() - 1; j++){
      if(i != j){
      solCopy.erase(solCopy.begin() + i);
      solCopy.insert(solCopy.begin() + j, solucao[i]);
       //d = custoTotal(solCopy) - fs;
       d2 = calculaDeltaReInsertion(i,j,solucao);
       //cout <<"i:"<< i << "  j:"<< j << "  delta = " << d2 << endl;
      if(d2 <= menor || (i == 1 && j == 1)){
        menor = d2;
        melhor = solCopy;
      }
      solCopy = solucao;
      }
    }
     
  }
  return melhor;
}

double calculaDeltaReInsertion(int i, int j, vector<int> s){
  double delta;
  if(i <= j){
    delta = matrizAdj[s[i]][s[j+1]] + matrizAdj[s[i]][s[j]] + matrizAdj[s[i-1]][s[i+1]] - matrizAdj[s[j]][s[j+1]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i]][s[i+1]];
            }
  else{
  delta = matrizAdj[s[i]][s[j-1]] + matrizAdj[s[i]][s[j]] + matrizAdj[s[i-1]][s[i+1]] - matrizAdj[s[j]][s[j-1]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i]][s[i+1]];
      }
  //cout << "Delta Re: "<< delta << endl;
  return delta;

}

vector<int> twoOptN(vector<int> solucao){
  vector<int> solCopy = solucao;
  vector<int> solInverted = solucao;
  double delta , d, menor = 0;
    for(int i = 1; i < solucao.size() - 2; i++){
      for(int j = i + 1; j < solucao.size() - 1; j++){
        solCopy = solucao;
        for(int k = i; k <= j; k++){
          solCopy[k] = solucao[j + i - k];
        }
        //d = custoTotal(solCopy) - fs;
        //cout << "i: "<<i<<"j: "<<j <<"delta = "<<d<<endl;
        delta = calculaDeltaTwoOpt(i, j, solucao);
        //cout << "i: "<<i<<"j: "<<j <<"d = "<<delta<<endl;
        if(delta <= menor){
          menor = delta;
          solInverted = solCopy;
        }
      }
    }
      
    return solInverted;
}

double calculaDeltaTwoOpt(int i, int j, vector<int> s){
  double delta;
  if(j == (i + 1)){
    delta = matrizAdj[s[i-1]][s[j]] + matrizAdj[s[i]][s[j+1]] - matrizAdj[s[i-1]][s[j-1]] - matrizAdj[s[j]][s[j+1]];
  }
  else{
  delta = matrizAdj[s[i-1]][s[j]] + matrizAdj[s[i]][s[j+1]] - matrizAdj[s[i-1]][s[i]] - matrizAdj[s[j]][s[j+1]];
  }
  return delta;
}

vector<int> orOpt2(vector<int> solucao){
  vector<int> solCopy = solucao;
  vector<int> melhor = solucao;
  double menor = 0;
  double d, delta;
  for(int i = 1; i < solucao.size() - 2; i++){
    for(int j = 1; j < solucao.size() - 3; j++){
      if(i != j){
        solCopy = solucao;
        solCopy.erase(solCopy.begin() + i, solCopy.begin() + i + 2);
        solCopy.insert(solCopy.begin() + j, &solucao[i], &solucao[i] + 2);
        
        delta = calculaDeltaOrOpt2(i,j,solucao);
        if(delta <= menor){
          menor = delta;
          melhor = solCopy;
        }
        
      }
      
    }
     
  }
  
  return melhor;
}

double calculaDeltaOrOpt2(int i, int j, vector<int> s){
  double delta;
  if(i < j){
    delta = matrizAdj[s[i-1]][s[i+2]] + matrizAdj[s[i]][s[j+1]] + matrizAdj[s[i+1]][s[j+2]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i+1]][s[i+2]] - matrizAdj[s[j+1]][s[j+2]];
  }
  else { 
    delta = matrizAdj[s[i]][s[j-1]] + matrizAdj[s[i+1]][s[j]] + matrizAdj[s[i+2]][s[i-1]] - matrizAdj[s[j]][s[j-1]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i+1]][s[i+2]];
  }

  return delta;
}

vector<int> orOpt3(vector<int> solucao){
  vector<int> solCopy = solucao;
  vector<int> melhor = solucao;
  double menor = 0;
  double d, delta;

  for(int i = 1; i < solucao.size() - 3; i++){
    for(int j = 1; j < solucao.size() - 4; j++){
      if(i != j){
        solCopy = solucao;
        solCopy.erase(solCopy.begin() + i, solCopy.begin() + i + 3);
        solCopy.insert(solCopy.begin() + j, &solucao[i], &solucao[i] + 3);
        delta = calculaDeltaOrOpt3(i, j, solucao);
        if(delta <= menor){
          menor = delta;
          melhor = solCopy;
        }
      } 
    }
  }
 // cout << "menor: " << menor << endl;
  return melhor;
}


double calculaDeltaOrOpt3(int i, int j, vector<int> s){
  double delta;
  if(i < j){
    delta = matrizAdj[s[i-1]][s[i+3]] + matrizAdj[s[i]][s[j+2]] + matrizAdj[s[i+2]][s[j+3]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i+2]][s[i+3]] - matrizAdj[s[j+2]][s[j+3]];
  }
  else { 
    delta = matrizAdj[s[i]][s[j-1]] + matrizAdj[s[i+2]][s[j]] + matrizAdj[s[i-1]][s[i+3]] - matrizAdj[s[j]][s[j-1]] - matrizAdj[s[i]][s[i-1]] - matrizAdj[s[i+2]][s[i+3]];
  }

  return delta;
}


vector<int> rvnd(vector<int> solucao){
  vector<int> s = solucao;
  vector<int> nLista = {0,1,2,3,4};
  double custo = custoTotal(solucao);
  int sel, pos;

  while(!nLista.empty()){
    int k = rand() % 5;
    //cout << "k = " << k << endl;

    for(int i = 0; i < nLista.size(); i++){
      if(nLista[i] == k){
        sel = k;
        pos = i; //erro: pos 1 quando só tem um elemento
        break;
      }
      else
      {
        sel = -1;
      }
      
    }
    //printSolution(nLista);
    //cout << "sel = " << sel << endl;
    //cout << "pos = " << pos << endl;

    if(sel != -1){
     if(sel == 0){
       //cout << "entrou swap" << endl;
       if(custo > custoTotal(swap(s))){
        //cout << "swap" << endl;
         s = swap(s);
         custo = custoTotal(s);
       }
       else
         nLista.erase(nLista.begin() + pos);
       
     }

     if(sel == 1){
       //cout << "entrou 2optn" <<endl;
       if(custo > custoTotal(twoOptN(s))){
        // cout << "2optn" <<endl;
         s = twoOptN(s);
         custo = custoTotal(s);
       }
       else
         nLista.erase(nLista.begin() + pos);
     }

     if(sel == 2){
       //cout << "entrou reinsertion" <<endl;
       if(custo > custoTotal(reInsertion(s))){
         //cout << "reinsertion" <<endl;
         s = reInsertion(s);
         custo = custoTotal(s);
       }
       else
         nLista.erase(nLista.begin() + pos);
     }

     if(sel == 3){
       //cout << "entrou oropt2" <<endl;
       if(custo > custoTotal(orOpt2(s))){
        // cout << "oropt2" <<endl;
         s = orOpt2(s);
         custo = custoTotal(s);
       }
       else
         nLista.erase(nLista.begin() + pos);
     }

     if(sel == 4){
       //cout << "entrou oropt3" <<endl;
       //printSolution(s);
       if(custo > custoTotal(orOpt3(s))){
         //cout << "oropt3" <<endl;
         s = orOpt3(s);
         custo = custoTotal(s);
       }
       else
         nLista.erase(nLista.begin() + pos);
     }
    }
  
  }
  return s;
}


vector<int> perturb(vector<int> solucao){ // swap de subsequencias
  vector<int> s = solucao;
  int inicio, fim, p, tam;

  do{
    tam = ceil(((double)dimension)/10);
    inicio = 1 + rand() % (solucao.size() / 2);
    fim  =  inicio + rand() % tam; // tamanho maximo n/10;
    p = rand() % solucao.size() - 2;
    }while(p + fim - inicio > solucao.size() - 2 || p <= fim || inicio == fim);
    //cout << "inicio: " << inicio << " fim: " << fim << " p: "<< p << endl;

    for(int i = inicio; i <= fim; i++){
      s[i] = solucao[p+i-inicio];
    }
    for(int j = p; j <= p + (fim - inicio); j++){
        s[j] = solucao[j-p+inicio];
    }  
  
  
  
  return s;
  
}


vector<int> gils_rvnd(int i_max, int i_ils){
  vector<int> s, s1, melhor;
  double fs = 0;
  for(int i = 1; i < i_max; i++){
    double alfa = (rand() % 11)*0.1;
    s = construction(alfa);
    s1 = s;
    int iter_ils = 0;

    while(iter_ils < i_ils){
      s = rvnd(s);
      if(custoTotal(s) < custoTotal(s1)){
        s1 = s;
        iter_ils = 0;
      }
      s = perturb(s1);
      iter_ils++;
    }
    if(custoTotal(s1) < fs || fs == 0){
      melhor = s1;
      fs = custoTotal(s1);
    }
  }
  return melhor;
}


double custoTotal(vector<int>solucao){
  double custo = 0;
  for(int i = 0, j = 1; i < solucao.size() - 1; i++, j++){
     custo = custo + matrizAdj[solucao[i]][solucao[j]];
  }
  return custo;
}

