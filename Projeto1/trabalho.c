/*
Data used: W. W. Zachary, An
information flow model for conflict and fission in small groups, Journal of
Anthropological Research 33, 452-473 (1977).

    Universidade de Brasilia
    Departamento de Ciência da Computação
    Projeto 1
    Teoria e Aplicação de Grafos, Turma A, 1/2019
    Prof. Díbio
    Autor: Lucas Dalle Rocha, 17/0016641
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MaxNumVertices  100
#define FALSE           0
#define TRUE            1

/*-- Funções de lista utilizadas para cálculo do coeficiente de aglomeração --*/
typedef struct elemento{
    int dado;
    struct elemento *proximo;
}t_elemento;

typedef struct lista{
    t_elemento *inicio;
    t_elemento *fim;
}t_lista;

t_lista *criaLista(){
    t_lista *l = (t_lista*)malloc(sizeof(t_lista));
    l->inicio = NULL;
    l->fim = NULL;
    return l;
}

void insereFinal(int valor, t_lista *l){
   t_elemento *novoultimo = (t_elemento*)malloc(sizeof(t_elemento));
   novoultimo->dado = valor;
   novoultimo->proximo = NULL;
   if(l->inicio == NULL){
      l->inicio = novoultimo;
   }else{
      l->fim->proximo = novoultimo;
   }
   l->fim = novoultimo;
}

int removerInicio(t_lista *l){
    if(l->inicio == NULL){
        return -1;
    }
    int tmp = l->inicio->dado;
    t_elemento *removido = l->inicio;
    l->inicio = l->inicio->proximo;
    free(removido);
    if(l->inicio == NULL){
       l->fim = NULL;
    }
    return tmp;
}

int estaVazia(t_lista *l){
    if(l->inicio == NULL){
        return 1;
    }
    return 0;
}

void imprimirLista(t_lista *l){ //para fins de teste
    t_elemento *atual = l->inicio;
    while(atual != NULL){
          printf("%d ",atual->dado+1);
          atual = atual->proximo;
    }
    printf("\n");
}
/*-- Fim --*/

/*-- Declarações da estrutura do grafo --*/
typedef int TipoValorVertice;

typedef struct TipoItem{
    TipoValorVertice Vertice;
}TipoItem;

typedef struct Struct_Celula *Apontador;
struct Struct_Celula{
    TipoItem Item;
    Apontador Prox;
};
typedef struct Struct_Celula Celula;
typedef struct TipoLista{
    Apontador Primeiro, Ultimo;
}TipoLista;

typedef struct TipoGrafo{
    TipoLista Adj[MaxNumVertices+1];
    TipoValorVertice NumVertices;
    short NumArestas;
}TipoGrafo;

Apontador Aux;
long i;
TipoValorVertice V1, V2, Adj;
TipoGrafo Grafo, GrafoT;
TipoValorVertice NVertices;
int NArestas;
short FimListaAdj;
/*-- Fim --*/

/*-- Operadores da lista utilizada no grafo --*/
void FLVazia(TipoLista *Lista){
    Lista->Primeiro = (Apontador)malloc(sizeof(Celula));
    Lista->Ultimo = Lista->Primeiro;
    Lista->Primeiro->Prox = NULL;
}

short Vazia(TipoLista Lista){
    return (Lista.Primeiro == Lista.Ultimo);
}

void Insere(TipoItem *x, TipoLista *Lista){
    /*-- Insere depois do último item da lista --*/
    Lista->Ultimo->Prox = (Apontador)malloc(sizeof(Celula));
    Lista->Ultimo = Lista->Ultimo->Prox;
    Lista->Ultimo->Item = *x;
    Lista->Ultimo->Prox = NULL;
}
/*-- Fim operadores --*/

void FGVazio(TipoGrafo *Grafo){
    long i;
    for(i = 0; i < Grafo->NumVertices; i++){
        FLVazia(&Grafo->Adj[i]);
    }
}

