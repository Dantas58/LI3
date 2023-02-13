#define DATE "09/10/2022"

#include "../Includes/date.h"

#include "stdio.h"
#include "stdlib.h"

/* função responsável por criar e preencher a estrutura de uma data
 * dá parse aos dados de uma string e garante que é uma data válida
 * caso a data seja inválida retorna uma data cujo dia é igual a 0 */
int verify_Date(char* token){

    //verify is string format is valid
    int n[3] = { 0 };
    sscanf(token, "%*[0-9]%n/%*[0-9]%n/%*[0-9]%n", &n[0], &n[1], &n[2]);
    if (n[0] != 2 || n[1] != 5 || n[2] != 10){
        return 0;
    }

    int day, month, year;
    sscanf(token, "%d/%d/%d", &day, &month, &year);
    if ((day < 1 || day > 31) || (month < 1 || month > 12)){
        return 0;
    }

    int date = 0;
    date |= (day&0xff); /*day storing in byte 0*/
    date |= (month&0xff)<<8; /*month storing in byte 1*/
    date |= (year&0xffff)<<16; /*year storing in byte 2 and 3*/

    return date;
}

/* função responsável por criar e preencher a estrutura de uma data
 * sem verificar se é uma data válida */
int create_Date(char* token){

    int day,month,year;
    sscanf(token, "%d/%d/%d", &day, &month, &year);

    int date = 0;
    date |= (day&0xff); /*day storing in byte 0*/
    date |= (month&0xff)<<8; /*month storing in byte 1*/
    date |= (year&0xffff)<<16; /*year storing in byte 2 and 3*/
    return date;
}

//divide a data em três ints diferentes (dia, mês e ano)
void get_Date(int date, int* day, int* month, int* year){
    *day = (date&0xff); /*day from byte 0*/
    *month = ((date>>8)&0xff); /*month from byte 1*/
    *year = ((date>>16)&0xffff); /*year from byte 2 and 3*/
}

//função que calcula idade assumindo que o dia atual é 09/10/2022
int calculate_Age(int date){

    int today_day, today_month, today_year;
    get_Date(create_Date(DATE), &today_day, &today_month, &today_year);
    int date_day, date_month, date_year;
    get_Date(date, &date_day, &date_month, &date_year);

    if (today_month < date_month) return (today_year - date_year - 1);
    else if (today_month > date_month) return (today_year - date_year);
    else{
        if (today_day < date_day) return (today_year - date_year - 1);
        else return (today_year - date_year);
    }
}

/* função que compara datas
 * se date1 mais recente retorna 1
 * se date2 mais recente retorna 2
 * se datas são iguais retorna 0 */
int compare_Date(int date1, int date2){

    int date1_day, date1_month, date1_year;
    get_Date(date1, &date1_day, &date1_month, &date1_year);
    int date2_day, date2_month, date2_year;
    get_Date(date2, &date2_day, &date2_month, &date2_year);

    if (date1_year > date2_year) return 1;
    else if (date1_year < date2_year) return 2;
    else{
        if (date1_month > date2_month) return 1;
        else if (date1_month < date2_month) return 2;
        else{
            if (date1_day > date2_day) return 1;
            else if (date1_day < date2_day) return 2;
            else return 0;
        }
    }
}

//função que recebe duas datas e retorna a mais recente
int get_MostRecentDate(int date1, int date2){
    if (compare_Date(date1, date2) == 1) return date1;
    else return date2;
}

//função que recebe uma data e verifica se esta está entre duas datas
int date_InBetween(int date, int first_date, int last_date){
    if (compare_Date(date, first_date) == 2 || compare_Date(date, last_date) == 1) return 0;
    else return 1;
}

int create_DateFromInts(int day, int month, int year){
    int date = 0;
    date |= (day&0xff); /*day storing in byte 0*/
    date |= (month&0xff)<<8; /*month storing in byte 1*/
    date |= (year&0xffff)<<16; /*year storing in byte 2 and 3*/
    return date;
}

//função que retorna o dia seguinte ao dia dado
int nextDay(int date){

    int day, month, year;
    get_Date(date,&day,&month,&year);

    if (day == 31){
        if (month == 12)
            return create_DateFromInts(1,1,year+1);
        else
            return create_DateFromInts(1,month+1,year);
    }

    return create_DateFromInts(day+1,month,year);
}

//função que transforma uma data numa string
char* date_ToString(int date){
    int day, month, year;
    get_Date(date,&day,&month,&year);
    char* data = malloc(15*sizeof (char));
    snprintf(data,15,"%02d/%02d/%d",day,month,year);
    return data;
}

