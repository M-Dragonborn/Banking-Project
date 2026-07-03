#include "../include/flowcash.h"

void run_demo(void) {
    printf("=== RUNNING DEMO SCENARIO ===\n\n");
    
    remove("data/bank_data/index.idx");
    remove("data/bank_data/user.log");
    remove("data/bank_data/admin.log");
    set_system_loan_status(1);
    
    FILE *f = fopen("data/bank_data/system.cfg", "w");
    if(f) {
        fprintf(f, "9999999\n1\n");
        fclose(f);
    }
    
    Account acc1 = {0};
    strcpy(acc1.name, "Musfiqur Rahman");
    strcpy(acc1.pin, "1234");
    acc1.balance = 0.0;
    acc1.loan_count = 0;
    acc1.loan_enabled = 1;
    get_next_account_id(acc1.id);
    write_account(&acc1);
    add_to_index(acc1.id);
    
    Account acc2 = {0};
    strcpy(acc2.name, "GM Entisar Hoque");
    strcpy(acc2.pin, "1234");
    acc2.balance = 0.0;
    acc2.loan_count = 0;
    acc2.loan_enabled = 1;
    get_next_account_id(acc2.id);
    write_account(&acc2);
    add_to_index(acc2.id);
    
    Account acc3 = {0};
    strcpy(acc3.name, "Ratul Saha");
    strcpy(acc3.pin, "1234");
    acc3.balance = 500.0;
    acc3.loan_count = 1;
    acc3.loan_enabled = 1;
    get_next_account_id(acc3.id);
    write_account(&acc3);
    add_to_index(acc3.id);
    
    char loan_file3[100];
    sprintf(loan_file3, "data/bank_data/loans/%s.loan", acc3.id);
    FILE *lf3 = fopen(loan_file3, "w");
    if(lf3) {
        fprintf(lf3, "1000.00 0.050 12 5 0\n");
        fclose(lf3);
    }
    
    Account acc4 = {0};
    strcpy(acc4.name, "Manual Tudu");
    strcpy(acc4.pin, "1234");
    acc4.balance = 200.0;
    acc4.loan_count = 1;
    acc4.loan_enabled = 1;
    get_next_account_id(acc4.id);
    write_account(&acc4);
    add_to_index(acc4.id);

    char loan_file4[100];
    sprintf(loan_file4, "data/bank_data/loans/%s.loan", acc4.id);
    FILE *lf4 = fopen(loan_file4, "w");
    if(lf4) {
        fprintf(lf4, "2000.00 0.050 24 10 1\n");
        fclose(lf4);
    }
    
    printf("1. Created accounts: %s, %s, %s, and %s\n", acc1.name, acc2.name, acc3.name, acc4.name);
    
    acc1.balance += 100.0;
    write_account(&acc1);
    log_transaction(acc1.id, "DEPOSIT", 100.0, "Demo deposit");
    char msg[100];
    sprintf(msg, "User %s: deposited BDT 100.00", acc1.id);
    log_user(msg);
    printf("2. Deposited BDT 100.00 to %s\n", acc1.name);
    
    acc1.balance -= 25.0;
    acc2.balance += 25.0;
    write_account(&acc1);
    write_account(&acc2);
    log_transaction(acc1.id, "TRANSFER", 25.0, "Demo transfer out");
    log_transaction(acc2.id, "TRANSFER", 25.0, "Demo transfer in");
    sprintf(msg, "User %s: transferred BDT 25.00 to %s", acc1.id, acc2.id);
    log_user(msg);
    printf("3. Transferred BDT 25.00 from %s to %s\n", acc1.name, acc2.name);
    
    printf("4. Balances:\n");
    printf("   %s: BDT %.2lf\n", acc1.name, acc1.balance);
    printf("   %s: BDT %.2lf\n", acc2.name, acc2.balance);
    
    printf("\n5. Log Files:\n");
    printf("--- user.log ---\n");
    f = fopen("data/bank_data/user.log", "r");
    if(f) {
        char line[200];
        while(fgets(line, sizeof(line), f)) printf("%s", line);
        fclose(f);
    }
    
    printf("--- admin.log ---\n");
    f = fopen("data/bank_data/admin.log", "r");
    if(f) {
        char line[200];
        while(fgets(line, sizeof(line), f)) printf("%s", line);
        fclose(f);
    }
    
    printf("\n=== DEMO COMPLETED ===\n");
}
