#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <ctype.h>
#include <unistd.h>


/* ############ ANOTAÇÕES DE VERSÃO ##################

***** 27/10/2016 - 00:32 ******
- Criado função que recebe a posicao dos navios, mas nao funcionou totalmente.
- Tentar zerar a matriz de posicao dos navios com "~"s pois esta com lixo
****************************

****** 27/10/2016 - 20:17 *****
- Concluído função PosicaoNavios() para testes.
	> Necessário ainda criar um método de verificação para mostrar o campo de batalha em tela contendo os icones ex: # ou @
*******************************

****** 27/10/2016 - 22:21 ******
- Reestruturei o código criando uma struct para cada "objeto": Tabuleiro, Jogador, Navios do Jogador, etc...

--- Melhorias a implementar ---
- Irei melhorar o código de ExibirTabuleiro(), para que sempre exiba com a matriz ja alterada,
  com os navios, bombas na água, ondas do mar, etc...
  Essa matriz que será mostrada, será sempre "renderizada" em 
*****************************************************************************

****** 31/10/2016 - 20:44 ********
- Concluído função que recebe posição dos navios do Jogador.
	* Testado em exibir.

--- A implementar ----
	- Na função exibir,receber as posições dos navios do Jogador e do Computador
	  como parâmetro. Nela, serão exibidas o campo de batalha, de acordo com cada bomba jogada, que será verificada
	  em CriarTabuleiro (ou outra função a pensar...)
******************************************************************************

******** 08/11/2016 - 20:37 ***********

- Iniciado a criação da função que escolhe a posição dos navios da máquina aleatóriamente com rand();
  o projeto de função está no arquivo random.c na pasta do projeto Desafio.

- Nota: As funções devem de alguma forma salvar a quantidade de navios de ambos jogadores (maquina e usuario) 
  e atribuir na Struct de vidas. Pois serão validados os acertos com base nesse dado.

---- Metas ----
	
	- Completar o programa para que ele rode, depois melhorar os detalhes (validação de caracteres, etc..)
	- Terminar a função random de posição de navios para a maquina (a quantidade de navios deve ser retornada para a Struct VidasComputador e Jogador)

*****************************************************************

********** 10/11/2016 - 23:49 ********

- Criada as funções que alocam os navios do computador aleatoriamente.

---- Metas ------
	
	- Criar funções de turnos, verificando a quantidade de vida restante dos jogadores.
	  quando acabar as vidas, o player contrario vence.
	- Ao final da batalha, criar uma função que pergunta se o jogador quer jogar novamente.
******************************************** /////// 

*/


//####### Variáveis globais de configuração do Jogo #########

#define TAM_MATRIZ_TABULEIRO 12

#define TAM_MIN_TAB 4 //tamanho minimo do tabuleiro
#define TAM_MAX_TAB TAM_MIN_TAB*2 //tamanho máximo do tabuleiro = Tamanho minimo * 2

/*######### Legenda do campo de batalha para desenvolvedor ##########/*

Obs: Essas legendas servem para verificações no código.
	 Ficará preenchido no Tabuleiro de posição dos navios.

~ (Tio) = Onda do mar
# (Hashtag) = Navio intacto
@ (Arroba) = Tiro na água
X (Letra X do alfabeto) = Tiro certo no Navio 

/*##################################################################*/

typedef struct TamanhoTabuleiro{
	int linhas,
		colunas;
}TAM_TABULEIRO;

typedef struct Posicoes{
	int pos_linha;
	int pos_coluna;
}POSICOES;

typedef struct PosicaoNavios{
	char posicNavJogador[TAM_MATRIZ_TABULEIRO][TAM_MATRIZ_TABULEIRO];
	int qtdNavJogador;

	char posicNavComputador[TAM_MATRIZ_TABULEIRO][TAM_MATRIZ_TABULEIRO];
	int qtdNavComputador;
}POS_NAVIOS;

