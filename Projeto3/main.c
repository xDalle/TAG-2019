/*
    Universidade de Brasilia
    Departamento de Ci�ncia da Computa��o
    Projeto 3
    Teoria e Aplica��o de Grafos, Turma A, 1/2019
    Prof. D�bio
    Autores: Lucas Dalle Rocha,     17/0016641
			 Fernando F. Cordeiro,  17/0057950
			 
	O programa consiste em ler o arquivo "professores.txt", em que est�o armazenados
	100 professores, suas habilidades (quantas disciplinas esses professores poder�o
	dar: 1, 2 ou 3) e escolas que s�o suas prioridades para darem aula. Em seguida,
	l� o arquivo "escolas.txt", em que est�o armazenadas 50 escolas, quais habilidades
	elas requerem (dos professores) e quantas vagas possui. Assim, cria um grafo em que,
	dos v�rtices 0-99 est�o os professores, e de 100-149 as escolas. Inicialmente, somente
	as arestas das prioridades dos professores s�o implementadas no grafo, uma vez que as
	arestas das escolas ser�o implementadas somente durante a execu��o do algoritmo de
	emparelhamento est�vel m�ximo.
	
	Dessa forma, com os dados estabelecidos dessa forma, o programa busca, pelo algoritmo de 
	Gale-Shapley, realizar um emparelhamento est�vel m�ximo, pela �tica dos professores. Assim, 
	o programa retorna quais escolas possuem determinados professores, quais escolas n�o possuem
	professores, a quantidade de professores que puderam ser alocados estavelmente, e, de maneira 
	optativa, quais foram eles. Para compilar o programa, execute no terminal:
	
	"gcc main.c -o main" e, em seguida, "./main".
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MaxNumVertices  150
#define FALSE           0
#define TRUE            1
//n�mero total de professores
#define NProfessores 100
//n�mero total de escolas
#define NEscolas 50
//n�mero m�ximo de vagas poss�veis
#define NVagas 2

/*-- Declara��es da estrutura do grafo --*/
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
    /*-- Insere depois do �ltimo item da lista --*/
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

