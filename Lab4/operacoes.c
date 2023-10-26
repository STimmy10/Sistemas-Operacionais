#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void division_by_zero_handler(int sinal);

int main() {
    // Registrar o manipulador para o sinal SIGFPE
    signal(SIGFPE, division_by_zero_handler);

    int num1, num2;

    printf("Digite o primeiro número real: \n");
    scanf("%d", &num1);
    printf("Digite o segundo número real: \n");
    scanf("%d", &num2);

    printf("O resultado da soma é: %d\n", num1 + num2);
    printf("O resultado da subtração é: %d\n", num1 - num2);
    printf("O resultado da multiplicação é: %d\n", num1 * num2);
    printf("O resultado da divisão é: %d\n", num1 / num2);

    return 0;
}

void division_by_zero_handler(int sinal) {
    printf("Erro: Divisão por zero (sinal %d)\n", sinal);
    exit(1);
}