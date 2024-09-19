#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "racun.h"

extern const char* accountFilePath = "racuni.txt"; //relativna putanja
extern const char* tempAccountFilePath = "temp.txt";
static int brojKreiranihRacuna = 0; 

void generate_iban(char* iban) {
    srand(time(NULL));
    for (int i = 0; i < 10; i++) {
        iban[i] = '0' + rand() % 10;
    }
    iban[10] = '\0';
}

void create_account() {
    BankAccount account;
    char iban[11];

    printf("Unesite korisnicko ime (do 49 znakova, bez razmaka): ");
    if (scanf("%49s", account.username) != 1) {
        printf("Greska pri unosu korisnickog imena.\n");
        return;
    }

    printf("Unesite lozinku (do 49 znakova, bez razmaka): ");
    if (scanf("%49s", account.password) != 1) {
        printf("Greska pri unosu lozinke.\n");
        return;
    }

    printf("Unesite pocetni iznos na racunu: ");
    if (scanf("%lf", &account.balance) != 1) {
        printf("Greska pri unosu pocetnog iznosa.\n");
        while (getchar() != '\n');
        return;
    }

    generate_iban(iban);
    strcpy(account.iban, iban);

    save_account_info(account);
    printf("Racun je uspjesno kreiran. Vas IBAN: %s\n", iban);
}

void login() {
    char username[MAKS_ZNAKOVI];
    char password[MAKS_ZNAKOVI];

    printf("\nUnesite korisnicko ime: ");
    if (scanf("%49s", username) != 1) {
        printf("Greska pri unosu korisnickog imena.\n");
        return;
    }

    printf("Unesite lozinku: ");
    if (scanf("%49s", password) != 1) {
        printf("Greska pri unosu lozinke.\n");
        return;
    }

    FILE* file = fopen(accountFilePath, "r");
    if (file == NULL) {
        perror("Greska pri otvaranju datoteke");
        return;
    }

    BankAccount account;
    int found = 0;

    // provjera newlines u stringu kako nebi doslo do greske tijekom usporedbe
    while (fscanf(file, "%49s %49s %10s %lf", account.username, account.password, account.iban, &account.balance) == 4) {
        if (strcmp(account.username, username) == 0 && strcmp(account.password, password) == 0) {
            found = 1;
            printf("Uspjesno ste se prijavili. \nStanje racuna: %.2lf\n", account.balance);
            fclose(file);
            user_menu(&account);
            return;
        }
    }

    if (!found) {
        printf("\nKorisnicko ime ili lozinka nisu ispravni.\n");
    }

    fclose(file);
}

void user_menu(BankAccount* account) {
    int odabir;
    do {
        printf("\n---------------------------------");
        printf("\nOdaberite opciju:\n");
        printf("1. Prijenos novca\n");
        printf("2. Odjava\n");
        printf("Vas izbor: ");
        scanf("%d", &odabir);

        switch (odabir) {
        case TRANSFER_OPTION:
            transfer_funds(account);
            break;
        case SIGN_OUT_OPTION:
            printf("Uspjesno ste se odjavili.\n");
            return;
        default:
            printf("\nNepostojeca opcija. Molimo unesite broj od 1 do 2 na izborniku.\n");
            while (getchar() != '\n');
            break;
        }
    } while (1);
}

