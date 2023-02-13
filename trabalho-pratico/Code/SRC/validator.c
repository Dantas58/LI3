#include "../Includes/validator.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>

/* função responsável por retornar um account_status
 * dá parse aos dados de uma string e dá return ao account_status correspondente
 * caso o account_status seja inválido retorna NoStatus */
enum account_status verify_AccountStatus (char* token){
    if (strcasecmp(token,"active") == 0){
        return Active;
    }
    else if (strcasecmp(token,"inactive") == 0){
        return Inactive;
    }
    else return NoStatus;
}

/* função responsável por retornar uma car_class
 * dá parse aos dados de uma string e dá return à car_class correspondente
 * caso a car_class seja inválida retorna NoClass */
enum car_class verify_CarClass (char* token){
    if (strcasecmp(token,"basic") == 0){
        return Basic;
    }
    else if (strcasecmp(token,"green") == 0){
        return Green;
    }
    else if (strcasecmp(token,"premium") == 0) {
        return Premium;
    }
    else return NoClass;
}

int verify_Distance (char* token){
    if (strlen(token) == 0) return 0;
    char* verify;
    int r = (int) strtol(token, &verify, 10);
    if ( (strcmp(verify,"\0") == 0) && r>0 ) return r;
    else return 0;
}

double verify_Score (char* token){
    if (strlen(token) == 0) return -1;
    char* verify;
    double r = strtod(token,&verify);
    if ( (strcmp(verify,"\0") == 0) && r>0 && ceil(r)==floor(r)) return r;
    else return -1;
}

double verify_Tip (char* token){
    if (strlen(token) == 0) return -1;
    char* verify;
    double r = strtod(token,&verify);
    if ( (strcmp(verify,"\0") == 0) && r>=0) return r;
    else return -1;
}

// função que calcula a média das avaliações
double calculate_RatingAverage(double total_rating, int total_rides){
    if (total_rides == 0) return 0;
    return total_rating/total_rides;
}