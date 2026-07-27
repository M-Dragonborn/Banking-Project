# FlowCash - Complete Project Dictionary & Presentation Guide

*This guide is the master reference for the entire FlowCash codebase. Every function in every file is mapped to its exact line numbers with a thorough explanation of its design, how it works step by step, and why specific decisions were made. Use this to confidently answer any technical question during the presentation.*

---

## 1. Project Management & Core Architecture (Musfiqur Rahman)
*Focus: Overall architecture, header design, module coordination, data flow, the full compilation and deployment pipeline, the main state loop, and cross-module integration.*

As the Project Coordinator, my role was not simply writing code — it was designing the entire system blueprint so that every team member could work on their isolated module without breaking anyone else's code. This required making foundational decisions about data structures, validation philosophy, file organization, compilation strategy, and deployment before a single line of logic was written.

---

### Architectural Decision: Why Modular C?

The first and most important decision was to reject the temptation to write everything in a single `main.c` file. A single-file approach would have meant every team member was editing the same file simultaneously, creating constant conflicts and making bugs nearly impossible to isolate. Instead, the project was split into six source files, each owned by a specific team member and responsible for exactly one concern:

| File | Owner | Responsibility |
|---|---|---|
| `src/utils.c` | Manuel Tudu | Database I/O, file operations, all shared utilities |
| `src/user.c` | Ratul Saha | Customer-facing menus and transactions |
| `src/admin.c` | G.M Entisar / Anower | Administrator panel and account management |
| `src/loan.c` | G.M Entisar / Anower | Full loan request, approval, and payment lifecycle |
| `src/demo.c` | Musfiqur | Automated integration test harness |
| `src/main.c` | Musfiqur | Program entry point, routing, and initialization |

The binding contract between all six files is `include/flowcash.h` — the master header that every file imports.

---

### `include/flowcash.h` (Lines 1-80) — The Master Header & Shared Contract

This file is the single most critical file in the project. It is not compiled itself — it is `#include`d by every `.c` file, meaning its contents are literally copy-pasted into every source file by the pre-processor before compilation begins. This guarantees that every module is operating on identical definitions.

**The Three Core Structs (Lines 12-34):**
In C, a `struct` is a custom data type that groups multiple variables of different types under one name. I designed three structs that represent the three core data entities of the bank:

- `Account` (Lines 12-19): Holds `id[8]` (7-digit account number + null terminator), `name[50]` (customer name), `pin[10]` (4-digit numeric PIN), `balance` (double-precision floating point for monetary accuracy), `loan_count` (integer 0-2), and `loan_enabled` (integer flag 0 or 1). Every single user action in the system reads or writes an `Account` struct.

- `Transaction` (Lines 21-26): Holds `date[22]` (full `YYYY-MM-DD HH:MM:SS` timestamp), `type[12]` (event category like DEPOSIT or WITHDRAW), `amount` (double), and `note[80]` (plain-text description). This struct defines the shape of every entry in a user's transaction log file.

- `Loan` (Lines 28-34): Holds `principal` (remaining BDT owed), `rate` (annual interest rate as a decimal, e.g., `0.05`), `months` (total repayment period), `paid_months` (how many installments have been made), and `missed` (flag indicating a missed payment). Each line in a `.txt` file maps exactly to this struct's field order.

**The Three Validation Macros (Lines 37-39):**
Pre-processor macros are not functions — they are text substitution rules processed before compilation. This has zero runtime performance cost and guarantees the same validation logic everywhere:

- `POSITIVE(x)` expands to `((x) > 0)` — used to reject zero and negative monetary amounts.
- `PIN_LEN(p)` expands to a chain of `strlen` and `isdigit` checks — ensures a PIN is exactly 4 numeric characters before it is accepted.
- `AMOUNT_RANGE(a)` expands to `((a) >= 0.01 && (a) <= 1000000.0)` — sets the hard floor and ceiling for any transaction in the system, preventing rounding edge-cases and unrealistically large amounts.

**Function Prototypes (Lines 41-79):**
In C, you cannot call a function that the compiler hasn't seen yet. Without prototypes, `user.c` could not call `write_account()` from `utils.c` because `utils.c` is compiled separately as its own translation unit. By declaring all function signatures in this header, it acts as a promise to the compiler: "Trust me, these functions exist — you will find their implementations during the linking stage." This mechanism is what makes the entire multi-file modular architecture possible.

