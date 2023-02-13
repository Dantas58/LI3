#ifndef PROJETO_PRINT_OUTPUS_H
#define PROJETO_PRINT_OUTPUS_H

int printToFile_OneLine(char *output, int output_number);
int printToFile_MultipleLines(char **output, int N, int output_number);

void printToTerminal_OneLine(char *output);
void paginacaoAnswer(char **answer, int N);

#endif //PROJETO_PRINT_OUTPUS_H
