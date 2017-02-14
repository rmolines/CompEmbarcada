#include <stdio.h>
#define LINE "|   %s   |      R$%f     |    %d    |    R$%f    |\n"
#define TITLE "| Produto | Preço Unitário | Quantidade | Preço Total |\n"
#define SUB "|      |      | Sub-Total    | R$%f |\n"
#define TAX "|      | Imposto 0.05         | R$%f |\n"
#define TOTAL "|       |       | Total        | R$%f |\n"
#define TAX_VALUE 0.05



void make_item (char name[], float price, int q){
    printf(LINE, name, price, q, price*q);
}

void make_sub (float price){
    printf(SUB, price);
}

void make_tax (float price){
    printf(TAX, price);
}

void make_total (float price){
    printf(TOTAL, price);
}

void calculo (float price[], int q[], float *sub, float *taxes, float *total){\
    int i;
    float c;

    for (i=0; i<3; i++){
        c = price[i]*q[i];
        *sub += c;
    }

    *taxes = *sub*TAX_VALUE;
    *total = *sub+*taxes;


}


int main (void){
    char name [3][256];
    int q[3];
    float price[3];
    int i;
    float total, taxes, sub;

    sub = 0;

    for (i=0; i<3; i++){
        scanf("%s %f %d", name[i], &price[i], &q[i]);
    }

    calculo(price, q, &sub, &taxes, &total);

    printf(TITLE);

    for (i=0; i<3; i++){
        make_item(name[i], price[i], q[i]);
    }

    make_sub(sub);
    make_tax(taxes);
    make_total(total);

    return 0;
}