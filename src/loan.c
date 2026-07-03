#include "../include/flowcash.h"

void request_loan(const Account *acc) {
    if (!acc->loan_enabled || !get_system_loan_status()) {
        printf("Loans are currently disabled for this account or system-wide.\n");
        return;
    }
    
    if (acc->loan_count >= 2) {
        printf("You have reached the maximum number of loans (2).\n");
        return;
    }
    
    // Check if missed payments exist
    char loan_file[100];
    sprintf(loan_file, "data/bank_data/loans/%s.loan", acc->id);
    FILE *f = fopen(loan_file, "r");
    if (f) {
        Loan l;
        while (fscanf(f, "%lf %lf %d %d %d", &l.principal, &l.rate, &l.months, &l.paid_months, &l.missed) == 5) {
            if (l.missed) {
                printf("You have missed payments on an existing loan. Cannot request new loan.\n");
                fclose(f);
                return;
            }
        }
        fclose(f);
    }
    
    double amount, rate;
    int months;
    
    if (!get_double("Enter loan amount: BDT ", &amount) || !POSITIVE(amount) || !AMOUNT_RANGE(amount)) {
        printf("Invalid amount.\n");
        return;
    }
    
    rate = 0.05; // Fixed 5% for simplicity
    
    if (!get_int("Enter repayment duration in months: ", &months) || !POSITIVE(months)) {
        printf("Invalid months.\n");
        return;
    }
    
    // Add to pending loans
    FILE *pf = fopen("data/bank_data/pending_loans.txt", "a");
    if (pf) {
        fprintf(pf, "%s|%.2lf|%.3lf|%d\n", acc->id, amount, rate, months);
        fclose(pf);
        printf("Loan request submitted for admin approval.\n");
        char msg[100];
        sprintf(msg, "User %s: requested loan of BDT %.2lf", acc->id, amount);
        log_user(msg);
    } else {
        printf("Failed to submit loan request.\n");
    }
}

void pay_loan(Account *acc) {
    char loan_file[100];
    sprintf(loan_file, "data/bank_data/loans/%s.loan", acc->id);
    FILE *f = fopen(loan_file, "r");
    if (!f) {
        printf("No active loans found.\n");
        return;
    }
    
    Loan loans[2];
    int count = 0;
    while (count < 2 && fscanf(f, "%lf %lf %d %d %d", &loans[count].principal, &loans[count].rate, &loans[count].months, &loans[count].paid_months, &loans[count].missed) == 5) {
        count++;
    }
    fclose(f);
    
    if (count == 0) {
        printf("No active loans to pay.\n");
        return;
    }
    
    printf("--- Active Loans ---\n");
    for (int i = 0; i < count; i++) {
        double monthly_interest = (loans[i].principal * loans[i].rate) / 12.0;
        double monthly_principal = loans[i].principal / (loans[i].months - loans[i].paid_months);
        double installment = monthly_principal + monthly_interest;
        
        printf("%d) Remaining Principal: BDT %.2lf, Paid: %d/%d months. Next Installment: BDT %.2lf %s\n", 
               i+1, loans[i].principal, loans[i].paid_months, loans[i].months, installment, loans[i].missed ? "(MISSED)" : "");
    }
    
    int choice;
    if (!get_int("Select loan to pay (or 0 to cancel, 9 to simulate a missed month): ", &choice)) return;
    
    if (choice == 9) {
        // Simulate a missed month for demo purposes
        int l_idx;
        if(get_int("Which loan number to miss? ", &l_idx) && l_idx >= 1 && l_idx <= count) {
            loans[l_idx-1].missed = 1;
            printf("Simulated missed payment for loan %d.\n", l_idx);
        }
    } else if (choice >= 1 && choice <= count) {
        int idx = choice - 1;
        double monthly_interest = (loans[idx].principal * loans[idx].rate) / 12.0;
        double monthly_principal = loans[idx].principal / (loans[idx].months - loans[idx].paid_months);
        double installment = monthly_principal + monthly_interest;
        
        if (acc->balance >= installment) {
            acc->balance -= installment;
            loans[idx].principal -= monthly_principal;
            loans[idx].paid_months++;
            loans[idx].missed = 0; // Cleared missed status
            
            write_account(acc);
            log_transaction(acc->id, "PAYMENT", installment, "Loan installment paid");
            
            char msg[100];
            sprintf(msg, "User %s: paid loan installment of BDT %.2lf", acc->id, installment);
            log_user(msg);
            printf("Payment successful.\n");
        } else {
            printf("Insufficient account balance for this installment.\n");
        }
    }
    
    // Save loans back
    f = fopen(loan_file, "w");
    if (f) {
        int remaining = 0;
        for (int i = 0; i < count; i++) {
            if (loans[i].paid_months < loans[i].months && loans[i].principal > 0.01) {
                fprintf(f, "%.2lf %.3lf %d %d %d\n", loans[i].principal, loans[i].rate, loans[i].months, loans[i].paid_months, loans[i].missed);
                remaining++;
            }
        }
        fclose(f);
        
        // Update account loan count if a loan was paid off
        if (remaining < acc->loan_count) {
            acc->loan_count = remaining;
            write_account(acc);
            if (remaining == 0) remove(loan_file);
        }
    }
}

