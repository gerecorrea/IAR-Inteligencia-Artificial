#include <bits/stdc++.h>

/* Anotações
	Dupla: Beatriz e Geremias
	
	Feito:
		Ambientação do problema, imposição da variáveis do problema
		Tudo principal por cmd até então
		Configuração aleatório do ambiente a cada iteração
		Fazendo automatizadamente 10 testes, com impressão dos dados de média e desvio padrão
		Gerando um arquivo de saída, formato .xlsx, para cada teste executado. Pode ser modificado na variável "formato"

	Fazer:
		Tudo certo.

	Compilação e execução exemplo:
	$ g++ -Wall -O3 -o e codRS.cpp
	$ ./e uf20-91-01.cnf saida

*/

using namespace std;

int qtd_iteracoes = 250000; //quantidade descrita na documentação
int numero_execucoes = 10; //quantidade descrita na documentação

//Para geração de configurações inteiramente aleatorias (dar uma olhada melhor nisso aqui dps)
//motor de geração aleatoria - mersenne_twister_engine, este para 64 bits.
random_device randomm;
mt19937_64 engine{randomm()};
uniform_real_distribution<double> distribuicaoBool{0.0, 1.0}; //realiza uma distribuição uniforme de valores entre 0 a 1 (0.00 a 1.00).

//Struct que representa as clausulas, armazenando as variáveis e as cláusulas satisfeitas
struct Clausula {
    vector<int> variaveis_clausula; //representa as 3 variáveis de uma cláusula

	//Construtor de uma cláusula, cujo recebe um vetor de inteiros (com 3 variaveis)
    Clausula(vector<int> &vecVar){
        variaveis_clausula = vecVar;
    }

	//Função que dado a configuração atual de todas as variaveis  e as variaveis que pertencem a essa clausula, verifico se ao menos uma é satisfeita 
	int verifica_satisfatoriedade(vector<bool> &configuracao_atual){
		bool satisfeito = false;
		for(int i=0; i<variaveis_clausula.size(); i++){
			if(variaveis_clausula[i] > 0){  //caso a variavel de posição i seja positiva
				
				if(configuracao_atual[variaveis_clausula[i]-1] == 1) //verifico se a configuração atual tem ela positiva, se sim, satisfaz e retorno true
					return true;

			} else { //caso a variável de posição i seja negativa
				if(configuracao_atual[abs(variaveis_clausula[i]-1)] == 0) //verifoc se a configuração atual tem ela negativa, se sim, satisfaz e retorno false.
					return true;
			}
		}
		return false; //a configuração não é satisfeita, retorno false.
	}

};

//Função para geração aleatória das variáveis. É igual a geração inicial do SA no main (aleatória).
vector<bool> gerarConfiguracaoAleatoria(int numero_variaveis){
    vector<bool> vetor_auxiliar;
    for(int i = 0; i < numero_variaveis; i++){
        double aleatorio = distribuicaoBool(engine);
		if(aleatorio > 0.5)
			vetor_auxiliar.push_back(1);
		else
			vetor_auxiliar.push_back(0);
    }
    return vetor_auxiliar;
}

//Função que verificar quantas claúsulas das totais dado a configuração atual são satisfeitas
int verificaQuantidadeClausulasSatisfeitas(vector<Clausula> &Clausulas, vector<bool> &configuracao_atual){
	int satisfeitas = 0;
	//Itero sobre cada clausula sat, verificando se ao menos uma das variáveis dessa clausula é satisfeita, se sim, qtd de satisfeitas (res) é incrementado
	for(Clausula i : Clausulas){
		if(i.verifica_satisfatoriedade(configuracao_atual)){
			satisfeitas++;
		}
	}
	return satisfeitas;
}

