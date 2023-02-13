#ifndef CODE_DATE_H
#define CODE_DATE_H

int verify_Date(char* token);
int create_Date(char* token);

int calculate_Age(int date);
int compare_Date(int date1, int date2);
int get_MostRecentDate(int date1, int date2);
int date_InBetween(int date, int first_date, int last_date);
int nextDay(int date);

char* date_ToString(int date);




#endif //CODE_DATE_H