---

### The Compilation Pipeline — How the Build Actually Works

When the program is compiled with:
```sh
gcc -Wall -Wextra -std=c11 -Iinclude -o flowcash src/main.c src/admin.c src/user.c src/loan.c src/utils.c src/demo.c
```

The following happens in three distinct sequential stages:

**Stage 1 — Pre-processing:**
For each `.c` file, the pre-processor runs first. It reads the file and expands all `#include` directives, literally inserting the full text of `flowcash.h` into the source file at that point. It then replaces all macro usages (`POSITIVE`, `PIN_LEN`, `AMOUNT_RANGE`) with their expanded text. The output of this stage is pure, expanded, comment-stripped C code.

**Stage 2 — Compilation:**
Each pre-processed `.c` file is independently compiled into an object file. The compiler checks for type mismatches, calls to undeclared functions, uninitialized variables, and all other errors and warnings. `-Wall` enables all standard warnings. `-Wextra` enables an additional set of warnings beyond the standard set. `-std=c11` tells the compiler to strictly enforce the ISO C11 standard, rejecting non-standard extensions and ensuring the code will compile identically on any C11-compliant compiler. `-Iinclude` specifies the header search path so `#include "flowcash.h"` is reliably located.

**Stage 3 — Linking:**
The linker takes all six object files plus the standard C library (which provides `printf`, `fopen`, `fgets`, `time`, etc.) and combines them into a single executable binary. This is where every function call that referenced a prototype resolves to its actual compiled machine code. If any prototype was declared but never defined, the linker reports an "undefined reference" error here.

The deliberate choice to use `-Wall -Wextra` during development forced the compiler to flag every suspicious pattern in our code. Multiple real bugs — format string mismatches between `fprintf` and the actual struct field types, and potential use of uninitialized variables — were caught and fixed at compile time before ever causing a runtime crash.

---

### `run.sh` & `run.bat` — Deployment & One-Click Execution Pipelines

The project supports two main deployment workflows: **Web Quick-Start** (remote automated download) and **One-Click Local Setup** (offline/cloned repository compilation).

#### 1. Web Quick-Start via `curl ... | bash`
```sh
curl -sL https://raw.githubusercontent.com/M-Dragonborn/Banking-Project/main/run.sh | bash
```

This is two separate programs chained together by a Unix pipe:
- **`curl -sL [URL]`**: Fetches the raw text of `run.sh` from GitHub's CDN. `-s` suppresses progress bars, and `-L` follows any HTTP redirects.
- **`| bash`**: The Unix pipe passes stdout from `curl` directly to `bash` stdin in memory, executing the script immediately without saving it to disk.

#### 2. Smart `run.sh` Step-by-Step Logic
When `run.sh` executes (either via web pipe or locally in Linux/macOS/Git Bash):
1. `#!/usr/bin/env bash` — Shebang line for shell execution portability.
2. `set -e` — Enables "exit on error" mode so the script immediately aborts if any command fails.
3. `mkdir -p data/bank_data/accounts data/bank_data/transactions data/bank_data/loans` — Safely creates required database directories.
4. **Smart Source Detection:**
   `if [ ! -f "src/main.c" ] || [ ! -f "include/flowcash.h" ]; then`
   - If source files are missing locally, it fetches all header and C source files from GitHub via `curl`.
   - If source files already exist locally (e.g. after manually downloading or cloning the repo), it skips network downloads, preserving local edits and working offline.
5. `gcc -Wall -Wextra -std=c11 -Iinclude -o flowcash ...` — Compiles the six C source files into the executable binary.
6. `exec ./flowcash "$@"` — Replaces the shell process with the compiled binary, forwarding any command-line flags (such as `--demo`).

