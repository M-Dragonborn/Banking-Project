══════════════════════════════════════════════════ 

## **PROJECT PROPOSAL** 

_On_ 

## **FlowCash -- Bank Management System** 

**Submission date:** 18/06/2026 

## **Submitted by:** 

|**Submitted by:**|**Submitted by:**|**Submitted by:**|
|---|---|---|
|**ID**<br>**NAME**<br>**INTAKE/SECTION**|||
|**20254203121**<br>**Musfiqur Rahman**<br>**49/2**|||
|**20254203097**<br>**G.M Entisar Hoque**<br>**49/2**|||
|**20254203128**<br>**Ratul Saha Anu**<br>**49/2**|||
|**20254203122**<br>**Anower Hossain**<br>**49/2**|||
|**20254203117**|**Manuel Tudu**|**49/2**|



## **Submitted To,** 

Meher Afroj 

Assistant Professor, Dept. of CSE, BUBT 

## **1. Needs / Problems** 

Traditional banking record-keeping using paper ledgers or basic spreadsheets is slow, error-prone, and difficult to scale. As the number of accounts grows, searching, updating, and maintaining transaction history becomes increasingly cumbersome. There is a clear need for a digital system that can handle these operations efficiently. 

- Paper-based and spreadsheet record-keeping cannot handle concurrent operations or provide fast search and retrieval. 

- Full database servers (MySQL, PostgreSQL) are too large and heavy for small-scale banking systems and require dedicated administration. 

- There is no lightweight, self-contained banking demonstration tool that runs in a terminal with zero dependencies. 

## **2. Goals / Objectives** 

The goal of this project is to design and implement a fully functional Bank Management System using the C programming language. The system will operate entirely in a terminal environment and store all data — including user accounts, balances, transaction histories, and loan records — in structured files on the file system, effectively using it as a flat-file database. 

Specific objectives: 

- Implement a complete bank management system in C (C99/C11 standard) with a menu-driven terminal interface. 

- Design a file system-based database structure where each account is stored as a binary file and each transaction is logged in append-only text files. 

- Support role-based access control with two distinct roles: Administrator (full system control) and User (account operations only). 

- Provide full CRUD operations: create, read, update, and delete bank accounts with persistent storage. 

- Implement a transaction system that records every deposit, withdrawal, transfer, and loan operation with timestamps and stores them in log files. 

- Include a loan management subsystem allowing users to take up to 2 loans, with the ability for the admin to enable or disable loans system-wide. 

- Implement input validation on all user inputs and robust error handling for all file operations to prevent crashes and data corruption. 

## **3. Existing System Analysis** 

To understand the current landscape, three categories of existing banking systems were analyzed: 

## **3.2 IOB Banking System (C — Academic Projects)** 

Various C banking projects on GitHub use fprintf/fscanf with text files for account storage. Features include basic account creation, deposit, withdrawal, and balance 

inquiry. 

**Shortcomings** : Unstructured text files prone to corruption. No transaction history or audit trail. No role-based access or authentication. Minimal error handling — crashes on invalid input. No loan management. Monolithic single-file code with no modular design. 

## **3.3 Commercial Banking Software (Temenos, Finacle)** 

Enterprise platforms used by real banks. Handle millions of accounts, real-time transactions, regulatory compliance, and multi-currency operations across distributed data centers. 

**Shortcomings for small-scale use:** Require dedicated servers, DBAs, and months of setup. Proprietary with expensive licensing. Depend on Oracle DB, middleware, and application servers. Written in Java/COBOL — not suitable for a C programming course. 

Our system addresses all of these: written in C, file-based persistence with structured binary and text files, role-based authentication, full transaction logging with timestamps, loan management, input validation, and zero external dependencies. 

## **4. Features / Scopes** 

The following features are planned for implementation. The scope is kept realistic and deliverable within the project timeline. 

## **4.1 Administrator Features** 

- Secure login with password and Logout and return to main menu 

- Create new user account with auto-generated 7-digit account number 

- Delete user account and all associated data files 

- Search for accounts by account number or name 