typedef struct TabuleiroJogo{
	char tabuleiroDeJogo[TAM_MATRIZ_TABULEIRO][TAM_MATRIZ_TABULEIRO];
	int numPartidas;
}TABULEIRO_JOGO;

typedef struct Jogador{
	int vidas;
	int vitorias;
	int derrotas;
}JOGADOR;

typedef struct Computador{
	int vidas;
	int vitorias;
	int derrotas;
}COMPUTADOR;


//############  Protótipos de funções  ###########
TAM_TABULEIRO RecebeTamanhoTabuleiro();// pede o tamanho do tabuleiro para o usuario(Jogador) escolher
TABULEIRO_JOGO CriarTabuleiroJogo(TAM_TABULEIRO TAMTAB);// Cria o tabuleiro do jogo de acordo com o tamanho escolhido
POS_NAVIOS LimpaTabuleiro(TAM_TABULEIRO TAMTAB); //Limpa a matriz tabuleiro e adiciona ondas no campo de batalha.

POSICOES EscolhePosicao(TAM_TABULEIRO TAMTAB);

POS_NAVIOS PosicaoNaviosComputador(TAM_TABULEIRO TAMTAB);//gera a quantidade e a posição dos navios do tabuleiro do computador aleatoriamente
void ExibirTabuleiroComputador(TAM_TABULEIRO TT, POS_NAVIOS PN);// exibe o tabuleiro do Computador

POS_NAVIOS PosicaoNaviosJogador(TAM_TABULEIRO TAMTAB); //função que pega as posições dos navios do jogador e retorna uma struct POSIÇÃO.
void ExibirTabuleiroJogador(TAM_TABULEIRO TT, POS_NAVIOS PN);// exibe o tabuleiro do Jogador


void ExibirTabuleiroVazio(TAM_TABULEIRO TT);

void TurnosAtaques(TAM_TABULEIRO TT, POS_NAVIOS PN, COMPUTADOR CP, JOGADOR JG);

POS_NAVIOS AtaqueJogador(POS_NAVIOS PN);
POS_NAVIOS AtaqueComputador(POS_NAVIOS PN);

JOGADOR VerificaDanoJogador(POS_NAVIOS PN);
COMPUTADOR VerificaDanoComputador(POS_NAVIOS PN);

//------------------------------------------------//

//######## TURNOS DE ATAQUE ##############

void TurnosAtaques(TAM_TABULEIRO TT, POS_NAVIOS PN, COMPUTADOR CP, JOGADOR JG){
	int linha = TT.linhas;
	int coluna = TT.colunas;

	int qtdNavJogador = PN.qtdNavJogador;
	int qtdNavComputador = PN.qtdNavComputador;

	JG.vidas = qtdNavJogador;//define quantidade de vidas na struct Jogador
	CP.vidas = qtdNavComputador;//define quantidade de vidas na struct Computador

	int Jog_atk_l;
	int Jog_atk_c;

	int Comp_atk_l;
	int Comp_atk_c;
	//--------------------------

	do{
		


	}while(JG.vidas > 0 || CP.vidas > 0);


	
}

//#####################################

//######## Ataque do Jogador #######
POS_NAVIOS AtaqueJogador(POS_NAVIOS PN){
	printf("\n\tDigite a posição de linha para o ataque\n");
}

//###### Ataque do Computador ######
POS_NAVIOS AtaqueComputador(POS_NAVIOS PN){

}


TAM_TABULEIRO RecebeTamanhoTabuleiro(){
	TAM_TABULEIRO TT;
	int LIN,
		COL;
	POS_NAVIOS PN;

	system("clear || cls");	
	do{
	printf("\n\tDigite a quantidade de linhas e colunas do tabuleiro de jogo.\n\tExemplo: Digite 4 para um tabuleiro 4x4.\n\tMínimo: %d\n\tMáximo: %d\n\t>> ",TAM_MIN_TAB,TAM_MAX_TAB);
	scanf("%d",&LIN);
	if(LIN < TAM_MIN_TAB || LIN > TAM_MAX_TAB)
		printf("\n\tERRO! Digite um tamanho válido!\n\n");
	else
		COL = LIN;
	}while(LIN < TAM_MIN_TAB || LIN > TAM_MAX_TAB);

	TT.linhas = LIN;
	TT.colunas = COL;	

	return(TT);	

	
}//-----------------------------------------------------------