#### 3. `run.bat` — Windows One-Click Setup
For Windows environments, `run.bat` provides a native double-click execution script:
1. `@echo off` & `setlocal enableextensions enabledelayedexpansion` — Prevents command echoing and enables batch expansion features.
2. **Directory Initialization:** Checks and creates missing database folders (`data\bank_data\...`).
3. **Compiler Detection & Fallback:**
   - Uses `where gcc >nul 2>nul` to check if GCC (MinGW-w64 / TDM-GCC) is available in system `%PATH%`.
   - If GCC is available, compiles all local C source files into `flowcash.exe`.
   - If GCC is not found, checks for pre-compiled `flowcash.exe` and falls back to running it.
   - If neither compiler nor binary is available, displays a clear message and pauses.
4. **Execution & Pause:** Executes `flowcash.exe %*`. If an exit error occurs, `pause` prevents the console window from closing instantly when launched via double-click from Windows File Explorer.

---

### `src/main.c` Lines 4-23: `init_directories()` — Self-Healing Bootstrap

This function runs silently every single time the program starts, as the very first thing `main()` does. Its purpose is to guarantee the two critical system configuration files exist with valid content before any other module tries to read them.

For `admin.txt`: It calls `fopen("data/bank_data/admin.txt", "r")`. The `"r"` mode is read-only and never creates a file — if the file does not exist, it returns `NULL` without creating anything. A `NULL` return is the signal to call `set_admin_password_hash("admin")`, which creates the file with the default password string `"admin"`. If the file already exists, `fopen` returns a valid pointer which is immediately closed without reading anything.

For `system.txt`: Same pattern. If missing, the file is created and two values are written: `9999999` (the starting account ID counter, which decrements with each new account) and `1` (the loan-enabled flag). These two integers on two lines are the entire content of `system.txt` — a minimal but sufficient configuration store that every other module reads.

Without this function, any first-run attempt to open these files would receive `NULL` from `fopen`, and functions like `get_next_account_id()`, `get_system_loan_status()`, and `get_admin_password_hash()` would silently fall back to hardcoded defaults or produce undefined behavior depending on whether they check for `NULL`.

---

### `src/main.c` Lines 25-69: `main(int argc, char **argv)` — The Central State Machine

This is the C program's mandatory entry point. The OS calls `main()` when the executable starts. `argc` is the count of command-line arguments (including the program name itself, so `argc` is always at least 1), and `argv` is an array of null-terminated strings, one per argument.

**Argument Parsing (Lines 26-30):**
`if (argc > 1 && strcmp(argv[1], "--demo") == 0)` — The first check `argc > 1` uses short-circuit evaluation: if false, the second condition is never evaluated, preventing an out-of-bounds access on `argv[1]`. `strcmp()` performs a byte-by-byte comparison of two strings and returns exactly `0` on a perfect match. If this condition is true, the entire interactive session is skipped and execution delegates to `run_demo()`.

**The Main Loop (Lines 35-60):**
The `do-while` loop is the heartbeat of the interactive program. Unlike a `while` loop where the condition is checked before the first iteration, `do-while` always executes the body at least once. Each iteration:
1. Calls `clear_screen()` which executes `system("cls")` on Windows or `system("clear")` on POSIX systems, wiping the terminal so the menu always appears clean at the top.
2. Prints the full ASCII art logo and the three menu options.
3. Calls `get_int("Select option: ", &choice)`. If the user types non-numeric input like `"abc"`, the internal `sscanf` call returns `0` (no fields parsed), `get_int` returns `0`, and the loop's `continue` statement skips to the next iteration without crashing or entering an invalid state.
4. Routes the validated integer through a `switch` statement: `case 1` calls `user_menu()`, `case 2` calls `admin_menu()`, `case 0` sets `choice` to 0 and prints a goodbye message.

The critical design point: `user_menu()` and `admin_menu()` are standard function calls. They run, complete their entire sub-session, and return control here. This is fundamentally different from what a naive implementation might do (calling `main()` recursively to "re-display" the menu), which would consume one stack frame per display and eventually crash with a Stack Overflow after enough iterations.

---

### `src/demo.c` Lines 4-120: `run_demo()` — The Automated Integration Test Harness

`run_demo()` was designed as a developer integration test and presentation tool. Rather than requiring a human to manually type inputs to verify the system works, it bypasses all interactive input functions and directly invokes the database and business logic functions programmatically.

