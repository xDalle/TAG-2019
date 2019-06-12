/*
    Universidade de Brasilia
    Departamento de Ciência da Computação
    Projeto 2
    Teoria e Aplicação de Grafos, Turma A, 1/2019
    Prof. Díbio
    Autor: Lucas Dalle Rocha, 17/0016641
    O programa cria um grafo baseado na grade curricular do curso de Ciência da Computação
    da Universidade de Brasília e extrai, do mesmo, uma das possíveis ordenações topológicas,
    utilizando o algoritmo de Kahn e, por fim, descobre o caminho crítico e seu peso total.
    Obs: vértices são disciplinas do curso e arestas são indicadas como pré-requisito para
    a disciplina seguinte (vértices adjacentes), sendo seu peso o número de créditos 
    proporcionado pela disciplina.
    Ademais, está esquematizado graficamente, pelo software graphviz, e ao executar o comando
    "dot -Tpng grafo.dot -o grafo.png" é gerada uma imagem para melhor visualização do DAG,
    e em destaque arestas da cor verde que indicam o caminho crítico.
    
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MaxNumVertices  100
#define FALSE           0
#define TRUE            1
#define NMaterias 32

/* Função de máximo entre dois números retirada de https://gcc.gnu.org/onlinedocs/gcc-4.9.2/gcc/Typeof.html */
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

/*-- Funções de lista utilizadas no programa --*/
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

int removerFinal(t_lista * l){
    if(l->inicio == NULL){
       return -1;
    }
    int tmp = l->fim->dado;
    t_elemento * ultimo = l->inicio;
    t_elemento * penultimo = NULL;
    while(ultimo->proximo != NULL){
         penultimo = ultimo;
         ultimo = ultimo->proximo;
    }
    if(penultimo != NULL){
       penultimo->proximo = NULL;
       l->fim = penultimo;

    }else{//lista possui apenas um elemento
        l->inicio = NULL;
        l->fim = NULL;
    }
    free(ultimo);
    return tmp;
}

int estaVazia(t_lista *l){
    if(l->inicio == NULL){
        return 1;
    }
    return 0;
}

void imprimirLista(t_lista *l, int *codigo){ //para fins de teste
    t_elemento *atual = l->inicio;
    int ctdn = 0;
    while(atual != NULL){
          if(atual->proximo != NULL){
          printf("%d, ",codigo[atual->dado]);
          }else{
          printf("%d.", codigo[atual->dado]);
          }
          atual = atual->proximo;
          ctdn++;
          if(ctdn % 8 == 0){
            printf("\n");
          }
    }
}

int estaNaLista(int valor, t_lista *l){
    t_elemento *atual = l->inicio;
    while(atual!=NULL){
        if(atual->dado == valor){
            return TRUE;
        }
        atual = atual->proximo;
    }
    return FALSE;
}
/*-- Fim --*/

/*-- Declarações da estrutura do grafo --*/
typedef int TipoValorVertice;
typedef int TipoPeso;