POS_NAVIOS LimpaTabuleiro(TAM_TABULEIRO TAMTAB){
	int lin = TAMTAB.linhas;
	int col = TAMTAB.colunas;
	int i,j;

	POS_NAVIOS PN;
	

	//Coloca ondas em toda a matriz de posição dos navios do jogador. Isso substitui os lixos de memória
	//da matriz inicial.
	for(i=0; i<lin; i++){
		for(j=0; j<col; j++){
			PN.posicNavJogador[i][j] = '~';
			PN.posicNavComputador[i][j] = '~';
		}
	}

	return(PN);
}//----------------------------------------------------------------------



/*#####################################################################################################
- FUNÇÃO CRIADA PARA OTIMIZAR O CÓDIGO, POIS SERÁ USADO MAIS
  DE UMA VEZ NO JOGO.
- A FUNÇÃO TEM COMO OBJETIVO RETORNAR UMA STRUCT POSICOES(LINHA,COLUNA)
  QUE O USUÁRIO ESCOLHER DO TABULEIRO.
 -> A FUNÇÃO PODE SER USADA PARA ESCOLHER AS POSIÇÕES DOS
 	NAVIOS ESCONDIDOS NO TABULEIRO, OU PARA ATACAR UMA POSIÇÃO.

#######################################################################################################*/

POSICOES EscolhePosicao(TAM_TABULEIRO TAMTAB){

	char linha;
	char maxLinha='A';
	int linhaINT;
	char contLinha;
	int coluna;
	int lin = TAMTAB.linhas;
	int col = TAMTAB.colunas;
	int i,j;
	int maxNavios = lin/2;
	

	int verificaDigito;

	POS_NAVIOS PN;   //Struct do tipo Posição dos Navios
	PN.posicNavJogador[lin][col];  //Define o tamanho da matriz


	maxLinha='A';
	for(i=0; i<(lin-1); i++){
	maxLinha++;
	}

	maxLinha = (int)maxLinha;


do{//Recebe posição de linha

			//system("clear || cls");

			//ExibirTabuleiroJogador(TAMTAB, PN);

			printf("\n\tDigite a Linha para a posição do navio.\n\tDigite apenas letras.\n\tExemplo: Linha 3 = C\n\t>> ");
			linha = getchar( );

			linha = toupper(linha);

			linhaINT = (int)linha;
			
			
			if((linhaINT != 10) && (linhaINT <= 64 || linhaINT > maxLinha))
				printf("\n\n\t| Erro!\n\tValor inválido para linhas.\n\tDica: Se atente para a quantidade de linhas do campo de batalha! :) |\n\n");			
			
		}while(linhaINT <= 64 || linhaINT > maxLinha);


		do{//Recebe posição de coluna
			printf("\n\tDigite a Coluna para a posição do navio.\n\tDigite apenas números de 1 a %d.\n\t>> ",col);
			scanf("%d",&coluna);

			if(coluna < 1 || coluna > col){
				printf("\n\n\t| Erro!\n\tValor inválido para coluna.\n\tDica: Se atente para a quantidade de colunas do campo de batalha! :) |\n\n");
			}

		}while(coluna < 1 || coluna > col);

		//Contador para achar o valor inteiro da posição de linha que é inserida em letras pelo Jogador.
		//Ex: C = 2; D = 3;
		linhaINT=0;
		for(contLinha='A'; contLinha<linha; contLinha++){
			linhaINT++;
		}
		//-----------------------------------

		coluna = coluna-1;//Subtrai 1 da coluna, pois na matriz inicia em '0' e não em '1';

		POSICOES POS;
		POS.pos_linha = linhaINT;
		POS.pos_coluna = coluna;

		return(POS);
	}