void InsereAresta(TipoValorVertice *V1, TipoValorVertice *V2, TipoGrafo *Grafo){
    TipoItem x;
    x.Vertice = *V2;
    Insere(&x, &Grafo->Adj[*V1]);
}

short ExisteAresta(TipoValorVertice Vertice1, TipoValorVertice Vertice2, TipoGrafo *Grafo){
    Apontador Aux;
    short EncontrouAresta = FALSE;
    Aux = Grafo->Adj[Vertice1].Primeiro->Prox;
    while(Aux != NULL && EncontrouAresta == FALSE){
        if(Vertice2 == Aux->Item.Vertice){
        EncontrouAresta = TRUE;
        }
        Aux = Aux->Prox;
    }
    return EncontrouAresta;
}

/*-- Operadores para obter a lista de adjacentes --*/
short ListaAdjVazia(TipoValorVertice *Vertice, TipoGrafo *Grafo){
    return (Grafo->Adj[*Vertice].Primeiro == Grafo->Adj[*Vertice].Ultimo);
}

Apontador PrimeiroListaAdj(TipoValorVertice *Vertice, TipoGrafo *Grafo){
    return (Grafo->Adj[*Vertice].Primeiro->Prox);
}

void ProxAdj(TipoValorVertice *Vertice, TipoGrafo *Grafo, TipoValorVertice *Adj, Apontador *Prox, short *FimListaAdj){
    /*-- Retorna Adj do Item apontado por Prox --*/
    *Adj = (*Prox)->Item.Vertice;
    *Prox = (*Prox)->Prox;
    if (*Prox == NULL){
        *FimListaAdj = TRUE;
    }
}

void LiberaGrafo(TipoGrafo *Grafo){
    Apontador AuxAnterior, Aux;
    for(i = 0; i < Grafo->NumVertices; i++){
        Aux = Grafo->Adj[i].Primeiro->Prox;
        free(Grafo->Adj[i].Primeiro);   /*Libera celula cabeca*/
        Grafo->Adj[i].Primeiro=NULL;
        while(Aux != NULL){
            AuxAnterior = Aux;
            Aux = Aux->Prox;
            free(AuxAnterior);
        }
    }
    Grafo->NumVertices = 0;
}

void ImprimeGrafo(TipoGrafo *Grafo){
    int i;
    Apontador Aux;
    for(i = 0; i < Grafo->NumVertices; i++){
        printf("Vertice%2d:", i+1);
        if(!Vazia(Grafo->Adj[i])){
            Aux = Grafo->Adj[i].Primeiro->Prox;
            while(Aux != NULL){
                printf("%3d ", Aux->Item.Vertice+1);
                Aux = Aux->Prox;
            }
        }
        printf("\n");
    }
}

void ImprimeLista(TipoLista Lista){
    Apontador Aux;
    Aux = Lista.Primeiro->Prox;
    while(Aux != NULL){
        printf("%3d ", Aux->Item.Vertice);
        Aux = Aux->Prox;
    }
}

int VerticesArq(char *str){
    FILE *arq;
    int vertCTD = 0;
    arq = fopen("karate.gml", "r");
    if(arq == NULL){
        //Finaliza o programa caso não seja possível ler o arquivo.
        return -1;
    }
    //Lê o arquivo linha por linha, armazenando a linha atual na string str
    while(!feof(arq)){
        fgets(str, 100, arq);
        for(int i=0; i<strlen(str); i++){
            //Calcula quantos vértices há, uma vez que id, no arquivo, precede um vértice
            if(str[i] == 'i' && str[i+1] == 'd'){
                vertCTD++;
                //Armazena o número de vértices em TEMP
            }
        }
    }
    fclose(arq);
    free(arq);
    return vertCTD;
}

