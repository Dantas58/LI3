#include "../Includes/print_outpus.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* função responsável pelo output de queries com apenas uma linha
 * cria o ficheiro necessário, com o nome apropriado, onde escreve o output da query */
int printToFile_OneLine(char *output, int output_number) {

    char file_name[100];

    snprintf(file_name, 100, "Resultados/command%d_output.txt", output_number);

    FILE *fp;
    fp = fopen(file_name, "w+");
    if (!fp) {
        perror("Error");
        return -1;
    }

    fputs(output, fp);
    if (strlen(output) != 0) free(output);

    fclose(fp);

    return 0;
}

/* função responsável pelo output de queries com mais de uma linha
 * cria o ficheiro necessário, com o nome apropriado, onde escreve o output da query */
int printToFile_MultipleLines(char **output, int N, int output_number) {

    char file_name[100];

    snprintf(file_name, 100, "Resultados/command%d_output.txt", output_number);

    FILE *fp;
    fp = fopen(file_name, "w+");
    if (!fp) {
        perror("Error");
        return -1;
    }


    if (output == NULL) return 0;

    for (int i = 0; i < N; i++) {
        fputs(output[i], fp);
        free(output[i]);
    }

    free(output);
    fclose(fp);

    return 0;
}

//função responsável pelo output de queries com apenas uma linha para o terminal
void printToTerminal_OneLine(char *output) {
    printf("%s", output);
    if (strlen(output) != 0) free(output);
}


//função responsável pelo output de queries com mais de uma linha para o terminal
char **printToTerminal_MultipleLines(char **output, int N, int pagina) {

    for (int i = (pagina - 1) * 50; i < N && i < pagina * 50; i++)
        printf("%s", output[i]);

    return output;
}


void paginacaoAnswer(char **answer, int N) {

    if (answer == NULL){
        printf("Não existem resultados para esta procura!\n");
        return;
    }

    int stop = 0, pagina = 1, pagina_ultima = N / 50;

    while (stop == 0) {
        printf("Encontra-se na pãgina %d. Resposta vai até à página %d.\n", pagina, pagina_ultima);
        printf("Escolha a página que quer ver ou 0 para Sair!\n");
        char choice[10];
        fgets(choice, sizeof(choice), stdin);
        int page = (int) strtol(choice, (char **) NULL, 10);
        if (page > 0 && page <= pagina_ultima) {
            printToTerminal_MultipleLines(answer, N, page);
        } else if (page == 0) stop = 1;
    }

    for (int i = 0; i < N; i++) free(answer[i]);
    free(answer);
}