//-----------------------------------------------------------------------------------



// ####################  GERA AS POSIÇÕES DOS NAVIOS DO COMPUTADOR ALEATORIAMENTE #########################
POS_NAVIOS PosicaoNaviosComputador(TAM_TABULEIRO TAMTAB){
	int max = TAMTAB.linhas;
	int maxQuantNav = max/2;
	int pos_linha, pos_coluna;
	int qtdNavios;
	int i;

	POS_NAVIOS PN;

	PN = LimpaTabuleiro(TAMTAB); //Limpa Tabuleiro de posição dos navios,
													//retirando os lixos de memoria da matriz POSIÇÃONAVIOSCOMPUTADOR

	//gerando a quantidade de navios, sendo
	// a quantidade máxima = metade da quantidade de linhas do campo
	do{
		qtdNavios = rand() % (maxQuantNav+1);
	}while(qtdNavios < 1);

	PN.qtdNavComputador = qtdNavios;//Adiciona a quantidade de navios do computador à Sruct Posicao Navios

	for(i=0; i<qtdNavios; i++){

		//gerando posição de linha para os navios
		do{
			pos_linha = rand() % (max);
			
		}while(pos_linha < 0 || pos_linha > max);
		//----------------

		//printf("\n|DEBUG|\nNavio %d\nLinha > %d\n",i+1,pos_linha+1);//DEBUG

		//gerando posição de coluna para os navios
		do{
			pos_coluna = rand() % (max);
			
		}while(pos_coluna < 0 || pos_coluna > max);
		//---------------

		//printf("\n|DEBUG|\nNavio %d\nColuna > %d\n",i+1,pos_coluna+1);//DEBUG


		PN.posicNavComputador[pos_linha][pos_coluna] = '#';//ADICIONANDO OS NAVIOS NAS POSIÇÕES DA MATRIZ
	}//------- FIM DO FOR ---------

	//ExibirTabuleiroComputador(TAMTAB, PN);

	return(PN);
}//-------------------------------- FIM DA FUNÇÃO POSICAONAVIOSCOMPUTADOR() ---------------------------------------


//################  RECEBE POSIÇÃO DOS NAVIOS DO JOGADOR NO TABULEIRO ########################
POS_NAVIOS PosicaoNaviosJogador(TAM_TABULEIRO TAMTAB){
	char navio = '#';
	char linha;
	char maxLinha;
	int linhaINT;
	char contLinha;
	int coluna;
	int lin = TAMTAB.linhas;
	int col = TAMTAB.colunas;
	int i,j;
	int maxNavios = lin/2;
	int quantidadeNavios = 0;

	int verificaDigito;

	POSICOES POS;


	POS_NAVIOS PN;   //Struct do tipo Posição dos Navios
	PN.posicNavJogador[lin][col];  //Define o tamanho da matriz



	//Coloca ondas em toda a matriz de posição dos navios do jogador. Isso substitui os lixos de memória
	//da matriz inicial.
	system("clear || cls");
	printf("\n\t---------------------------------------------------");
	printf("\n\t| Seu tabuleiro de jogo tem a seguinte estrutura: |");
	printf("\n\t---------------------------------------------------\n\n");
	PN = LimpaTabuleiro(TAMTAB);
	ExibirTabuleiroJogador(TAMTAB,PN); 

	sleep(2);
	//-------------------------------

	//Pede a quantidade de navios que o jogador irá esconder no campo de batalha
	do{
		verificaDigito=1;
		system("clear || cls");
		printf("\n\tDigite a quantidade de navios para esconder no Campo de Batalha.\n\tMínimo = 1\n\tMáximo = %d\n\t>> ",maxNavios);
		scanf("%d",&quantidadeNavios);

		if(quantidadeNavios < 1 || quantidadeNavios > maxNavios){
			printf("\n\n\t| Erro!\n\tQuantidade inválida!\n\tDica: Se atente a quantidade mínima e máxima para os navios! :) |\n\n");
		}

		PN.qtdNavJogador = quantidadeNavios;//Adiciona a quantidade de navios escolhido a Struct Posicao Navios		
		
	}while(verificaDigito == 0 || (quantidadeNavios < 1 || quantidadeNavios > maxNavios));
	//-------------------------------------------------------------------
	
	for(i=0; i<quantidadeNavios; i++){
		
		printf("\n\t-------------- NAVIO %d --------------\n\n",i+1);
		
		//ExibirTabuleiroVazio(TAMTAB);
		POS = EscolhePosicao(TAMTAB);//funçao que pede a linha e a coluna da posicao para o jogador

		
		//system("cls || clear");
		printf("\n\t----------------------------\n");
		PN.posicNavJogador[POS.pos_linha][POS.pos_coluna] = '#';
		ExibirTabuleiroJogador(TAMTAB,PN);
	}

	return(PN);
}//---------------------- FIM DA FUNÇÃO POSIÇÃOJOGADOR() -------------------------------------------------------------------