void colocaArestas(char *str){
    //abrimos o arquivo novamente para procurar arestas
    FILE *arq;
    char TEMPS[2] = {'\0'}, TEMPT[2] = {'\0'};
    int TEMP = -1, TEMP1 = -1; //seta as duas variáveis temporárias como -1, para utilizarmos na parte de arestas
    arq = fopen("karate.gml", "r");
    while(!feof(arq)){
        fgets(str, 100, arq);
        for(int i=0; i<strlen(str); i++){
            if(str[i] == 's' && str[i+1] == 'o' && str[i+2] == 'u' && str[i+3] == 'r' && str[i+4] == 'c' && str[i+5] == 'e'){
                TEMPS[0] = str[i+7]; //se encontrarmos source no arquivo, dois espaços depois teremos o primeiro dígito do número do vértice
                if(str[i+8] != '\0'){
                    TEMPS[1] = str[i+8]; //se o próximo dígito for um número, este será o segundo dígito do vértice
                }
                /*transformamos a string em um número inteiro e armazenamos em TEMP, reduzido de 1,
                para armazenar no grafo (que começa em 0 e vai até NVertices-1)
                */
                TEMP = atoi(TEMPS) - 1;
                TEMPS[0] = TEMPS[1] = '\0'; //esvaziamos a string
            }else if(str[i] == 't' && str[i+1] == 'a' && str[i+2] == 'r' && str[i+3] == 'g' && str[i+4] == 'e' && str[i+5] == 't'){
                TEMPT[0] = str[i+7];
                if(str[i+8] != '\0'){
                    TEMPT[1] = str[i+8];  //target é análogo a source, uma vez que se trata de um grafo não direcionado
                }
                TEMP1 = atoi(TEMPT) - 1; //armazenamos o inteiro em TEMP1
                TEMPT[0] = TEMPT[1] = '\0';
            }
            if(TEMP != -1 && TEMP1 != -1){
                //uma vez que o valor de TEMP e TEMP1 mudaram, temos uma aresta entre esses dois vértices
                V1 = TEMP;
                V2 = TEMP1;
                Grafo.NumArestas++; //número de arestas incrementado
                //arestas inseridas de V1 a V2 e V2 a V1 (não direcionado)
                InsereAresta(&V1, &V2, &Grafo);
                InsereAresta(&V2, &V1, &Grafo);
                TEMP = TEMP1 = -1; //seta a flag -1 para as variáveis novamente
            }
        }
    }
    fclose(arq);
    free(arq);
}

int grauVertice(int vert){
    int grauCTD = 0;
    V1 = vert; //V1 recebe j a cada incremento para calcular todos os vértices
    if(!ListaAdjVazia(&V1, &Grafo)){ //calcula apenas se a lista de adjacentes não está vazia
        Aux = PrimeiroListaAdj(&V1, &Grafo); //define começo da lista e percorre os adjacentes
        FimListaAdj = FALSE;
        while(!FimListaAdj){
            ProxAdj(&V1, &Grafo, &Adj, &Aux, &FimListaAdj);
            grauCTD++; //incrementa o valor do grau do vértice j+1
        }
    }
    return grauCTD;
}