void view_loan_status(const Account *acc) {
    char loan_file[100];
    sprintf(loan_file, "data/bank_data/loans/%s.loan", acc->id);
    FILE *f = fopen(loan_file, "r");
    if (!f) {
        printf("No active loans.\n");
        return;
    }
    
    Loan l;
    int count = 1;
    printf("\n--- Loan Status ---\n");
    while (fscanf(f, "%lf %lf %d %d %d", &l.principal, &l.rate, &l.months, &l.paid_months, &l.missed) == 5) {
        printf("Loan %d: Principal BDT %.2lf, Rate %.1f%%, Paid %d/%d months. Status: %s\n",
               count++, l.principal, l.rate * 100.0, l.paid_months, l.months, l.missed ? "MISSED PAYMENT" : "OK");
    }
    fclose(f);
}

void approve_deny_loans(void) {
    FILE *f = fopen("data/bank_data/pending_loans.txt", "r");
    if (!f) {
        printf("No pending loan requests.\n");
        return;
    }
    
    char lines[100][150];
    int count = 0;
    while (fgets(lines[count], sizeof(lines[0]), f)) {
        lines[count][strcspn(lines[count], "\n")] = 0;
        count++;
    }
    fclose(f);
    
    if (count == 0) {
        printf("No pending loan requests.\n");
        return;
    }
    
    FILE *f_new = fopen("data/bank_data/pending_loans.txt", "w"); // rewrite unresolved
    for (int i = 0; i < count; i++) {
        char id[10];
        double amount, rate;
        int months;
        sscanf(lines[i], "%[^|]|%lf|%lf|%d", id, &amount, &rate, &months);
        
        printf("\nPending request from Account %s for BDT %.2lf (%d months).\n", id, amount, months);
        char choice[10];
        if (get_string("Approve? (y/n/s to skip): ", choice, sizeof(choice))) {
            if (tolower(choice[0]) == 'y') {
                char loan_file[100];
                sprintf(loan_file, "data/bank_data/loans/%s.loan", id);
                FILE *lf = fopen(loan_file, "a");
                if (lf) {
                    fprintf(lf, "%.2lf %.3lf %d 0 0\n", amount, rate, months);
                    fclose(lf);
                }
                
                Account acc;
                if (read_account(id, &acc)) {
                    acc.loan_count++;
                    acc.balance += amount; // funds deposited
                    write_account(&acc);
                    log_transaction(id, "LOAN", amount, "Loan approved and deposited");
                }
                
                char msg[100];
                sprintf(msg, "Admin: approved loan for %s", id);
                log_admin(msg);
                printf("Loan approved.\n");
                
            } else if (tolower(choice[0]) == 'n') {
                char msg[100];
                sprintf(msg, "Admin: denied loan for %s", id);
                log_admin(msg);
                printf("Loan denied.\n");
            } else {
                fprintf(f_new, "%s\n", lines[i]);
                printf("Skipped.\n");
            }
        }
    }
    if (f_new) fclose(f_new);
}