- List all user accounts with account number, name, balance, and status 

- View total bank balance (sum of all account balances) and outstanding loans 

- Enable or disable the loan feature system-wide 

- View system statistics: total accounts, total deposits, total loans 

## **4.2 User Features** 

- Account login using account number and 4-digit PIN and logout to mainmenu 

- Deposit positive amounts only and withdraw with sufficient-funds validation 

- Withdraw money with sufficient fund validation 

- Check current account balance & view complete transaction history 

- Transfer money to another account with balance and account validation 

- Take a loan maximum of 2 loans per account, only if loan feature is enabled by admin 

- Repay an outstanding loan 

- View current loan status: amount taken, remaining, repayment status 

- Check account status: active/inactive, balance, loan count and Change account PIN 

## **5. File System Based Database** 

Since this project does not use a traditional database server, the file system itself serves as the database. Data is organized in a dedicated directory with a clear structure: 

**Directory structure:** Note that the file system that will be used for the project is still undecided, this is a sample. 

./bank_data/ accounts/       - One .dat file per account (binary struct storage) transactions/   - One .log file per account (append-only transaction log) loans/          - One .loan file per account (loan records) admin.cfg       - Admin password hash and settings system.cfg      - Global bank state (loan toggle, next account number) index.idx       - Account index: maps account numbers to file paths 

Account files store the complete Account struct in binary format for fast read/write. Transaction logs are plain text append-only files that record every operation with a timestamp. The system config file stores global state as key-value pairs. The index file allows O(1) account lookup by account number. 

## **6. Requirements** 

## **6.1 Hardware Requirements** 

- Processor: x86 or x64 compatible (Intel/AMD), 1 GHz or higher 

- RAM: 512 MB minimum (2 GB recommended) 

- Storage: 50 MB free disk space for source code and data files 

- Display: Any terminal-compatible display (80x24 characters minimum) 

- Input: Standard keyboard 

## **6.2 Software Requirements** 

- Operating System: Linux (any distribution), macOS, or Windows with WSL/MinGW 

- Compiler: GCC (GNU Compiler Collection) version 5.0 or higher 

- Text Editor: Any (VS Code, Vim, Nano) 

- Terminal: Any POSIX-compatible terminal emulator 

## **6.3 Personnel Requirements** 

- 5 team members with working knowledge of C programming 

- Required skills: structs, pointers, file I/O (fopen, fread, fwrite, fprintf), dynamic memory (malloc/free) 

- Basic understanding of data structures: arrays, linked lists 

## **7. Development Team** 

**==> picture [444 x 216] intentionally omitted <==**

**----- Start of picture text -----**<br>
ID  Name  Responsibilities<br>20254203121 Musfiqur Rahman Project co-ordination &<br>management<br>20254203097 G.M Entisar Hoque Admin panel<br>20254203128 Ratul Saha Anu User panel<br>20254203122 Anower Hossain Admin panel<br>20254203117 Manuel Tudu Database<br>**----- End of picture text -----**<br>


## **8. Limitations & Future Works** 

## **8.1 Limitations** 

The system has the following limitations: terminal-only interface with no GUI, singlemachine use with no network or concurrent access, no data encryption on stored files, no interest calculation, no multi-currency support, no report generation, limited search by exact account number or name prefix only, no session timeout, no automated backup, and performance degrades with large numbers of accounts due to flat file indexing. 

## **8.2 Future Works** 

Add a graphical interface using ncurses or GTK. Implement network support via TCP/IP sockets. Migrate from flat files to SQLite. Add AES-256 encryption for account files and transaction logs. Implement interest and fee calculation engine. Add multi-currency support with exchange rate storage. Generate formatted reports. Add more granular roles: teller, manager, auditor. Implement automated backup and restore. Add an immutable audit trail for compliance and forensic analysis. 

## **9. References** 

- [1]. C Programming Absolute Beginner's Guide (by Greg Perry and Dean Miller) [2]. Freecodecamp.com 

- [3]. Stackoverflow.com 

_— End of Proposal —_ 

