#ifndef FLOWCASH_H
#define FLOWCASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

/* Data Models */
typedef struct {
    char   id[8];          // 7-digit account number (string)
    char   name[50];
    char   pin[10];        // 4-digit PIN
    double balance;
    int    loan_count;     // 0-2
    int    loan_enabled;   // 0 = disabled, 1 = enabled (global toggle)
} Account;

typedef struct {
    char   date[12];       // DD/MM/YYYY (plain text, no timestamp needed)
    char   type[12];       // DEPOSIT, WITHDRAW, TRANSFER, LOAN, PAYMENT
    double amount;
    char   note[80];
} Transaction;

typedef struct {
    double principal;
    double rate;          // annual interest rate (e.g. 0.05 for 5%)
    int    months;        // total months for repayment
    int    paid_months;   // months already paid
    int    missed;        // 0 = ok, 1 = missed payment
} Loan;

/* Validation Macros */
#define POSITIVE(x)   ((x) > 0)
#define PIN_LEN(p)    (strlen(p) == 4 && isdigit(p[0]) && isdigit(p[1]) && isdigit(p[2]) && isdigit(p[3]))
#define AMOUNT_RANGE(a) ((a) >= 0.01 && (a) <= 1000000.0)

/* Function Prototypes */
/* utils.c */
void log_user(const char *msg);
void log_admin(const char *msg);
int get_next_account_id(char *out_id);
int get_system_loan_status(void);
void set_system_loan_status(int status);
int read_account(const char *id, Account *acc);
int write_account(const Account *acc);
int delete_account_files(const char *id);
void add_to_index(const char *id);
void remove_from_index(const char *id);
void log_transaction(const char *id, const char *type, double amount, const char *note);
void clear_input_buffer(void);
int get_string(const char *prompt, char *buffer, int max_len);
int get_double(const char *prompt, double *val);
int get_int(const char *prompt, int *val);
int ask_confirm(const char *prompt);
char* get_current_date(void);
int get_admin_password_hash(char *hash_out);
void set_admin_password_hash(const char *hash);

/* admin.c */
void admin_menu(void);

/* user.c */
void user_menu(void);

/* loan.c */
void request_loan(const Account *acc);
void pay_loan(Account *acc);
void view_loan_status(const Account *acc);
void approve_deny_loans(void);

/* demo.c */
void run_demo(void);

#endif // FLOWCASH_H