int main(int argc, char const *argv[]) {
	int numero_variaveis; //quantidade de variáveis do sistema (arquivo de entrada)
	int numero_clausulas; //quantidade de cláusulas do sistema (arquivo de entrada)
	ifstream arquivo_entrada;
	ofstream arquivo_saida;
	string s;
	int i;

	//Variáveis de melhore soluções:
	int melhor_solucao_clausulas_satisfeitas;
	vector<bool> melhor_solucao_configuracao_variaveis;

	//variaveis de estatistica total:
	int vetor_solucoes[numero_execucoes];
	double soma = 0;
	double media;
	double desvio_padrao;
	double somaIteracoes = 0;
	double mediaIteracoes;


	if (argc != 3){
		printf("2 argumentos: <arquivoEntrada> <arquivoSaida> \n");
		return 1;
	}


	// INICIO leitura da entrada
	arquivo_entrada.open(argv[1]);
	if (!arquivo_entrada.is_open()){
		printf("Erro na abertura do arquivo \"%s\" para entrada\n", argv[1]);
	}

	//Dados para saída.
	string arqSaida;
	string formato = ".xlsx";

	arquivo_entrada >> s >> s;
	arquivo_entrada >> numero_variaveis >> numero_clausulas;
	vector<Clausula> Clausulas;

    for(int i = 0; i < numero_clausulas; i++){
        vector<int> vet;
        for(int j = 0; j < 3; j++){
            int x;
            arquivo_entrada >> x;
            vet.push_back(x);
        }
		int t;
		arquivo_entrada >> t; // leio o 0 (serve como ^)
		Clausulas.push_back(Clausula(vet));
    }
	arquivo_entrada.close();
	
	//Variáveis de iteração comum e que variam constantemente
	vector<bool> configuracao_atual;
	int clausulas_satisfeitas;

	for(int j=0; j<numero_execucoes; j++){
		//Gera uma configuração inicial aleatoria
		configuracao_atual.clear();
		configuracao_atual = gerarConfiguracaoAleatoria(numero_variaveis);
		clausulas_satisfeitas = verificaQuantidadeClausulasSatisfeitas(Clausulas, configuracao_atual);

		melhor_solucao_clausulas_satisfeitas = clausulas_satisfeitas;
		melhor_solucao_configuracao_variaveis = configuracao_atual;
		
		arqSaida = "/0"; //garantir que fique vazia
		arqSaida = argv[2] + to_string(j) + formato; //altero o nome para saída.
		arquivo_saida.open(arqSaida);
		
		for(i = 1; i < qtd_iteracoes; i++){
			//Vai iterando e gerando configurações aleatórios, a fim de ver se alguma se torna melhor
			configuracao_atual = gerarConfiguracaoAleatoria(numero_variaveis);
			clausulas_satisfeitas = verificaQuantidadeClausulasSatisfeitas(Clausulas, configuracao_atual);
			
			//Se a qtd de clausulas satisfeitas é maior que a grava na solução final atual, então troca.
			if(clausulas_satisfeitas > melhor_solucao_clausulas_satisfeitas){
				melhor_solucao_clausulas_satisfeitas = clausulas_satisfeitas;
				melhor_solucao_configuracao_variaveis = configuracao_atual;
			}

			arquivo_saida << i << " " << melhor_solucao_clausulas_satisfeitas << "\n";

			if(clausulas_satisfeitas == numero_clausulas){//Caso esteja totalmente satisfeito, encerra a busca.
				cout << "Completamente satisfeito com " << i << " iterações" << endl;
				break; 
			}
		}
		if(i == qtd_iteracoes)
			somaIteracoes += qtd_iteracoes;
		else
			somaIteracoes += i - 1;//pois nesse caso ele deu um i++ extra!
		
		vetor_solucoes[j] = melhor_solucao_clausulas_satisfeitas;
		soma += melhor_solucao_clausulas_satisfeitas;
		
		arquivo_saida.close();

		cout << endl << "###### GERAÇÃO DE DADOS DE SAÍDA ######" << endl;
		cout << "Quantidade de iterações: " << qtd_iteracoes << endl;
		cout << "Quantidade de iterações realizadas: " << i << endl;
		cout << "Satisfeitas " << melhor_solucao_clausulas_satisfeitas << " de " << numero_clausulas << " clausulas." << endl << endl;

		cout << "Configuração das variáveis finais: " << endl;
		for (i = 0; i < melhor_solucao_configuracao_variaveis.size(); i++){
			if(i%7==0)
				cout << endl << " ";
			if(melhor_solucao_configuracao_variaveis[i] == 1)
				cout << " x" << i+1 << ";\t";
			else if(melhor_solucao_configuracao_variaveis[i] == 0)
				cout << "~x" << i+1 << ";\t";
		} 
		cout << endl << endl << "###################################################\n" << endl;
		cout << endl;
	} 

	//Dados estatísticos:
	cout << "Saída dos dados estáticos para os " << numero_execucoes << " testes:" << endl;
	media = soma/numero_execucoes;
	cout <<  "media:" << media << endl;
	double somatorioDesvio = 0;
	int max = vetor_solucoes[0], indice = 0; //para pegar o melhor teste
	for(i=0;i<numero_execucoes;i++){
		somatorioDesvio += pow(vetor_solucoes[i] - media, 2);
		//Para pegar a melhor execução
		if(vetor_solucoes[i] > max){
			max = vetor_solucoes[i];
			indice = i;
		}
	}

	desvio_padrao = somatorioDesvio / numero_execucoes;

	cout << "Desvio padrão: " << desvio_padrao << endl;

	mediaIteracoes = somaIteracoes / numero_execucoes;
	cout << "Média de iterações realizadas: " << mediaIteracoes << endl;
	
	cout << "Melhor teste é o teste " << indice << " com " << max << " de " << numero_clausulas << " cláusulas satisfeitas." << endl; 

	cout << endl << "Gerados " << numero_execucoes << " arquivos de saída no formato " << formato << " com as iterações atuais e a quantidades de cláusulas satisfeitas em cada iteração." << endl;
	
	cout << endl << "#################################################\n" << endl;

    return 0;
}