typedef struct TipoItem{
    TipoValorVertice Vertice;
    TipoPeso Peso;
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
TipoPeso Peso;
TipoGrafo Grafo;
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

void InsereAresta(TipoValorVertice *V1, TipoValorVertice *V2, TipoPeso *Peso, TipoGrafo *Grafo){
    TipoItem x;
    x.Vertice = *V2;
    x.Peso = *Peso;
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

void ProxAdj(TipoValorVertice *Vertice, TipoGrafo *Grafo, TipoValorVertice *Adj, TipoPeso *Peso, Apontador *Prox, short *FimListaAdj){
    /*-- Retorna Adj do Item apontado por Prox --*/
    *Adj = (*Prox)->Item.Vertice;
    *Peso = (*Prox)->Item.Peso;
    *Prox = (*Prox)->Prox;
    if (*Prox == NULL){
        *FimListaAdj = TRUE;
    }
}

void Retira(Apontador p, TipoLista *Lista, TipoItem *Item){
    /*--Obs.: item a ser retirado e o seguinte ao apontado por p--*/
    Apontador q;
    if(Vazia(*Lista) || p == NULL || p->Prox == NULL){
        printf("Erro: Lista vazia ou posicao nao existe\n");
        return;
    }
    q = p->Prox;
    *Item = q->Item;
    p->Prox = q->Prox;
    if(p->Prox == NULL) Lista->Ultimo = p;
    free(q);
}

void RetiraAresta(TipoValorVertice *V1, TipoValorVertice *V2, TipoPeso *Peso, TipoGrafo *Grafo){
    Apontador AuxAnterior, Aux;
    short EncontrouAresta = FALSE;
    TipoItem x;
    AuxAnterior = Grafo->Adj[*V1].Primeiro;
    Aux = Grafo->Adj[*V1].Primeiro->Prox;
    while(Aux != NULL && EncontrouAresta == FALSE){
        if(*V2 == Aux->Item.Vertice){
            Retira(AuxAnterior, &Grafo->Adj[*V1], &x);
            Grafo->NumArestas--;
            EncontrouAresta = TRUE;
        }
        AuxAnterior = Aux;
        Aux = Aux->Prox;
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

void ImprimeGrafo(TipoGrafo *Grafo, int *codigo){
    int i;
    Apontador Aux;
    for(i = 0; i < Grafo->NumVertices; i++){
        printf("Disciplina %2d: ", codigo[i]);
        if(!Vazia(Grafo->Adj[i])){
            Aux = Grafo->Adj[i].Primeiro->Prox;
            while(Aux != NULL){
                printf("%3d (%d) ", codigo[Aux->Item.Vertice], Aux->Item.Peso);
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
        printf("%3d (%12d)", Aux->Item.Vertice, Aux->Item.Peso);
        Aux = Aux->Prox;
    }
}

/* Função que esquematiza o grafo e suas relações entre disciplinas */
void MontaGrafo(TipoGrafo *Grafo, int *credito){
    /* V1 = 0 */
    V1 = 0; Peso = credito[V1];
    V2 = 7;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 13;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 28;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    /* V1 = 2 */
    V1 = 2; Peso = credito[V1];
    V2 = 14;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    /* V1 = 3 */
    V1 = 3; Peso = credito[V1];
    V2 = 6;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    /* V1 = 5 */
    V1 = 5; Peso = credito[V1];
    V2 = 9;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    /* V1 = 6 */
    V1 = 6; Peso = credito[V1];
    V2 = 10;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 14;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 16;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 17;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 18;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 21;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 22;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 23;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 26;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 27;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 28;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    /* V1 = 7 */
    V1 = 7; Peso = credito[V1];
    V2 = 12;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 17;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    /* V1 = 9 */
    V1 = 9; Peso = credito[V1];
    V2 = 19;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 23;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    /* V1 = 10 */
    V1 = 10; Peso = credito[V1];
    V2 = 15;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 20;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 28;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    /* V1 = 11 */
    V1 = 11; Peso = credito[V1];
    V2 = 25;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    /* V1 = 13 */
    V1 = 13; Peso = credito[V1];
    V2 = 17;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 27;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    /* V1 = 15 */
    V1 = 15; Peso = credito[V1];
    V2 = 19;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    /* V1 = 17 */
    V1 = 17; Peso = credito[V1];
    V2 = 30;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    /* V1 = 18 */
    V1 = 18; Peso = credito[V1];
    V2 = 20;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    V2 = 29;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    /* V1 = 19 */
    V1 = 19; Peso = credito[V1];
    V2 = 24;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    /* V1 = 21 */
    V1 = 21; Peso = credito[V1];
    V2 = 26;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    /* V1 = 23 */
    V1 = 23; Peso = credito[V1];
    V2 = 29;
    InsereAresta(&V1, &V2, &Peso, Grafo);
    /* V1 = 25 */
    V1 = 25; Peso = credito[V1];
    V2 = 29;
    InsereAresta(&V1, &V2, &Peso, Grafo);
}

/* Função que encontra uma ordenação topológica e, em seguida,
mostra o caminho crítico (ordem do caminho e peso total)*/
void CaminhoCritico(TipoGrafo * Grafo, int *codigo, int *credito){
    int entrada[NMaterias], finalizar[NMaterias], n, maiorCaminho = 0, verticeFinal = -1, soma = 0, salvo;
    t_lista *L = criaLista();
    t_lista *S = criaLista();
    t_lista *aux = criaLista();
    t_elemento *atual = aux->inicio;
    for(int i=0; i<NMaterias; i++){
        entrada[i] = 0; //inicializa entradas das matérias = 0
    }
    for(int i=0; i<NMaterias; i++){
        V1 = i;
        if(!ListaAdjVazia(&V1, Grafo)){ //calcula apenas se a lista de adjacentes não está vazia
            Aux = PrimeiroListaAdj(&V1, Grafo); //define começo da lista e percorre os adjacentes
            FimListaAdj = FALSE;
            while(!FimListaAdj){
                ProxAdj(&V1, Grafo, &Adj, &Peso, &Aux, &FimListaAdj);
                entrada[Adj]++; //para cada adjacência encontrada, a entrada é incrementada
            }
        }
    }
    for(int i=0; i<NMaterias; i++){
        if(entrada[i] == 0){
            insereFinal(i, S); //os vértices que possuem entrada = 0 são inseridos na lista S
        }
    }
    while(estaVazia(S) == FALSE){
        n = removerInicio(S);
        insereFinal(n, L); //na lista L são inseridos os vértices que não possuem mais vértices de entrada
        V1 = n;
        if(!ListaAdjVazia(&V1, Grafo)){
            Aux = PrimeiroListaAdj(&V1, Grafo);
            FimListaAdj = FALSE;
            while(!FimListaAdj){
                ProxAdj(&V1, Grafo, &Adj, &Peso, &Aux, &FimListaAdj);
                RetiraAresta(&V1, &Adj, &Peso, Grafo);  //retira todas as adjacências de um vértice e, se seu vizinho não possuir mais
                entrada[Adj]--;                         //entradas (entrada[adj] == 0), ele é inserido na lista S
                if(entrada[Adj] == 0){
                    insereFinal(Adj, S);
                }
            }
        }
    }
    printf("\nOrdenacao topologica:\n");
    imprimirLista(L, codigo); //imprime a ordenação topológica com o código das matérias

    MontaGrafo(Grafo, credito); //remonta o grafo, após ter retirado todas as arestas
    for(int i=0; i<NMaterias; i++){
        finalizar[i] = 0; //inicializa todos os vértices com finalizar[v] = 0
    }
    while(estaVazia(L) == FALSE){
        n = removerInicio(L);
        V1 = n;
        if(!ListaAdjVazia(&V1, Grafo)){
            Aux = PrimeiroListaAdj(&V1, Grafo);
            FimListaAdj = FALSE;
            while(!FimListaAdj){
                ProxAdj(&V1, Grafo, &Adj, &Peso, &Aux, &FimListaAdj);
                finalizar[Adj] = max(finalizar[Adj], finalizar[V1] + Peso); //algoritmo utilizado para calcular o peso total (maior caminho) de todos os vértices
            }
        }
    }
    for(int i=0; i<NMaterias; i++){
        if(finalizar[i] > maiorCaminho){
            maiorCaminho = finalizar[i]; //caminho crítico será o maior deles
            verticeFinal = i;            //e o vértice final do caminho crítico = i
        }
    }

    for(int i=0; i<NMaterias; i++){
        V1 = i;
        if(!ListaAdjVazia(&V1, Grafo)){
            Aux = PrimeiroListaAdj(&V1, Grafo);
            FimListaAdj = FALSE;
            while(!FimListaAdj){
                ProxAdj(&V1, Grafo, &Adj, &Peso, &Aux, &FimListaAdj);
                if(Adj == verticeFinal){
                    insereFinal(V1, S); //encontra vértice precedente ao final e insere na lista S
                }
            }
        }
    }

    while(estaVazia(S) == FALSE){
        n = removerInicio(S);
        if(estaNaLista(n, L) == FALSE){
        insereFinal(n, L);  //insere na lista todos os vértices que possuem uma relação com o vértice final, a partir do último vértice encontrado
        }
        for(int i=0; i<NMaterias; i++){
            V1 = i;
            if(!ListaAdjVazia(&V1, Grafo)){
                Aux = PrimeiroListaAdj(&V1, Grafo);
                FimListaAdj = FALSE;
                while(!FimListaAdj){
                    ProxAdj(&V1, Grafo, &Adj, &Peso, &Aux, &FimListaAdj);
                    if(Adj == n){
                        insereFinal(V1, S); //S sendo utilizada como lista auxiliar
                    }
                }
            }
        }
    }
    insereFinal(verticeFinal, L);   //em L, agora, temos todos os possíveis vértices que fazem parte do caminho crítico
    for(int i=0; i<NMaterias; i++){
        entrada[i] = 0;
    }
    for(int i=0; i<NMaterias; i++){
        V1 = i;
        if(!ListaAdjVazia(&V1, Grafo)){
            Aux = PrimeiroListaAdj(&V1, Grafo);
            FimListaAdj = FALSE;
            while(!FimListaAdj){
                ProxAdj(&V1, Grafo, &Adj, &Peso, &Aux, &FimListaAdj);
                entrada[Adj]++; //verificamos novamente as entradas, para encontrar algum vértice que não possua entrada
            }                   //e este será o vértice inicial
        }
    }

    for(int i=0; i<NMaterias; i++){
        if(entrada[i] == 0 && estaNaLista(i, L)){
            insereFinal(i, S);  //é inserido em S os que não possuem entrada e já estão em L como possíveis vértices de caminho crítico
        }
    }

    n = S->inicio->dado;    //busca o primeiro da lista de S, como possível vértice inicial do caminho crítico
    V1 = salvo = n;
    while(soma != maiorCaminho){
        V1 = salvo;
        if(!ListaAdjVazia(&V1, Grafo)){
            Aux = PrimeiroListaAdj(&V1, Grafo);
            FimListaAdj = FALSE;
            while(!FimListaAdj){
                ProxAdj(&V1, Grafo, &Adj, &Peso, &Aux, &FimListaAdj);
                if(estaNaLista(V1, L) == TRUE){ //caso o vértice esteja em L como possível participante do caminho crítico
                    insereFinal(V1, aux);       //é inserido em uma lista auxiliar
                    soma = soma + Peso;         //variável soma busca obter valor igual ao peso total
                    salvo = Adj;                //salva o adjacente para continuar a busca
                    break;
                }
            }
        }
    }
    atual = aux->inicio;
    printf("\nCaminho critico:\n");
    while(atual!= NULL){
        printf("%d -> ", codigo[atual->dado]);  //imprime o caminho crítico, que foi armazenado na lista aux
        atual = atual->proximo;
    }
    printf("%d\n", codigo[verticeFinal]);   //imprime o último vértice, que não estava na lista aux

    printf("\nDistancia do caminho critico: %d\n", maiorCaminho);   //imprime o peso total
}
/* ============================================================= */
int main(int argc, char *argv[]){

    int codigo[NMaterias], credito[NMaterias];
    char command[50]; //string que armazenará o comando a ser executado pelo sistema (para gerar a imagem)
    
    /* ORDENANDO CÓDIGO DAS MATÉRIAS */

    /* 1º SEMESTRE */
    codigo[0] = 113034;     /*      C1      */
    credito[0] = 6;
    codigo[1] = 113468;     /*      ISC     */
    credito[1] = 4;
    codigo[2] = 113450;     /*      FTC     */
    credito[2] = 4;
    codigo[3] = 113476;     /*      APC     */
    credito[3] = 6;
    codigo[4] = 116726;     /*    INFOSOC   */
    credito[4] = 2;

    /* 2º SEMESTRE */
    codigo[5] = 129011;     /*      CL      */
    credito[5] = 6;
    codigo[6] = 116319;     /*      ED      */
    credito[6] = 4;
    codigo[7] = 113042;     /*      C2      */
    credito[7] = 6;
    codigo[8] = 113093;     /*      IAL     */
    credito[8] = 4;

    /* 3º SEMESTRE */
    codigo[9] = 116394;     /*      OAC     */
    credito[9] = 4;
    codigo[10] = 117889;    /*      TP1     */
    credito[10] = 4;
    codigo[11] = 113107;    /*      A1      */
    credito[11] = 4;
    codigo[12] = 113417;    /*      CN      */
    credito[12] = 4;
    codigo[13] = 115045;    /*      PE      */
    credito[13] = 4;

    /* 4º SEMESTRE */
    codigo[14] = 117366;    /*      LC1     */
    credito[14] = 4;
    codigo[15] = 117897;    /*      TP2     */
    credito[15] = 4;
    codigo[16] = 117901;    /*      TAG     */
    credito[16] = 4;
    codigo[17] = 116572;    /*      RC      */
    credito[17] = 4;

    /* 5º SEMESTRE */
    codigo[18] = 116343;    /*      LP      */
    credito[18] = 4;
    codigo[19] = 117935;    /*      PC      */
    credito[19] = 4;
    codigo[20] = 116441;    /*      ES      */
    credito[20] = 4;
    codigo[21] = 116378;    /*      BD      */
    credito[21] = 4;
    codigo[22] = 116653;    /*      IAA     */
    credito[22] = 4;

    /* 6º SEMESTRE */
    codigo[23] = 116432;    /*      SB      */
    credito[23] = 4;
    codigo[24] = 117960;    /*      FSO     */
    credito[24] = 4;
    codigo[25] = 116882;    /*      AC      */
    credito[25] = 6;
    codigo[26] = 116416;    /*      SI      */
    credito[26] = 4;
    codigo[27] = 117943;    /*      CE      */
    credito[27] = 4;

    /* 7º SEMESTRE */
    codigo[28] = 117536;    /*      PAA     */
    credito[28] = 4;
    codigo[29] = 117951;    /*      COMP    */
    credito[29] = 4;
    codigo[30] = 117927;    /*      SC      */
    credito[30] = 4;

    /* 8º SEMESTRE */
    codigo[31] = 117919;    /*      MC      */
    credito[31] = 2;

    /* FIM DA ORDENAÇÃO */
    
    NVertices = NMaterias;
    Grafo.NumVertices = NVertices; //define número de vértices no grafo
    Grafo.NumArestas = 0; //inicia o grafo com zero arestas (será incrementado a cada chamada InsereAresta)
    FGVazio(&Grafo); //cria grafo vazio
    MontaGrafo(&Grafo, credito); //monta o grafo com as matérias da grade curricular
    //Caso queira visualizar o grafo:
    printf("Ciencia da computacao:\n");
    ImprimeGrafo(&Grafo, codigo); //imprime o grafo com lista de adjacência e o peso das arestas

    CaminhoCritico(&Grafo, codigo, credito); //executa o algoritmo para encontrar o caminho crítico

    LiberaGrafo(&Grafo);   /* Imprime sujeira normalmente */
    ImprimeGrafo(&Grafo, codigo);
    
    strcpy(command, "dot -Tpng grafo.dot -o grafo.png"); //armazena na string o comando para gerar uma imagem do grafo em png
    system(command); //executa o comando da string
    printf("\nImagem do grafo em formato png gerada com sucesso!\n");
    return 0;
}