It creates four accounts for the team members by directly populating `Account` structs and calling `write_account()` and `add_to_index()`. It then manually writes formatted `.txt` files for two accounts to simulate realistic loan states: one account with an active loan in good standing, and one account where the `missed` flag has been set to `1` to simulate a missed payment. Finally, it executes a direct deposit and a transfer to verify those transaction flows complete without errors, and prints the log file contents to prove the database is reading and writing correctly.

Running `.\flowcash.exe --demo` is the single command that proves the entire system is functional end-to-end and resets the database to a known clean state — invaluable for demonstrations.

---

### Cross-Module Data Flow — The Full Picture

To illustrate how all six files collaborate during a single user action, here is the complete execution trace for depositing money:

1. **`main.c`** — `main()` receives input `1` from the user → calls `user_menu()`.
2. **`user.c`** — `user_menu()` displays the user menu, receives `5` (Deposit) → calls `user_deposit()`.
3. **`user.c`** — `user_deposit()` calls `get_double()` to collect the amount, checks it with `POSITIVE()` and `AMOUNT_RANGE()` macros defined in `flowcash.h`, then adds it to `current_user.balance`.
4. **`user.c`** calls `write_account(&current_user)` — crosses into **`utils.c`**.
5. **`utils.c`** — `write_account()` constructs the file path via `sprintf`, opens it in write mode (`"w"`), writes all 6 struct fields via `fprintf`, and closes the file. Returns `1` (success).
6. **`user.c`** calls `log_transaction(current_user.id, "DEPOSIT", amount, "Cash deposit")` — crosses into **`utils.c`**.
7. **`utils.c`** — `log_transaction()` calls `get_current_date()` to get the live timestamp, opens the user's `.txt` file in append mode, and writes the formatted event record.
8. **`user.c`** calls `log_user(msg)` — crosses into **`utils.c`**.
9. **`utils.c`** — `log_user()` opens `user.txt` in append mode and writes the plain-sentence summary.
10. Control returns up the call stack: `log_user()` returns to `user_deposit()`, `user_deposit()` returns to `user_menu()`, `user_menu()` continues its own loop and redraws the user panel menu.

Every step in this chain crosses module boundaries, and every step works because of the shared definitions in `include/flowcash.h`. This is the proof that the architectural decisions made at the outset of the project — modular design, a shared header, prototype declarations — were the correct ones.

---

## 2. Database Design & Utilities (Manuel Tudu)
*Focus: The flat-file database engine, indexing system, and all shared input/output utilities.*

I designed and built `src/utils.c`, which is the database engine of the entire project. Every other file calls my functions to read, write, search, or delete data. I chose a Flat-File Database approach, meaning each account is stored as a plain text file in a folder rather than inside a heavy database server.

### `src/utils.c` Lines 3-10: `log_user()` and Lines 12-19: `log_admin()`
These two functions provide the audit trail system. Both open their respective log files (`user.txt` or `admin.txt`) using `fopen` with the `"a"` (append) mode. Append mode is critical here — it means the system will add a new line to the end of the existing file rather than deleting and recreating it. Each call writes one plain-text sentence via `fprintf` describing exactly what happened. These logs build up over time and create a permanent, human-readable history of all activity in the bank.

### `src/utils.c` Lines 21-32: `read_account()`
This function is how the program loads a user from the hard drive into active memory. It receives an Account ID string, constructs the file path by using `sprintf` to format it as `"data/bank_data/accounts/XXXXXXX.txt"`, and opens that file with `fopen` in `"r"` (read) mode. If the file does not exist, `fopen` returns `NULL` and the function immediately returns `0` (failure). Otherwise, it uses `fscanf` with a carefully crafted format string to parse each field from the file into the `Account` struct's fields. It returns `1` only if all 6 fields were successfully read, guaranteeing the struct is fully populated.

### `src/utils.c` Lines 34-48: `write_account()`
This is the reverse of reading — it saves the current state of an `Account` struct back to disk. It constructs the same file path and opens it with `fopen` in `"w"` (write) mode. Write mode completely overwrites the existing file, which is exactly what we want — we want the file to always contain the latest data, not to accumulate old versions. It uses `fprintf` to write each struct field as its own line, then closes the file.

