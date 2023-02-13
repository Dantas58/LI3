#ifndef PROJETO_VALIDATOR_H
#define PROJETO_VALIDATOR_H

enum account_status{
    NoStatus,
    Active,
    Inactive
};

enum car_class{
    NoClass,
    Basic,
    Green,
    Premium,
};


enum account_status verify_AccountStatus (char* token);
enum car_class verify_CarClass (char* token);
int verify_Distance (char* token);
double verify_Score (char* token);
double verify_Tip (char* token);

double calculate_RatingAverage(double total_rating, int total_rides);


#endif //PROJETO_VALIDATOR_H