void ImprimeGrafo(TipoGrafo *Grafo){
    int i;
    Apontador Aux;
    for(i = 0; i < Grafo->NumVertices; i++){
        printf("Vertice %2d: ", i);
        if(!Vazia(Grafo->Adj[i])){
            Aux = Grafo->Adj[i].Primeiro->Prox;
            while(Aux != NULL){
                printf("%3d (%d) ", Aux->Item.Vertice, Aux->Item.Peso);
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

/* Fun��o que esquematiza os professores no grafo */
void setProfessores(TipoGrafo *Grafo, int *HAB){
	//O peso das arestas representa a prioridade de um professor em rela��o � uma escola
	//A prioridade � estabelecida de menor para maior (1 � a maior prioridade)
	//TEMP1 � uma vari�vel utilizada para funcionar como contador
    int TEMP1 = 0, PesoPrioridade;
	//A string "str" armazenar� cada linha do arquivo "professores.txt"
	//E a "armazenaNUM" inicia vazia e ir� armazenar um n�mero de at� 3 algarismos
	//Futuramente ser� convertida em um inteiro pela fun��o "atoi"
    char str[30], armazenaNUM[3] = {' '};
	//Vari�vel que receber� o arquivo
    FILE *arq;
	
	//Abre arquivo professores.txt para leitura apenas
    arq = fopen("professores.txt", "r");
	//L� at� o fim do arquivo
    while(!feof(arq)){
		//Armazena cada linha em "str"
        fgets(str, 30, arq);
		//Seta prioridade inicial como 1
        PesoPrioridade = 1;
		//L� cada caractere da linha
        for(int i=0; i<strlen(str); i++){
            switch(str[i]){
		    //Caso caractere seja igual a 'P'
            case 'P':
				//Armazena o n�mero seguinte a P, dependendo da quantidade de algarismos
                armazenaNUM[0] = str[i+1];
                if(str[i+2] != ' '){
                    armazenaNUM[1] = str[i+2];
                }
                if(str[i+3] != ' '){
                    armazenaNUM[2] = str[i+3];
                }
				//V1 recebe o n�mero do v�rtice do professor
                V1 = atoi(armazenaNUM) - 1;
				//reseta o armazenamento de n�meros
                armazenaNUM[0] = armazenaNUM[1] = armazenaNUM[2] = ' ';
                break;
			//Caso caractere seja igual a 'H'
            case 'H':
				//Armazena o n�mero de algarismo �nico
                armazenaNUM[0] = str[i+1];
				//Recebe habilidade de determinado professor
                HAB[TEMP1] = atoi(armazenaNUM);
				//Incrementa contador
                TEMP1++;
				//reseta armazenamento de n�meros
                armazenaNUM[0] = ' ';
                break;
			//Caso caractere seja igual a 'E'
            case 'E':
				//Armazena n�mero de at� dois algarismos
                armazenaNUM[0] = str[i+1];
                if(str[i+2] != ' '){
                    armazenaNUM[1] = str[i+2];
                }
				//V2 recebe o n�mero da escola, mas para representa��o no grafo somamos 99
				//Assim, escola 1, por exemplo, � criada no v�rtice 1+99 = 100,
				//ap�s representa��o dos professores (de 1 a 99)
                V2 = atoi(armazenaNUM) + 99;
				//reseta o armazenamento de n�meros
                armazenaNUM[0] = armazenaNUM[1] = ' ';
                Peso = PesoPrioridade;
                InsereAresta(&V1, &V2, &Peso, Grafo);
				//Incrementa peso
                PesoPrioridade++;
                break;
            default:
                break;
            }
        }
    }
	//Fecha e libera arquivo
    fclose(arq);
    free(arq);
}

/* Fun��o que esquematiza os v�rtices das escolas no grafo */
void setEscolas(TipoGrafo *Grafo, int HABE[NEscolas][NVagas]){
	//TEMP � meramente uma vari�vel tempor�ria
	//vaga � uma var�avel que oscila entre 0 e 1
	//Ela estabelece qual vaga est� sendo adotada
    int TEMP, vaga;
	//Semelhante � fun��o "setProfessores"
    char str[10], armazenaNUM[2] = {' '};
	
    FILE *arq;
    arq = fopen("escolas.txt", "r");
    while(!feof(arq)){
        fgets(str, 10, arq);
		//vaga inicia como zero
        vaga = 0;
        for(int i=0; i<strlen(str); i++){
            switch(str[i]){
            case 'E':
                armazenaNUM[0] = str[i+1];
                if(str[i+2] != ' '){
                    armazenaNUM[1] = str[i+2];
                }
				//V1 recebe o n�mero da escola - 1 (para ser armazenada na matriz)
                V1 = atoi(armazenaNUM) - 1;
				//reseta o armazenamento de n�meros
                armazenaNUM[0] = armazenaNUM[1] = ' ';
                break;
            case 'H':
                armazenaNUM[0] = str[i+1];
				//TEMP recebe o n�mero da habilidade
                TEMP = atoi(armazenaNUM);
				//Armazena na matriz e em determinada vaga qual habilidade � requerida
                HABE[V1][vaga] = TEMP;
				//Incrementa vaga
                vaga++;
				//reseta armazenamento de n�meros
                armazenaNUM[0] = ' ';
                break;
            default:
                break;
            }
        }
    }
    fclose(arq);
    free(arq);
}

/* Fun��o que verifica se h� professores livres, que ainda n�o foram analisados em todas suas possibilidades de escolas */
int verifyProfLivres(TipoGrafo *Grafo, int *marcado, int *contaPrioridade, int *grauProfessores){
    for(int i = 0; i < NProfessores; i++){
		//Caso n�o esteja marcado
        if(marcado[i] == FALSE){
			//contaPrioridade estabelece uma contagem de quantas arestas foram analisadas
			//grauProfessores estabelece quantas arestas cada professor possui
			//caso n�o tenha terminado a an�lise de suas arestas, retorna i como V1
            if(grauProfessores[i] >= contaPrioridade[i]){
                return i;
            }
        }
    }
	//caso tenha terminado, retorna -1
    return -1;
}

/* Algoritmo Gale-Shapley aplicado no problema */
void GaleShapley(TipoGrafo *Grafo, int *HAB, int HABE[NEscolas][NVagas]){
	
	/* - marcado armazena se determinado professor j� foi alocado em alguma escola
	   - contaPrioridade armazena a contagem de arestas analisadas de cada professor
	   - grauProfessores armazena quantas arestas possui cada professor
	   - verificaFim armazena V1 ou -1 caso n�o possua outro v�rtice a ser analisado 
	   - TEMP armazena os pesos de cada professor ao ser alocado em alguma escola
	   - Peso1 e Peso2 s�o booleanas que armazenam a presen�a de vaga requisitada 1 ou 2
	   - vaga armazena qual vaga est� sendo analisada (1 ou 2)
																						*/
																						
    int marcado[NProfessores], contaPrioridade[NProfessores], grauProfessores[NProfessores],
	verificaFim, TEMP[NProfessores], Peso1, Peso2, vaga = 1;
	
	//Inicia as vari�veis
    for(int i=0; i<NProfessores; i++){
        marcado[i] = FALSE;
        grauProfessores[i] = 0;
        TEMP[i] = 0;
		contaPrioridade[i] = 1;
    }
	
	//Calcula o grau de cada professor (quantas arestas possui)
    for(int i=0; i<NProfessores; i++){
        V1 = i;
		//calcula apenas se a lista de adjacentes n�o est� vazia
        if(!ListaAdjVazia(&V1, Grafo)){
			//define come�o da lista e percorre os adjacentes
            Aux = PrimeiroListaAdj(&V1, Grafo);
            FimListaAdj = FALSE;
			//At� o fim da lista, incrementa cada adjac�ncia
            while(!FimListaAdj){
                ProxAdj(&V1, Grafo, &Adj, &Peso, &Aux, &FimListaAdj);
                grauProfessores[i]++;
            }
        }
    }
	
	//Inicia o valor da var�avel verificaFim
    verificaFim = verifyProfLivres(Grafo, marcado, contaPrioridade, grauProfessores);
	
	//Analisa at� o fim das primeiras vagas ou at� finalizar tudo completamente
    while(verificaFim != -1){
        V1 = verificaFim;
		//Pesos iniciais definidos como zero
        Peso1 = Peso2 = FALSE;
		//Verifica as adjacencias de V1
        if(!ListaAdjVazia(&V1, Grafo)){
            Aux = PrimeiroListaAdj(&V1, Grafo);
            FimListaAdj = FALSE;
            while(!FimListaAdj){
                ProxAdj(&V1, Grafo, &Adj, &Peso, &Aux, &FimListaAdj);
				//S� � analisado quando o Peso for igual ao contador de prioridade
                if(Peso == contaPrioridade[V1]){
				//V2 recebe escola a ser analisada
                V2 = Adj;
				//TEMP armazena a prioridade de V1
                TEMP[V1] = Peso;
				//Incrementa prioridade para uma futura an�lise do mesmo professor
                contaPrioridade[V1]++;
                break;
                }
            }
        }
		
		//Caso a escola ainda n�o possua professores
        if(ListaAdjVazia(&V2, Grafo)){
			//Aloca primeiramente as primeiras vagas e somente se o professor n�o estiver marcado
			//e sua habilidade for maior ou igual a requerida
            if(marcado[V1] == FALSE && vaga == 1 && HAB[V1] >= HABE[V2-100][0]){
				//Peso setado como 1, para representar primeira vaga
                Peso = 1;
				//Escola V2 aloca professor V1 na primeira vaga
                InsereAresta(&V2, &V1, &Peso, Grafo);
				//Marca professor V1
                marcado[V1] = TRUE;
			//Aloca agora as segundas vagas se o professor n�o estiver marcado, possuir uma segunda vaga
			//e sua habilidade for maior ou igual a requerida
            }else if(marcado[V1] == FALSE && vaga == 2 && HABE[V2-100][1] != 0 && HAB[V1] >= HABE[V2-100][1]){
				//Peso setado como 2, para representar segunda vaga
                Peso = 2;
				//Escola V2 aloca professor V1 na segunda vaga
                InsereAresta(&V2, &V1, &Peso, Grafo);
				//Marca professor V1
                marcado[V1] = TRUE;
            }
		//Caso a escola j� possua professores	
        }else{
			//An�logo ao anterior, com algumas peculiaridades
            if(marcado[V1] == FALSE && vaga == 1 && HAB[V1] >= HABE[V2-100][0]){
                Aux = PrimeiroListaAdj(&V2, Grafo); 
                FimListaAdj = FALSE;
                while(!FimListaAdj){
                    ProxAdj(&V2, Grafo, &Adj, &Peso, &Aux, &FimListaAdj);
					//Caso a escola V2 j� possua um professor na primeira vaga,
					//Peso1 recebe 1
                    if(Peso == 1){
                        Peso1 = TRUE;
                        break;
                    }
                }
				//Caso n�o possua um professor na primeira vaga, apenas insere esse
				//novo professor na primeira vaga
                if(Peso1 == FALSE){
                    Peso = 1;
                    InsereAresta(&V2, &V1, &Peso, Grafo);
                    marcado[V1] = TRUE;
                }else{
					//Caso exista, analisa se a prioridade do professor que est� na escola
					//� maior do que a desse novo professor
                    if(TEMP[Adj] > TEMP[V1]){
						//Caso seja, retira o antigo professor
                        RetiraAresta(&V2, &Adj, &Peso, Grafo);
                        Peso = 1;
						//Insere o novo
                        InsereAresta(&V2, &V1, &Peso, Grafo);
						//Marca o novo como TRUE e antigo como FALSE, respectivamente
                        marcado[V1] = TRUE;
                        marcado[Adj] = FALSE;
                    }
					//Caso a prioridade seja igual, o programa busca alocar o professor de menor habilidade requerida,
					//com o intuito de reservar um professor de maior habilidade para uma outra escola
                    if(TEMP[Adj] == TEMP[V1]){
                        if(HAB[V1] < HAB[Adj]){
                            RetiraAresta(&V2, &Adj, &Peso, Grafo);
                            Peso = 1;
                            InsereAresta(&V2, &V1, &Peso, Grafo);
                            marcado[V1] = TRUE;
                            marcado[Adj] = FALSE;
                        }
                    }
                }
            }
			//Agora, para a segunda vaga, se existir
            if(marcado[V1] == FALSE && vaga == 2 && HABE[V2-100][1] != 0 && HAB[V1] >= HABE[V2-100][1]){
				//Verifica se a escola j� possui segunda vaga preenchida e armazena em Peso2
                Aux = PrimeiroListaAdj(&V2, Grafo);
                FimListaAdj = FALSE;
                while(!FimListaAdj){
                    ProxAdj(&V2, Grafo, &Adj, &Peso, &Aux, &FimListaAdj);
                    if(Peso == 2){
                        Peso2 = TRUE;
                        break;
                    }
                }
				//Caso n�o exista, aloca professor na escola na segunda vaga
                if(Peso2 == FALSE){
                    Peso = 2;
                    InsereAresta(&V2, &V1, &Peso, Grafo);
                    marcado[V1] = TRUE;
                }else{
					//Caso exista, aloca analogamente ao caso anterior (da vaga 1)
                    if(TEMP[Adj] > TEMP[V1]){
                        RetiraAresta(&V2, &Adj, &Peso, Grafo);
                        Peso = 2;
                        InsereAresta(&V2, &V1, &Peso, Grafo);
                        marcado[V1] = TRUE;
                        marcado[Adj] = FALSE;
                    }
                    if(TEMP[Adj] == TEMP[V1]){
                        if(HAB[V1] < HAB[Adj]){
                            RetiraAresta(&V2, &Adj, &Peso, Grafo);
                            Peso = 2;
                            InsereAresta(&V2, &V1, &Peso, Grafo);
                            marcado[V1] = TRUE;
                            marcado[Adj] = FALSE;
                        }
                    }
                }
            }
        }
		//Por fim, verifica se programa chegou ao fim e, caso tenha chegado mas
		//somente analisado a primeira vaga, reseta o contador de prioridade e
		//analisar� a segunda vaga, posteriormente
        verificaFim = verifyProfLivres(Grafo, marcado, contaPrioridade, grauProfessores);
        if(verificaFim == -1 && vaga == 1){
            for(int i=0; i<NProfessores; i++){
                contaPrioridade[i] = 1;
            }
            vaga = 2;
            verificaFim = 0;
        }
    }
}

/* Fun��o que imprime as rela��es requisitadas pelo projeto */
void ImprimeRelacao(TipoGrafo *Grafo){
	
	/* - ctd � uma vari�vel utilizada como contador
	   - temProf � um vetor que armazena se cada escola possui professor
	   - alocados � um vetor que armazena quais professores foram alocados
	   - opcao ser� utilizada futuramente como alternativa de imprimir os 
		 professores alocados ou n�o
																	*/
																	
    int ctd = 0, temProf[NEscolas], alocados[NProfessores], opcao;
	
	//Inicia os vetores como FALSE
    for(int i=0; i<NEscolas; i++){
        temProf[i] = FALSE;
    }
    for(int i=0; i<NProfessores; i++){
        alocados[i] = FALSE;
    }
	
    printf("ESCOLAS COM PROFESSORES:\n");
	//Verifica a partir do v�rtice 100 at� o 149 (onde est�o armazenadas as escolas)
    for(int i = NProfessores; i < NProfessores+NEscolas; i++){
        V1 = i;
        if(!ListaAdjVazia(&V1, Grafo)){
            Aux = PrimeiroListaAdj(&V1, Grafo);
            FimListaAdj = FALSE;
            while(!FimListaAdj){
				//Caso possua adjac�ncia com determinado professor, o vetor "alocado" dele
				//� setado como TRUE
                ProxAdj(&V1, Grafo, &Adj, &Peso, &Aux, &FimListaAdj);
                alocados[Adj] = TRUE;
				//ctd utilizado meramente de maneira est�tica, caso seja a primeira adjac�ncia
                if(ctd == 0){
					//Novamente, retira-se 99 pois os v�rtices das escolas come�am em 100
					//E soma-se a adjac�ncia, pois os v�rtices dos professores come�am em 0
                    printf("Escola %2d possui professores: %2d", V1-99, Adj+1);
					//Caso a escola tenha professor, seta o vetor dela como TRUE
                    temProf[V1-100] = TRUE;
					//Incrementa ctd para n�o repetir o printf
                    ctd++;
                }else{
					//Caso tenha mais adjac�ncia, somente imprime em seguida
                    printf(" %2d ", Adj+1);
                }
            }
        }
		//Caso a escola possua adjac�ncia, imprime um salto de linha e reseta o contador
        if(!ListaAdjVazia(&V1, Grafo)){
        printf("\n");
        ctd = 0;
        }
    }
	
	//Imprime escolas sem professores, caso vetor temProf seja igual a FALSE
    printf("ESCOLAS SEM PROFESSORES:\n");
    for(int i=0; i<NEscolas; i++){
        if(temProf[i] == FALSE){
            printf("Escola %2d nao possui professores\n", i+1);
        }
    }
	
	//Reseta contador pois ser� utilizado para contar quantos professores puderam ser
	//alocados estavelmente
	ctd = 0;
	for(int i=0; i<NProfessores; i++){
		if(alocados[i] == TRUE){
			ctd++;
		}
	}
	
	printf("NUMERO DE PROFESSORES ALOCADOS ESTAVELMENTE: %d\n", ctd);
	//Imprime dependendo da op��o selecionada, que deve ser obrigatoriamente 1 ou 2
	while(opcao != 1 && opcao != 2){
		printf("Deseja imprimir todos os professores que foram alocados?\n1- Sim\n2- Nao\n");
		scanf("%d", &opcao);
	}
	
    if(opcao == 1){
        printf("PROFESSORES QUE FORAM ALOCADOS:\n");
        for(int i=0; i<NProfessores; i++){
            if(alocados[i] == TRUE){
                printf("%d\n", i+1);
            }
        }
    }
}

/* ============================================================= */

int main(int argc, char *argv[]){
	//O vetor HABProfessores armazena todas as habilidades dos 100 professores
	//A matriz HABEscolas armazena quais vagas cada escola possui (no m�ximo duas vagas)
    int HABProfessores[NProfessores], HABEscolas[NEscolas][NVagas]; //2 � o n�mero de vagas poss�veis
	
    for(int i=0; i<NEscolas; i++){
        for(int j=0; j<NVagas; j++){
			//inicialmente, cada escola � setada em 0, isto �, n�o possui vagas (para evitar lixo na matriz)
            HABEscolas[i][j] = 0;
        }
    }
	
	//define n�mero de v�rtices que ir�o compor o grafo como soma de professores e escolas (150)
    NVertices = NProfessores + NEscolas;
	//define n�mero de v�rtices no grafo
    Grafo.NumVertices = NVertices;
	//inicia o grafo com zero arestas (ser� incrementado a cada chamada InsereAresta)
    Grafo.NumArestas = 0;
	//cria grafo vazio
    FGVazio(&Grafo);
	
	//seta os v�rtices dos professores no grafo (0-99) e armazena suas habilidades no vetor
    setProfessores(&Grafo, HABProfessores);
	//armazena as habilidades requeridas pelas escolas no vetor
    setEscolas(&Grafo, HABEscolas);
	
	//executa o algoritmo de Gale-Shapley
    GaleShapley(&Grafo, HABProfessores, HABEscolas);
	
	//Imprime as rela��es requeridas pelo projeto
    ImprimeRelacao(&Grafo);
	
    LiberaGrafo(&Grafo);   /* Imprime sujeira normalmente */
    ImprimeGrafo(&Grafo);

    return 0;
}