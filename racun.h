#ifndef BANK_ACCOUNT_H
#define BANK_ACCOUNT_H
#define MAKS_ZNAKOVI 50

typedef struct {  
    char username[MAKS_ZNAKOVI];
    char password[MAKS_ZNAKOVI];
    char iban[11];
    double balance;
} BankAccount;

typedef enum {
    CREATE_ACCOUNT_OPTION = 1,
    LOGIN_OPTION,
    DELETE_ACCOUNT_OPTION,
    SORT_OPTION,
    SEARCH_OPTION,
    EXIT_OPTION
} MenuOption;

typedef enum {
    TRANSFER_OPTION = 1,
    SIGN_OUT_OPTION
} UserMenuOption;

void create_account();
void login();
void delete_account();
void save_account_info(BankAccount account);
void sort_accounts();
void search_accounts();
int compare_accounts(const void* a, const void* b);
void transfer_funds(BankAccount* sender);
void user_menu(BankAccount* account);
void generate_iban(char* iban);

#define OPEN_FILE(file, path, mode) \
    file = fopen(path, mode); \
    if (file == NULL) { \
        perror("Greska pri otvaranju datoteke"); \
        return; \
    }

#endif // BANK_ACCOUNT_H