### `src/utils.c` Lines 50-61: `delete_account_files()`
When an admin deletes a user, we need to remove three files: their account `.txt`, their transaction `.txt`, and their `.txt` file. This function uses `sprintf` to build each path and then calls the standard C `remove()` function on each one. It also calls `remove_from_index()` to purge the account from the master lookup file.

### `src/utils.c` Lines 63-69: `add_to_index()` and Lines 71-90: `remove_from_index()`
The `index.txt` file is a master phonebook. Each line contains an account ID paired with its file path. `add_to_index()` simply opens the index in append mode and writes a new line for the new account.

`remove_from_index()` is more involved. Since you cannot delete a line from the middle of a text file in C, the function reads every line of the index into a temporary array in memory, skipping the line whose ID matches the one being deleted, and then rewrites the entire `index.txt` file from scratch using only the surviving lines.

### `src/utils.c` Lines 92-100: `log_transaction()`
Every financial event — deposits, withdrawals, transfers, loan payments — gets recorded here. The function opens the user's personal transaction log file in append mode and writes a formatted line that includes the real-time timestamp from `get_current_date()`, the event type, the amount, and a note. This is how the bank maintains an unbroken chronological record of every movement of money for each individual account.

### `src/utils.c` Lines 102-117: `get_next_account_id()`
To prevent two accounts ever sharing the same ID, this function manages a global counter in `system.txt`. It reads the current counter value, converts it to a zero-padded 7-digit string using `sprintf(out_id, "%07d", next_id)`, and then immediately decrements the counter and saves it back to `system.txt`. This means each new account gets a unique, sequentially decreasing ID, starting from `9999999` downwards.

### `src/utils.c` Lines 119-145: `get_system_loan_status()` and `set_system_loan_status()`
`system.txt` stores two integers on separate lines: the next account ID counter, and a `1` or `0` representing whether loans are globally enabled. `get_system_loan_status()` reads both values but only returns the second one. `set_system_loan_status()` reads the existing ID counter first so it doesn't lose it, then rewrites the entire file with the same counter but the new loan status flag.

### `src/utils.c` Lines 147-167: `get_admin_password_hash()` and `set_admin_password_hash()`
These two functions are the only gateway to the admin password stored in `admin.txt`. `get_admin_password_hash()` opens the file, reads the stored password string, and copies it into the provided buffer. If the file doesn't exist, it falls back to the string `"admin"`. `set_admin_password_hash()` simply opens `admin.txt` in write mode and writes the new password string.

### `src/utils.c` Lines 169-221: Safe Input Wrappers
The most critical engineering decision in the utilities file was to completely avoid `scanf` for reading strings. The standard `scanf("%s", buffer)` has no length limit, meaning if a user types 500 characters, it writes 500 characters into a buffer that might only be 10 characters long — a classic buffer overflow that crashes the program.

Instead, `get_string()` (Lines 175-180) uses `fgets(buffer, max_len, stdin)`, which strictly stops reading after `max_len - 1` characters. It then strips the trailing newline character using `strcspn`. `get_double()` and `get_int()` both call `get_string()` first, then use `sscanf` on the already-safely-captured string. `get_current_date()` (Lines 215-221) calls `time(NULL)` to get the current Unix timestamp, converts it to a `struct tm` using `localtime()`, and formats it into the `YYYY-MM-DD HH:MM:SS` string.

---

## 3. User Panel (Ratul Saha Anu)
*Focus: Account creation, authentication, deposits, withdrawals, transfers, and PIN management.*

I built `src/user.c`, the interface where bank customers interact with the system. My code handles everything from creating a new account to making a transfer.

### `src/user.c` Lines 6-47: `create_account()`
This function handles the onboarding process for a new customer. It first uses `get_string()` to collect the customer's full name. Then it collects a 4-digit PIN and validates it with the `PIN_LEN(pin)` macro — this macro checks that the string is exactly 4 characters long and that every character passes `isdigit()`. If the PIN is invalid, the function exits immediately.

Next, it collects the initial deposit and validates it with both `POSITIVE()` and `AMOUNT_RANGE()` to ensure it is a real, valid amount. Once all inputs are valid, it calls `get_next_account_id()` to claim a unique ID, populates a fresh `Account` struct with all the collected data (using `memset` to explicitly initialize the struct to zero first), and saves it to the database with `write_account()`. It also registers the account in the index with `add_to_index()` and logs an initial transaction.

