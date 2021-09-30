# Trabalho de Sistemas Operacionais - _Vaccine Shel_
 
Este trabalho é uma simulação de um shell com algumas características a mais. O vsh é o shell e temos dois vírus fatais circulando entre os programa (SIGUSR1 e SIGSR2). O vsh está imune as esses dois sinais, porém tem um comportamente dos demais processos criados durante a execução, ele imprime uma mensagem na tela. Além disso, sinais enviados pelo teclado não funcionam no vsh e nem no filho em foreground.
 	Quando o vsh recebe apenas um comando como parâmetro ele executa o processo em foreground, esse processo também está imune, mas não exibe nada na tela. Quando o vsh recebe mais de um comando atráves de pipes, exemlpo:
	
	`ls | sort`

Esses programas vão ser executados em background, e para gerenciar uma funcionalidade do trabalho, os processos criados em BG estarão no mesmo grupo, e quando qualquer um deles recebe _SIGUSR1_ ou _SIGUSR2_ o grupo todo deve encerrar a execução. O vsh cria um processo "sentinela" para gerenciar esses processos. Eventualmente esses sentinelas viram "zombies", mas o vsh de tempos em tempos faz uma limpeza desses processos "zombies".

O vsh termina quando recebe o comando _armagedon_ ele encerra todas os processos e para a sua execução. O comando _liberamoita_ faz a limpeza de todos os processos que estão em estadp "zombie".

---

## Como executar

No seu terminal, no diretório dos arquivos, digite o comando:

	`make`

e execute `./vsh`. A partir de agora o vsh já está rodando e pode receber comandos da "mesma maneira" que o shell comum. 

NOTA: Não deve receber mais do que 5 comandos com uso de | (pipe).