float coefAglomeracao(int vert, int grau){
    float numerador = 0, denominador, resultado;
    int grauVert = grau;
    t_lista * l = criaLista(); //lista utilizada para analisar cada vértice
    V1 = vert;
    if(!ListaAdjVazia(&V1, &Grafo)){
        Aux = PrimeiroListaAdj(&V1, &Grafo);
        FimListaAdj = FALSE;
        while(!FimListaAdj){
            ProxAdj(&V1, &Grafo, &Adj, &Aux, &FimListaAdj);
            insereFinal(Adj, l); //insere na lista, em ordem, as arestas do vértice analisado
        }
    }
    //criamos dois ponteiros, para comparar na lista todos os elementos entre si
    t_elemento * agora = l->inicio;
    t_elemento * anda = l->inicio->proximo;
    while(agora->proximo != NULL){ //encerra quando o prox de 'agora' for igual a NULL, para o 'anda' não progredir após o NULL
        if(ExisteAresta(agora->dado, anda->dado, &Grafo)){
            //mostra triângulos formados:
            //printf("\nTRIÂNGULO EM %d %d %d", V1+1, agora->dado+1, anda->dado+1);

            //se houver aresta de modo que forme um triângulo entre V1 e os dados de 'agora' e 'anda', temos um incremento na variável numerador
            numerador++;
        }
        if(anda->proximo!=NULL){
            anda = anda->proximo; //avança o 'anda' e mantém o 'agora'
        }else{
            agora = agora->proximo; //avança o 'agora' e volta o 'anda' para comparar após o 'agora'
            anda = agora->proximo;
        }
    }
    while(estaVazia(l) != 1){
        removerInicio(l); //ao terminar, esvazia a lista
    }
    //variável denominador para cálculo do coeficiente
    denominador = grauVert * (grauVert-1);
    if(denominador > 0){ //não pode existir divisão por zero
        resultado = (2*numerador)/denominador; //fórmula do coeficiente, armazenada em float
    }else{
        resultado = 0;
    }
    return resultado;
}

/*-- Não consegui implementar o algoritmo de Bron Kerbosh corretamente
(está gerando apenas um clique maximal e encerrando o programa subitamente) --*/
void bronKerbosh(t_lista * R, t_lista * P, t_lista * X){
    t_lista * Vizinhos = criaLista();
    t_lista * Rnew = criaLista();
    t_lista * Pnew = criaLista();
    t_lista * Xnew = criaLista();
    t_lista * aux = criaLista();
    int flag1 = 0;
    int flag2 = 0;
    if(estaVazia(P) == TRUE && estaVazia(X) == TRUE){
        printf("\nClique encontrado:\n");
        imprimirLista(R);
    }
    t_elemento *v = P->inicio;
    while(estaVazia(P) == FALSE && v != P->fim){
        V1 = v->dado;
        //união de R e v
        t_elemento *atual = R->inicio;
        while(atual != NULL){
            if(V1 == atual->dado){
                flag1 = 1;
                break;
            }
            atual = atual->proximo;
        }
        atual = R->inicio;
        while(atual != NULL){
            insereFinal(atual->dado, Rnew);
            atual = atual->proximo;
        }
        if(flag1 != 1){
        insereFinal(V1, Rnew);
        }
        //interseção de P e vizinhos de v
        atual = P->inicio;
        t_elemento *atual2 = Vizinhos->inicio;
        if(!ListaAdjVazia(&V1, &Grafo)){
            Aux = PrimeiroListaAdj(&V1, &Grafo);
            FimListaAdj = FALSE;
            while(!FimListaAdj){
                ProxAdj(&V1, &Grafo, &Adj, &Aux, &FimListaAdj);
                insereFinal(Adj, Vizinhos);
            }
        }
        while(atual != NULL){
            while(atual2 != NULL){
                if(atual->dado == atual2->dado){
                    insereFinal(atual->dado, Pnew);
                }
                atual2 = atual2->proximo;
            }
            atual = atual->proximo;
	    atual2 = Vizinhos->inicio;

        }
        //interseção de X e vizinhos de v
        atual = X->inicio;
        atual2 = Vizinhos->inicio;
        while(atual != NULL){
            while(atual2 != NULL){
                if(atual->dado == atual2->dado){
                    insereFinal(atual->dado, Xnew);
                }
                atual2 = atual2->proximo;
            }
            atual = atual->proximo;
	    atual2 = Vizinhos->inicio;
        }
        //bronkerbosch
        bronKerbosh(Rnew, Pnew, Xnew);
        //P = o que tem em P e nao tem em v
        atual = P->inicio;
        while(atual != NULL){
            if(atual->dado != V1){
                insereFinal(atual->dado, aux);
            }
            atual = atual->proximo;
        }
        atual = P->inicio;
        while(atual != NULL){
            removerInicio(P);
            atual = atual->proximo;
        }
        atual = aux->inicio;
        while(atual != NULL){
            insereFinal(atual->dado, P);
            removerInicio(aux);
            atual = atual->proximo;
        }
        //X = união de X com v
        atual = X->inicio;
        while(atual != NULL){
            if(V1 == atual->dado){
                flag2 = 1;
                break;
            }
            atual = atual->proximo;
        }
        atual = X->inicio;
        while(atual != NULL){
            insereFinal(atual->dado, aux);
            atual = atual->proximo;
        }
        if(flag2 != 1){
            insereFinal(V1, aux);
        }
        atual = X->inicio;
        while(atual != NULL){
            removerInicio(X);
            atual = atual->proximo;
        }
        atual = aux->inicio;
        while(atual != NULL){
            insereFinal(atual->dado, X);
            removerInicio(aux);
        }
        //se p não está vazio, v = p->inicio
        if(estaVazia(P) == FALSE){
            v = P->inicio;
        }
    }
}