### `src/user.c` Lines 49-74: `login_user()`
This function also serves as the logout mechanism, making use of the `is_logged_in` static variable. If `is_logged_in` is already `1`, calling this function sets it to `0` and prints "Logged out." — this is how the Logout option works from the menu.

For the actual login, it collects an Account ID and PIN, then calls `read_account()` to load the matching account file from disk. If the file is found, it uses `strcmp()` to compare the typed PIN against the PIN stored in the loaded struct. `strcmp()` returns `0` only if the two strings are character-for-character identical — if they match, `is_logged_in` is set to `1` and the user is granted access.

### `src/user.c` Lines 76-92: `user_deposit()`
This function collects a deposit amount using `get_double()` and passes it through `POSITIVE()` and `AMOUNT_RANGE()` before touching the balance. If valid, it simply adds the amount to `current_user.balance` using the `+=` operator. It then calls `write_account()` to persist the updated balance to the hard drive, and `log_transaction()` to record the event in the transaction history file.

### `src/user.c` Lines 94-114: `user_withdraw()`
Nearly identical to deposit, but with a crucial safety gate on Line 102: `if (amount > current_user.balance)`. If the requested withdrawal exceeds the available funds, the function prints an "Insufficient funds" message and returns immediately without modifying anything. Only if the user has enough funds does the code proceed to subtract the amount and call `write_account()`.

### `src/user.c` Lines 116-155: `user_transfer()`
This function combines both operations atomically. It first asks for a recipient Account ID and uses `strcmp()` to prevent a user from transferring money to themselves. It calls `read_account()` to verify the recipient actually exists. Then it collects the amount and checks the sender's balance. If everything passes, it subtracts from `current_user.balance` and adds to `target.balance`. It then calls `write_account()` on **both** accounts, saving both changes. Both accounts' transaction logs are also updated to record the outgoing and incoming transfers respectively.

### `src/user.c` Lines 157-172: `change_pin()`
Collects a new PIN string and validates it with `PIN_LEN()`. If valid, it uses `strcpy()` to overwrite the existing PIN field in `current_user` and saves the account file with `write_account()`.

### `src/user.c` Lines 174-224: `user_menu()`
The main customer interface loop. It runs indefinitely in a `do-while (1)` loop. At the top of every iteration it prints the appropriate menu — a shorter version for logged-out users and the full version for logged-in users. It captures the user's choice with `get_int()` and routes it through a `switch` statement. The only way to exit this loop and return to the main menu is to select option `0`.

---

## 4. Admin Panel & Loan Subsystem (G.M Entisar Hoque & Anower Hossain)
*Focus: Privileged access control, account management tools, system statistics, and the full loan lifecycle.*

We built the admin and loan modules that control the entire bank from a management perspective. We kept the logic highly explicit so that it remains beginner-friendly and readable.

### `src/admin.c` Lines 5-125: `list_and_manage_accounts()`
This is the most powerful administrative tool. It opens `index.txt` and reads all registered account IDs into an in-memory array (up to 1000 accounts). It then loops through that array, calling `read_account()` on each one to print a numbered list of all users.

After the list is shown, it asks the admin to pick a number. The selected number is used as an index into the ID array to load that specific account. A sub-menu then appears with 5 options:
- **View full info**: Prints all of the account's fields, explicitly expanding the `loan_enabled` integer into a `"Yes"` or `"No"` string using an `if-else` block for clarity.
- **Delete account**: Calls `ask_confirm()` first to prevent accidental deletion, then calls `delete_account_files()`.
- **Adjust balance**: Allows adding or subtracting any amount (including negative amounts to simulate a fine or fee) and saves the change.
- **Reset PIN**: Validates the new PIN with `PIN_LEN()` before overwriting and saving.
- **Toggle loan eligibility**: Flips the `loan_enabled` field using the `!` (NOT) operator, so `1` becomes `0` and vice versa, again using explicit `if-else` logic to tell the admin what happened.