/*//CRIA TABULEIRO DE JOGO COM TODAS ALTERAÇÕES
TABULEIRO_JOGO CriarTabuleiroJogo(TAM_TABULEIRO TAMTAB){

	return(TJ);
}*/


//###################### FUNÇÕES DE EXIBIÇÃO DO TABULEIRO #################

void ExibirTabuleiroVazio(TAM_TABULEIRO TT){
	int i,j,
		l,c;
	char let='A';
	
	l = TT.linhas;
	c = TT.colunas;	

	
	printf("\n\n");
	
	for(i=0; i<c; i++){
		printf("\t%d",i+1);
	}
	printf("\n\n");
	for(i=0; i<l; i++){
		printf("  %c",let);
		let++;
		for(j=0; j<c; j++){
			printf("\t~");
		}
		printf("\n\n\n");
	}
}//----------------------------------------------


void ExibirTabuleiroJogador(TAM_TABULEIRO TT, POS_NAVIOS PN){
	int i,j,
		l,c;
	char let='A';
	
	l = TT.linhas;
	c = TT.colunas;

	PN.posicNavJogador[l][c];

	
	printf("\n\n");
	
	for(i=0; i<c; i++){
		printf("\t%d",i+1);
	}
	printf("\n\n");
	for(i=0; i<l; i++){
		printf("  %c",let);
		let++;
		for(j=0; j<c; j++){
			printf("\t%c",PN.posicNavJogador[i][j]);
		}
		printf("\n\n\n");
	}
}//----------------------------------------------

void ExibirTabuleiroComputador(TAM_TABULEIRO TT, POS_NAVIOS PN){
	int i,j,
		l,c;
	char let='A';
	
	l = TT.linhas;
	c = TT.colunas;

	PN.posicNavComputador[l][c];

	
	printf("\n\n");
	
	for(i=0; i<c; i++){
		printf("\t%d",i+1);
	}
	printf("\n\n");
	for(i=0; i<l; i++){
		printf("  %c",let);
		let++;
		for(j=0; j<c; j++){
			printf("\t%c",PN.posicNavComputador[i][j]);
		}
		printf("\n\n\n");
	}
}//-------------------------------------------------------------------------------

void main(){
	setlocale(LC_ALL, "Portuguese"); //Linguagem Português. Corrige problemas com acentuação no Dev C++
	TAM_TABULEIRO TT;
	POS_NAVIOS PN_JOG;
	POS_NAVIOS PN_COMP;

	TT = RecebeTamanhoTabuleiro();
	

	PN_JOG = PosicaoNaviosJogador(TT);

	PN_COMP = PosicaoNaviosComputador(TT);





	//ExibirTabuleiro(TT,PN);

}