/* ============================================================= */
int main(int argc, char *argv[]){

    int TEMP = 0;
    char str[100];
    float coef_Aglo_Medio = 0;

    /*-- Arquivo GML --*/
    TEMP = VerticesArq(str);
    if(TEMP != -1){
    NVertices = TEMP;
    }else{
        printf("Erro ao abrir o arquivo!");
        return 1;
    }
    Grafo.NumVertices = NVertices; //define número de vértices no grafo
    Grafo.NumArestas = 0; //inicia o grafo com zero arestas (será incrementado a cada chamada InsereAresta)
    FGVazio(&Grafo); //cria grafo vazio
    colocaArestas(str); //acessa arquivo novamente para ler e colocar as arestas no grafo

    //Caso queira visualizar o grafo:
    //printf("GRAFO:\n\n");
    //ImprimeGrafo(&Grafo);

    /*-- Calcula grau de cada vértice --*/
    int grau[NVertices]; //cria variável para armazenar, futuramente, o grau de cada vértice
    printf("GRAU DE CADA VÉRTICE:");
    for(int j=0; j<NVertices; j++){
        grau[j] = grauVertice(j);
        printf("\nGrau de %d: %d", j+1, grau[j]);
    }

    /*-- Cliques Maximais -- com ERROS!
    t_lista * R = criaLista();
    t_lista * P = criaLista();
    t_lista * X = criaLista();

    for(int j=0; j<NVertices; j++){
        insereFinal(j, P);
    }

    bronKerbosh(R, P, X);
    */
	
    /*-- Calcula coeficiente de aglomeração de cada vértice --*/
    float coef_Aglo[NVertices]; //cria variável para armazenar o coef. de aglomeração de cada vértice
    printf("\nCOEFICIENTE DE AGLOMERAÇÃO DE CADA VÉRTICE:");
    for(int j=0; j<NVertices; j++){
        coef_Aglo[j] = coefAglomeracao(j, grau[j]);
        printf("\nCoeficiente de aglomeração do vértice %d: %.6f", j+1, coef_Aglo[j]); //j+1 pois o vértice está armazenado à -1 no grafo
    }

    /*-- Calcula coeficiente de aglomeração médio do grafo --*/
    printf("\nCOEFICIENTE MÉDIO DE AGLOMERAÇÃO DO GRAFO: ");
    for(int j=0; j<NVertices; j++){
        coef_Aglo_Medio = coef_Aglo_Medio + coef_Aglo[j]; //soma todos os coeficientes
    }
    coef_Aglo_Medio = coef_Aglo_Medio/NVertices; //e divide pelo número de vértices, para obter o coeficiente médio do grafo
    printf("%.6f\n", coef_Aglo_Medio);

  LiberaGrafo(&Grafo);   /* Imprime sujeira normalmente */
  ImprimeGrafo(&Grafo);
  return 0;
}