### `src/admin.c` Lines 127-147: `search_accounts()`
This function allows searching the entire database by partial name or ID. It reads a query string from the admin, then loops through `index.txt`. For each account, it calls `read_account()` and then uses `strstr(acc.name, query)` and `strstr(acc.id, query)`. The `strstr()` function returns non-NULL if the query string appears anywhere within the target string, which allows partial name matching.

### `src/admin.c` Lines 149-192: `view_statistics()`
This function provides a full financial overview. It opens `index.txt` and loops through every account in the bank. For each account, it adds `acc.balance` to a running `global_balance` total. If that account has active loans, it opens their `.txt` file and loops through each loan entry, adding the `principal` to a running `outstanding_loan_value` total. After the loop, it prints the total account count, the total money currently held across all accounts, the number of active loans, and the total principal still owed to the bank. It explicitly evaluates the global loan system status with an `if-else` block to print whether it's enabled or disabled.

### `src/admin.c` Lines 194-208: `change_admin_password()`
Prompts for a new password string, validates that it is not empty using `strlen()`, and if the admin confirms, calls `set_admin_password_hash()` to overwrite the `admin.txt` file with the new password. The action is recorded in `admin.txt`.

### `src/admin.c` Lines 210-279: `admin_menu()`
The admin menu loop manages authentication entirely within itself using the `admin_logged_in` static integer flag. When it is `0`, only a Login option is shown. When the admin logs in successfully, `admin_logged_in` is set to `1` and the full menu with all 7 options becomes visible. The password check uses `strcmp()` to compare the input against the stored password.

---

### `src/loan.c` Lines 3-56: `request_loan()`
When a user wants to borrow money, this function is called. It first runs a series of eligibility gates:
1. Checks if the global loan feature is enabled via `get_system_loan_status()`.
2. Checks if the user's personal `loan_enabled` flag is `1`.
3. Checks if `acc->loan_count` is already at the limit of `2`.
4. Opens the user's `.txt` file and checks if any existing loan has a `missed` flag set to `1`. If a user has missed a payment, they are blocked from requesting another loan.

If the user passes all gates, the function collects the desired amount and duration in months, hardcodes a `5%` interest rate, and appends a pipe-delimited record (`id|amount|rate|months`) to the centralized `pending_loans.txt` file for the admin to review.

### `src/loan.c` Lines 58-157: `pay_loan()`
This is the most mathematically involved function in the project. It opens the user's `.txt` file and reads all active loan entries into a `Loan loans[2]` array. We intentionally wrote this loop as an explicit `while(1)` block with discrete `break` conditions to make the file-reading logic easy for beginners to understand. For each loan, it calculates the monthly installment dynamically:
- `monthly_interest = (principal x rate) / 12` — The interest cost for this month.
- `monthly_principal = principal / (total_months - paid_months)` — The principal repayment portion for this month.
- `installment = monthly_principal + monthly_interest` — The total amount due.

After showing the loans and installment amounts, the user selects which one to pay. The function checks the account balance, and if sufficient, subtracts the installment, reduces the remaining principal, increments `paid_months`, and clears the `missed` flag. It then rewrites the entire `.txt` file, and if a loan is fully paid off, that loan entry is simply not written back, effectively deleting it.

### `src/loan.c` Lines 159-183: `view_loan_status()`
A read-only display function. It opens the user's `.txt` file and loops through each entry, printing the loan number, remaining principal, interest rate, and months paid versus total months. It also displays whether the loan status is `OK` or `MISSED PAYMENT`, checking the flag via an explicit `if-else` statement.

### `src/loan.c` Lines 185-251: `approve_deny_loans()`
The admin's loan review tool. It reads all pending requests from `pending_loans.txt` into a `lines[100][150]` array, then immediately reopens the file in write mode. For each pending request, it parses the pipe-delimited fields using `sscanf`, displays the request to the admin, and asks for a `y`, `n`, or `s` (skip) response.

- If `y`: It creates or appends to the user's `.txt` file with the new loan entry, reads the user's account, adds the loan amount directly to their balance, increments `loan_count`, and saves everything.
- If `n`: The request is simply not written anywhere — it is permanently rejected.
- If `s`: The original line is written back to the file, keeping it pending for later review.