void transfer_funds(BankAccount* sender) {
    char recipientIban[11];
    double amount;
    int senderUpdated = 0, recipientUpdated = 0;

    printf("Unesite IBAN primatelja: ");
    if (scanf("%10s", recipientIban) != 1) {
        printf("Greska pri unosu IBAN-a.\n");
        return;
    }

    printf("Unesite iznos za prijenos: ");
    do {
        if (scanf("%lf", &amount) != 1) {
            printf("Greska pri unosu iznosa.\n");
            return;
        }
        if (amount <= 0 || amount > sender->balance) {
            printf("Iznos mora biti veci od 0 i manji ili jednak vasem trenutnom stanju (%.2lf). Pokusajte ponovo: ", sender->balance);
        }
    } while (amount <= 0 || amount > sender->balance);

    //stvaranje privremenog fajla za promjenu informacija korisnickih racuna
    FILE* file = fopen(accountFilePath, "r");
    FILE* tempFile = fopen(tempAccountFilePath, "w");

    if (file == NULL || tempFile == NULL) {
        perror("Greska pri otvaranju datoteke");
        if (file) fclose(file);
        if (tempFile) fclose(tempFile);
        return;
    }

    BankAccount account;
    while (fscanf(file, "%49s %49s %10s %lf", account.username, account.password, account.iban, &account.balance) == 4) {
        if (strcmp(account.iban, recipientIban) == 0) {
            // updejtanje stanja racuna primatelja
            account.balance += amount;
            recipientUpdated = 1;
        }
        if (strcmp(account.username, sender->username) == 0 && strcmp(account.iban, sender->iban) == 0) {
            // updejtanje stanja racuna posiljatelja
            account.balance -= amount;
            sender->balance = account.balance; 
            senderUpdated = 1;
        }
        fprintf(tempFile, "%s %s %s %.2lf\n", account.username, account.password, account.iban, account.balance);
    }

    fclose(file);
    fclose(tempFile);

    // zamjena fajla sa starim podacima
    remove(accountFilePath);
    rename(tempAccountFilePath, accountFilePath);

    if (recipientUpdated && senderUpdated) {
        printf("Prijenos uspjesan. Novo stanje: %.2lf\n", sender->balance);
    }
    else {
        printf("Primatelj s IBAN-om %s nije pronaden ili greska sa racunom posiljaoca.\n", recipientIban);
    }
}
void delete_account() {
    char username[MAKS_ZNAKOVI];
    char password[MAKS_ZNAKOVI];

    printf("\nUnesite korisnicko ime: ");
    if (scanf("%49s", username) != 1) {
        printf("Greska pri unosu korisnickog imena.\n");
        return;
    }

    printf("Unesite lozinku: ");
    if (scanf("%49s", password) != 1) {
        printf("Greska pri unosu lozinke.\n");
        return;
    }

    FILE* file;
    OPEN_FILE(file, accountFilePath, "r");

    FILE* tempFile = fopen(tempAccountFilePath, "w");
    if (tempFile == NULL) {
        perror("Greska pri kreiranju privremene datoteke");
        fclose(file);
        return;
    }

    BankAccount* account = (BankAccount*)malloc(sizeof(BankAccount));
    if (account == NULL) {
        perror("Greska pri alokaciji memorije");
        fclose(file);
        fclose(tempFile);
        return;
    }

    int found = 0;
    while (fscanf(file, "%49s %49s %49s %lf", account->username, account->password, account->iban, &account->balance) == 4) {
        if (strcmp(account->username, username) == 0 && strcmp(account->password, password) == 0) {
            found = 1;
            printf("\nRacun je uspjesno izbrisan.\n");
        }
        else {
            fprintf(tempFile, "%s %s %s %.2lf\n", account->username, account->password, account->iban, account->balance);
        } //salje sve ostale racune u temp file
    }

    if (!found) {
        printf("\nKorisnicko ime ili lozinka nisu ispravni.\n");
    }

    free(account);
    fclose(file);
    fclose(tempFile);

    remove(accountFilePath);
    rename(tempAccountFilePath, accountFilePath);
}

void save_account_info(BankAccount account) {
    FILE* file = fopen(accountFilePath, "a");
    if (file == NULL) {
        perror("Pogreska pri otvaranju datoteke");
        return;
    }

    fprintf(file, "%s %s %s %.2lf\n", account.username, account.password, account.iban, account.balance);
    fclose(file);
}

void sort_accounts() {
    FILE* file = fopen(accountFilePath, "r");
    if (file == NULL) {
        perror("Greska pri otvaranju datoteke za citanje");
        return;
    }

    BankAccount* accounts = NULL;
    int count = 0;

    // citanje svih racuna na fajlu
    while (!feof(file)) {
        accounts = realloc(accounts, (count + 1) * sizeof(BankAccount));
        if (fscanf(file, "%49s %49s %10s %lf", accounts[count].username, accounts[count].password, accounts[count].iban, &accounts[count].balance) == 4) {
            count++;
        }
    }
    fclose(file);

    if (count == 0) {
        printf("Nema racuna za sortiranje.\n");
        free(accounts);
        return;
    }

    qsort(accounts, count, sizeof(BankAccount), compare_accounts);

    file = fopen(accountFilePath, "w");
    if (file == NULL) {
        perror("Greska pri otvaranju datoteke za pisanje");
        free(accounts);
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "%s %s %s %.2lf\n", accounts[i].username, accounts[i].password, accounts[i].iban, accounts[i].balance);
    }

    fclose(file);
    free(accounts);
    printf("Racuni su uspjesno sortirani.\n");
}

void search_accounts() {
    char username[MAKS_ZNAKOVI];
    printf("\nUnesite korisnicko ime za pretragu: ");
    scanf("%49s", username);

    BankAccount key = { .username = "" }; //kreira strukturu za pregled
    strcpy(key.username, username); //kopira uneseno ime

    FILE* file = fopen(accountFilePath, "r");
    if (file == NULL) {
        perror("Greska pri otvaranju datoteke");
        return;
    }

    BankAccount* accounts = NULL; //pointer na racune
    int count = 0;

    while (!feof(file)) {
        accounts = realloc(accounts, (count + 1) * sizeof(BankAccount));
        fscanf(file, "%49s %49s %49s %lf", accounts[count].username, accounts[count].password, accounts[count].iban, &accounts[count].balance);
        count++;
    }

    fclose(file);

    //pretraga trazenog korisnika
    BankAccount* result = bsearch(&key, accounts, count, sizeof(BankAccount), compare_accounts);
    if (result != NULL) {
        printf("\nRacun pronaden: %s, Stanje: %.2lf\n", result->username, result->balance);
    }
    else {
        printf("\nRacun nije pronaden.\n");
    }

    free(accounts);
}

int compare_accounts(const void* a, const void* b) {
    return strcmp(((BankAccount*)a)->username, ((BankAccount*)b)->username);
}
