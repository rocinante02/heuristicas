#include "readData.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<int> OptimalRoute; // vetor de solucão
int dimension; // quantidade total de vertices
double ** matrizAdj; // matriz de adjacencia
double ** matrizDist; // matriz reorganizada;
int contador = 1;
vector<int> optimal;

void printData();
void printBestSolution();
void verificaMatriz(vector< vector<int> > inception);
bool comp(const int &a, const int &b);

int main(int argc, char** argv) {

    readData(argc, argv, &dimension, &matrizAdj);
    readSolution(argc, argv, &dimension, OptimalRoute);

    //printData();
    printBestSolution();

    vector< vector<int> > inception;

    for(int i = 0; i < dimension; i++){
      
      optimal.push_back(OptimalRoute[i]);
    }
    
   
    for(int i = 0; i < dimension; i++){
      inception.push_back(optimal); // deu ruim
      sort(inception[i].begin(), inception[i].end(),comp);
      contador++;
    }

  

    for (int i = 0; i < dimension; i++) {
      for (int j = 0; j < dimension; j++) {
        cout << inception[i][j] << " ";
      } 
      cout << endl;
    }
    verificaMatriz(inception);
}

void printData() {
  cout << "Dimension: " << dimension << endl;
  for (size_t i = 1; i <= dimension; i++) {
    for (size_t j = 1; j <= dimension; j++) {
      cout << matrizAdj[i][j] << " ";
    }
    cout << endl;
  }
}


void printBestSolution() {
  cout << "\nOptimal Route:" << endl;
  for (int i = 0; i < dimension + 1; i++) {
      cout << OptimalRoute[i] << " ";
    }
  cout << endl;
  cout << endl;
  }

 bool comp( const int &a, const int &b) // comparação dos custos utilizada para ordenar os objetos
  {
    return matrizAdj[contador][a] < matrizAdj[contador][b];
  }

void verificaMatriz(vector< vector<int> > inception){
  for(int i = 1; i <= dimension; i++){
    cout << "Cliente: " << i << " Mais Proximo Disponivel: " << inception[i-1][1] << " Selecionado:"  << OptimalRoute[i] <<endl;   
  }
}
