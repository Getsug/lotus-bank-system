/*
*Bank System that runs in linux terminal
*created by Herbert Okuthe
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <termios.h>
#include <fcntl.h>


#define SIZE 20
#define TRUE 1
#define FALSE 0

typedef struct Admin
{
    char name[SIZE];
    int id;
    char password[BUFSIZ] ;
    char phoneNumber[SIZE];
}Admin;

typedef struct Customer
{
    char name[SIZE];
    int accountNumber;
    int balance;
    char password[BUFSIZ];
    char phoneNumber[SIZE];

}Customer;


void logoDisplay();
void welcomePage();
int chooseUserType();
int adminLogin(int id, char *passwd);
int customerLogin(int accountNum, char *passwd);
int adminAcitvity();
int customerAcitvity(int userAccountNum);
void getAdminIDnPasswd(int *id, char *passwd);
void getUserAccNumAndPasswd(int *accNum, char *passwd);
void turnoffCanonMode();
void turnoffEcho();
void getCurrSettOrRestoreSett(int trigger);
void kboardAsyncSig();
void breakLoop(int signum);
void displayMessage(int signum);
int timer(int millisec);
void delay();
void customWait();
void encrypt(char *password);
void decrypt(char *password);

/*GLOBAL VARIABLES*/
Admin superAdmin = {"Admin001", 001, "1", "01099990000"}; // to login into the Systme for the first time
int g_keyNotPressed = TRUE;         // equals 1 when no key has been presed but zero when any key is pressed


int main(void)
{

    system("clear");    //clear everything on terminal

    /*Displays the bank's logo on the terminal*/
    logoDisplay();

    /*prints an animated welcome message*/
    welcomePage();


    int loggedIn = FALSE;

    /*keep prompting user to selct user type and login*/
    do
    {
        /* 1.admin 2.Customer*/
        int user = chooseUserType();

        if(user == 1)
        {
            int adminID;
            char adminPasswd[SIZE];

            /*initializes adminID and passwd*/
            getAdminIDnPasswd(&adminID, adminPasswd);


            loggedIn = adminLogin(adminID, adminPasswd);

            if(loggedIn)
            {
                int stopActivity  = FALSE;

                /*keep prompting user to select an activity*/
                do
                {
                    stopActivity = adminAcitvity();

                } while(stopActivity  == FALSE);

            }


            //printf("Id: %d\n", adminID);
            //printf("passwd: %s\n", adminPasswd);

            //printf("Welcome Admin\n");
            //adminMenu();
        }
        else if(user == 2)
        {
            int userAccountNum;
            char userPasswd[SIZE];

            /*initializes userAccountNum and userPasswd*/
            getUserAccNumAndPasswd(&userAccountNum, userPasswd);

            loggedIn = customerLogin(userAccountNum, userPasswd);

            if(loggedIn)
            {
                int stopActivity  = FALSE;

                /*keep prompting user to select an activity*/
                do
                {
                    stopActivity = customerAcitvity(userAccountNum);

                } while(stopActivity  == FALSE);

            }

        }
        else
        {
            printf("\n\t\t\t\tInvalid input!\n\n");
        }

        delay();                         //delay for 2 sec

        system("clear");


    } while(loggedIn == FALSE);



}

//functions

/*
*creates a child process to execute figlet cmd and display the bank name
as an ASCII text banner.
*/
void logoDisplay()
{
    printf("\n\n");

    int pid = fork();

    if(pid < 0)
    {
        perror("Creation of a new process is unsuccessful!\n");
        exit(1);
    }

    if(pid == 0)
    {
        //char *argv[] = {"figlet", "-c", "MAZE BANK", NULL};
        //execvp(argv[0], argv);
        execlp("figlet", "figlet", "-c", "-k", "-t","MAZE BANK", NULL);
    }

    wait(NULL);


    char flower[8][50] = {	"	                           .-. .-.			",
                            "	                          (   |   )			",
                            "	                        .-.:  |  ;,-.		",
                            "	                       (_ __`.|.'__ _)		",
                            "	                       (    .'|`.    )		",
                            "	                        `-'/  |  \\`-'		",
                            "	                          (   !   )			",
                            "	                           `-' `-'			"
                        };



    printf("\n\n");                         //create  distance from figlet text banner

    for(int i = 0; i < 8; i++)
    {
        printf("%s\n", flower[i]);
    }

    printf("\n\n\n");

    delay();

    system("clear");
}


//asynchronous signal handler
void breakLoop(int signum)
{
    g_keyNotPressed = FALSE;         // a key has been pressed
}

/*only prints the message when no key has been pressed*/
void displayMessage(int signum)
{
    if(g_keyNotPressed)
    {
        printf("\n\t\t\t-----press enter to continue-----\n");
    }
}


/*causes a delay until the user presses a key*/
void delay()
{
    g_keyNotPressed =  TRUE;             // initalize global varibale

    getCurrSettOrRestoreSett(0);         // get terminal settings

    turnoffEcho();
    turnoffCanonMode();                 //  no buffering

    signal(SIGALRM, displayMessage);    //  displays messsage when it receives signal from timer
    signal(SIGIO, breakLoop);           //  install asynchronous signal handler
    kboardAsyncSig();                   //  send signal whenever a character is typed

    timer(2000);                        //  timer set to 2sec (2000ms)

    /*an infinite loop that only breaks when a key is pressed*/
    while(g_keyNotPressed)
    {

    }

    /*
    *pressing any key on the keyboard sends a signal and the handler is called
    *Calling the signal handler inturrupts sleep and the delay caused by sleep is stopped
    */
    //sleep(20);

    getCurrSettOrRestoreSett(1);         //restore terminal settings

}


/*waits until user is done viewing page*/
void customWait()
{

    char key;
    int backToPrevPage = FALSE;

    printf("\t\t\t----press q when done----\n");

    getCurrSettOrRestoreSett(0);
    turnoffEcho();
    turnoffCanonMode();

    do
    {
        key = getchar();

        if(key == 'q' || key == 'Q')               //13 is return key(enter)
        {
            backToPrevPage = TRUE;
        }

    } while(backToPrevPage == FALSE);

    getCurrSettOrRestoreSett(1);
}


void welcomePage()
{
    char line1[] = "WELCOME";
    char line2[] = "TO";
    char line3[] = "MAZE BANK";


    int len1 = strlen(line1), len2 = strlen(line2), len3 = strlen(line3);

    initscr();
    echo();

    int row = 8, col = 35;

    for(int i = 0; i < len1; i++)
    {
        mvaddch(row, col, line1[i]);    // display character
        refresh();

        usleep(80264);                  //delays display of the next character

        col++;
    }

    row = 10;
    col = 38;

    for(int j = 0; j < len2; j++)
    {
        mvaddch(row, col, line2[j]);    // display character
        refresh();

        usleep(80264);

        col++;
    }

    row = 12;
    col = 34;

    for(int i = 0; i < len3; i++)
    {
        mvaddch(row, col, line3[i]);    // display character
        refresh();

        usleep(80264);

        col++;
    }

    sleep(2);

    endwin();

    system("clear");                    //clears the terminal

}


int chooseUserType()
{
    int user;
    char buf[BUFSIZ];

    /*
    *Clears stdin buffer by reading all values from file desciptor zero
    *note: This enables the scanf below to read values typed when this page is displayed
    */
    read(0, buf, BUFSIZ);


    //printf("%s  %d\n", buf, len);

    printf("\n\n\n \t\t  ****************************************\n\n");
    printf("\t\t\t  Please Select the User Type\n\n");
    printf("\t\t\t\t  1.Admin\n");
    printf("\t\t\t\t  2.Customer");
    printf("\n\n \t\t  ****************************************\n\n\n");
    printf("\t\t\t\t  User: ");
    scanf(" %d", &user);

    return user;
}

void getAdminIDnPasswd(int *id, char *passwd)
{
    printf("\n\n\t\t\t\tEnter userID: ");
    scanf(" %d", id);

    getCurrSettOrRestoreSett(0);
    turnoffEcho();

    printf("\t\t\t\tEnter Password:");
    scanf(" %s", passwd);

    getCurrSettOrRestoreSett(1);
}


void getUserAccNumAndPasswd(int *accNum, char *passwd)
{
    printf("\n\n\t\t\t\tEnter account number: ");
    scanf(" %d", accNum);

    getCurrSettOrRestoreSett(0);
    turnoffEcho();

    printf("\t\t\t\tEnter password:");
    scanf(" %s", passwd);

    getCurrSettOrRestoreSett(1);
}

//add admin logic
int adminLogin(int id, char *passwd)
{
    FILE *file = fopen("adminData.csv", "r");

    /*if file doesn't exist then create one, add superAdmin data then read it*/
    if(file == NULL)
    {
        file = fopen("adminData.csv", "w");

        //label
        fprintf(file, "%s, %s, %s, %s\n", "ID", "NAME", "PHONE NUMBER", "PASSWORD");

        //hash password before storage
        encrypt(superAdmin.password);

        fprintf(file, "%d,%s,%s,%s\n", superAdmin.id, superAdmin.name, superAdmin.phoneNumber, superAdmin.password);
        // log file printf("didn't find file but created one\n");
        fclose(file);                                 // close write mode

        file = fopen("adminData.csv", "r");
    }

    Admin dbAdmin;
    char line[BUFSIZ];
    int matchFound = FALSE;
    int csvRow = 0, csvCol = 0;
    int len;


    //read read csv file to find if admin account exists
    while(fgets(line, BUFSIZ, file))
    {
        //remove extraline
        len = strlen(line);
        line[len - 1] = '\0';

        //get first token
        char *token = strtok(line, ",");

        while(token != NULL)
        {
            //printf("%s  ", token);

            //skip label and assign values from csvRow 1
            if((csvRow > 0) && csvCol == 0)
            {
                dbAdmin.id = atoi(token);
            }
            else if((csvRow > 0) && csvCol == 1)
            {
                strcpy(dbAdmin.name, token);
            }
            else if((csvRow > 0) && csvCol == 2)
            {
                strcpy(dbAdmin.phoneNumber, token);
            }
            else if((csvRow > 0) && csvCol == 3)
            {
                char decryptedPass[BUFSIZ];
                strcpy(decryptedPass, token);

                //decrypt hashed password
                decrypt(decryptedPass);

                strcpy(dbAdmin.password, decryptedPass);
            }

            //move to next column
            csvCol++;

            //get next token
            token = strtok(NULL, ",");
        }
        //printf("\n");


        // log file printf("id:%d  pass:%s\n", dbAdmin.id, dbAdmin.password);

        if(dbAdmin.id == id && (strcmp(dbAdmin.password, passwd) == 0))
        {
            matchFound = TRUE;
            break;
        }


        csvRow++;   //move to next row
        csvCol = 0; //reset col
    }


    if(matchFound)
    {
        system("clear");

        printf("\n\t\t\t\t  %s\n", dbAdmin.name);
    }
    else
    {
        printf("\n\t\t\tInvalid ID or password or both\n\n");
    }

    fclose(file);

    return matchFound;

}


int adminAcitvity()
{
    void createAccount();
    void viewAccountList();
    //void deleteAccount();
    void updateAccount(int adminAction, int transactionNum);

    int adminAction = TRUE;
    int choice;

    //system("clear");
    printf("\n\n \t\t  ****************************************\n\n");
    printf("\t\t\t   Activity menu\n\n");
    printf("\t\t\t1.Create Cutomer Account\n");
    printf("\t\t\t2.Delete Customer Account\n");
    printf("\t\t\t3.View all accounts\n");
    printf("\t\t\t4.Transfer Money\n");
    printf("\t\t\t5.Deposit\n");
    printf("\t\t\t6.Withdraw\n");
    printf("\t\t\t7.Logout");
    printf("\n\n \t\t  ****************************************\n\n");

    printf("\t\t\tSelect activity: ");
    scanf(" %d", &choice);

    switch (choice)
    {
        case 1:
            createAccount(); break;
        case 2:
            updateAccount(adminAction, 2); break;
        case 3:
            viewAccountList(); break;
        case 4:
            updateAccount(adminAction, 4);
            break;
        case 5:
            updateAccount(adminAction, 5);
            break;
        case 6:
            updateAccount(adminAction, 6);
            break;
        case 7:
            system("clear");
            exit(0); break;
        default:
            printf("\t\t\tInvalid input!\n");
            delay();
            system("clear");
    }

    return FALSE;
}

//customer login logic
int customerLogin(int accountNum, char *passwd)
{
    FILE *file = fopen("customerData.csv", "r");

    /*if file doesn't exist then create one, add superAdmin data then read it*/
    if(file == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

    Customer dbcustomer;
    char line[BUFSIZ];
    int matchFound = FALSE;
    int csvRow = 0;
    int len;


    //read read csv file to find if admin account exists
    while(fgets(line, BUFSIZ, file))
    {
        int csvCol = 0;


        //remove extraline
        len = strlen(line);
        line[len - 1] = '\0';

        //get first token
        char *token = strtok(line, ",");

        while(token != NULL)
        {

            //skip label and assign values from csvRow 1
            if((csvRow > 0) && csvCol == 0)
            {
                strcpy(dbcustomer.name,token);
            }
            else if((csvRow > 0) && csvCol == 1)
            {
                dbcustomer.accountNumber = atoi(token);
            }
            else if((csvRow > 0) && csvCol == 2)
            {
                dbcustomer.balance = atoi(token);
            }
            else if((csvRow > 0) && csvCol == 3)
            {
                strcpy(dbcustomer.phoneNumber,token);
            }
            else if((csvRow > 0) && csvCol == 4)
            {
                char decryptedPass[BUFSIZ];
                strcpy(decryptedPass, token);

                //decrypt hashed password
                decrypt(decryptedPass);

                strcpy(dbcustomer.password, decryptedPass);
            }

            //move to next column
            csvCol++;

            //get next token
            token = strtok(NULL, ",");
        }


        // log file printf("id:%d  pass:%s\n", dbAdmin.id, dbAdmin.password);

        if(dbcustomer.accountNumber == accountNum && (strcmp(dbcustomer.password, passwd) == 0))
        {
            matchFound = TRUE;
            break;
        }

        csvRow++;   //move to next row
    }


    if(matchFound)
    {
        system("clear");

        printf("\n\t\t\t\t  %s\n", dbcustomer.name);
    }
    else
    {
        printf("\n\t\t\tInvalid ID or password or both\n\n");
    }

    fclose(file);

    return matchFound;

}


int customerAcitvity(int userAccountNum)
{

    void updateAccount(int adminAction, int transactionNum);
    void checkBalance(int accountNum);

    int adminAction = FALSE;
    int choice;

    //system("clear");
    printf("\n\n \t\t  ****************************************\n\n");
    printf("\t\t\t   Activity menu\n\n");
    printf("\t\t\t1.Transfer Money\n");
    printf("\t\t\t2.Deposit\n");
    printf("\t\t\t3.Withdraw\n");
    printf("\t\t\t4.Check Balance\n");
    printf("\t\t\t5.Logout");
    printf("\n\n \t\t  ****************************************\n\n");

    printf("\t\t\tSelect activity: ");
    scanf(" %d", &choice);

    switch (choice)
    {
        case 1:
            updateAccount(adminAction, 1);
            break;
        case 2:
            updateAccount(adminAction, 2);
            break;
        case 3:
            updateAccount(adminAction, 3);
            break;
        case 4:
            checkBalance(userAccountNum);
            break;
        case 5:
            system("clear");
            exit(0); break;
        default:
            printf("\t\t\tInvalid input!\n");
            delay();
            system("clear");
    }

    return FALSE;
}



//createAccount
void createAccount()
{
    Customer customer;
    customer.balance = 0; //initial value

    //clear buffer
    getchar();

    printf("\n\t\t\tEnter client's information\n\n");
    printf("\t\t\tName: ");
    fgets(customer.name, SIZE, stdin);
    printf("\t\t\tPhone Number: ");
    scanf(" %s", customer.phoneNumber);
    printf("\t\t\tAccount Number: ");
    scanf(" %d", &customer.accountNumber);
    printf("\t\t\tPassword: ");
    scanf(" %s", customer.password);

    /*remove neline(\n)*/
    int len = strlen(customer.name);
    customer.name[len - 1] = '\0';

    //printf("%s\n", customer.name);

    FILE *file = fopen("customerData.csv", "r");

    if(file == NULL)
    {
        file = fopen("customerData.csv", "w");

        fprintf(file,"%s, %s, %s, %s, %s\n","NAME", "ACC NUM", "BALANCE","PHONE NUM", "PASSWORD");
    }
    fclose(file);

    file = fopen("customerData.csv", "a+");

    if(file == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

    //hash password before storage
    encrypt(customer.password);

    fprintf(file, "%s,%d,%d,%s,%s\n", customer.name, customer.accountNumber, customer.balance, customer.phoneNumber, customer.password);

    printf("\n\n\t\t\tAccount successfully created\n\n");

    customWait();

    system("clear");
    fclose(file);
}

//deleteAccount
void viewAccountList()
{
    //Customer customer;
    char line[BUFSIZ];
    char delim[2] = ",";
    int count = 0;

    FILE *file = fopen("customerData.csv", "r");

    if(file == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

    system("clear");
    printf("\n\n\t\t\t\tCUSTOMER LIST\n\n");

    while(fgets(line, BUFSIZ, file))
    {
        int col = 0;

        if(count == 0)
        {
            printf(" ");
        }
        else
        {
            printf("%d.", count);
        }

        //remove \n
        int len = strlen(line);
        line[len - 1] = '\0';

        //first token
        char *token = strtok(line, delim);

        while(token != NULL)
        {
            //don't display password
            if(col != 4)
            {
                printf("%-20s", token);
            }

            token = strtok(NULL, delim);
            col++;
        }
        printf("\n\n\n");

        count++;

    }

    customWait();
    system("clear");

    fclose(file);
}


/*
*has tranfer, deposit,  withdraw and delete capabilities
*/
void updateAccount(int adminAction, int transactionNum)
{
    int accountsExist(int fromAccount, int toAccount);
    int sufficientBalance(int fromAccount, int transferAmount);

    Customer customer;
    int keyedAccNum;
    int csvRow = 0;
    char line[BUFSIZ];
    char delim[2] = ",";
    int fromAccount, toAccount, transferAmount;
    int depositAccount, depositAmount;
    int withdrawAccount, withdrawAmount;
    int bothAccountsExist = FALSE;
    int sufficientWithdrawCash = FALSE;
    int deleted = FALSE, transferred = FALSE, deposited = FALSE, withdrawn = FALSE;


    //run deletion code requsted by admin
    if(adminAction && transactionNum == 2)
    {
        printf("\n\t\t\tAccount Deletion\n\n");
        printf("\t\t\tAccount Number: ");
        scanf(" %d", &keyedAccNum);
    }
    //run code that transfers money
    else if((adminAction && transactionNum == 4) || (!adminAction && transactionNum == 1))
    {
        printf("\n\t\t  Enter the required account details\n\n");
        printf("\t\t\tTransfer From Acc: ");
        scanf(" %d", &fromAccount);
        printf("\t\t\tTransfer To Acc: ");
        scanf(" %d", &toAccount);
        printf("\t\t\tTransfer Amount: ");
        scanf(" %d", &transferAmount);

        bothAccountsExist = accountsExist(fromAccount, toAccount);
        sufficientWithdrawCash = sufficientBalance(fromAccount, transferAmount);
    }
    //run code that deposits money
    else if((adminAction && transactionNum == 5) || (!adminAction && transactionNum == 2))
    {
        printf("\n\t\t\tMoney Deposit\n\n");
        printf("\t\t\tDeposit Acc: ");
        scanf(" %d", &depositAccount);
        printf("\t\t\tDeposit Amount: ");
        scanf(" %d", &depositAmount);
    }
    //run code that withdraws money
    else if((adminAction && transactionNum == 6) || (!adminAction && transactionNum == 3))
    {
        printf("\n\t\t\tMoney Withdrawal\n\n");
        printf("\t\t\tWithdrawal Acc: ");
        scanf(" %d", &withdrawAccount);
        printf("\t\t\tWithdraw Amount: ");
        scanf(" %d", &withdrawAmount);

        bothAccountsExist = accountsExist(withdrawAccount, withdrawAccount);
        sufficientWithdrawCash = sufficientBalance(withdrawAccount, withdrawAmount);
    }


    FILE *file = fopen("customerData.csv", "r");
    FILE *outfile = fopen("newData.csv", "a+");

    if(file == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

    fprintf(outfile,"%s, %s, %s, %s, %s\n","NAME", "ACC NUM", "BALANCE","PHONE NUM", "PASSWORD");

    while(fgets(line, BUFSIZ, file))
    {
        int csvCol = 0;

        //remove \n
        int len = strlen(line);
        line[len - 1] = '\0';

        //first token
        char *token = strtok(line, delim);

        while(token != NULL)
        {
            //skip label and assign values from csvRow 1
            if((csvRow > 0) && csvCol == 0)
            {
                strcpy(customer.name, token);
            }
            else if((csvRow > 0) && csvCol == 1)
            {
                customer.accountNumber = atoi(token);
            }
            else if((csvRow > 0) && csvCol == 2)
            {
                customer.balance = atoi(token);
            }
            else if((csvRow > 0) && csvCol == 3)
            {
                strcpy(customer.phoneNumber, token);
            }
            else if((csvRow > 0) && csvCol == 4)
            {
                strcpy(customer.password, token);
            }

            token = strtok(NULL, delim);
            csvCol++;
        }


        //run deletion code requsted by admin
        if(adminAction && transactionNum == 2)
        {
            if((csvRow > 0) && (customer.accountNumber != keyedAccNum))
            {
                fprintf(outfile, "%s,%d,%d,%s,%s\n", customer.name, customer.accountNumber, customer.balance, customer.phoneNumber, customer.password);
            }
            else if(customer.accountNumber == keyedAccNum)
            {
                deleted = TRUE;
            }
        }
        //run code that transfers money
        else if((adminAction && transactionNum == 4) || (!adminAction && transactionNum == 1))
        {
            if(bothAccountsExist)
            {
                if(sufficientWithdrawCash)
                {
                    //add money to account
                    if((csvRow > 0) && (customer.accountNumber == toAccount))
                    {
                        fprintf(outfile, "%s,%d,%d,%s,%s\n", customer.name, customer.accountNumber, customer.balance + transferAmount, customer.phoneNumber, customer.password);
                        transferred = TRUE;
                    }
                    //deduct money from account
                    else if((csvRow > 0) && (customer.accountNumber == fromAccount))
                    {
                        fprintf(outfile, "%s,%d,%d,%s,%s\n", customer.name, customer.accountNumber, customer.balance - transferAmount, customer.phoneNumber, customer.password);
                    }
                    else if(csvRow > 0)
                    {
                        fprintf(outfile, "%s,%d,%d,%s,%s\n", customer.name, customer.accountNumber, customer.balance, customer.phoneNumber, customer.password);
                    }
                }

            }
        }
        //run code that deposits money
        else if((adminAction && transactionNum == 5) || (!adminAction && transactionNum == 2))
        {
            if(accountsExist(depositAccount, depositAccount))
            {
                if((csvRow > 0) && (customer.accountNumber != depositAccount))
                {
                    fprintf(outfile, "%s,%d,%d,%s,%s\n", customer.name, customer.accountNumber, customer.balance, customer.phoneNumber, customer.password);
                }
                else if((csvRow > 0) && (customer.accountNumber == depositAccount))
                {
                    if(depositAmount > 0)
                    {
                        fprintf(outfile, "%s,%d,%d,%s,%s\n", customer.name, customer.accountNumber, customer.balance + depositAmount, customer.phoneNumber, customer.password);
                        deposited = TRUE;
                    }

                }
            }

        }
        //run code that withdraws money
        else if((adminAction && transactionNum == 6) || (!adminAction && transactionNum == 3))
        {
            if(bothAccountsExist)
            {
                if(sufficientWithdrawCash)
                {
                    if((csvRow > 0) && (customer.accountNumber != withdrawAccount))
                    {
                        fprintf(outfile, "%s,%d,%d,%s,%s\n", customer.name, customer.accountNumber, customer.balance, customer.phoneNumber, customer.password);
                    }
                    else if((csvRow > 0) && (customer.accountNumber == withdrawAccount))
                    {
                        if(withdrawAmount > 0)
                        {
                            fprintf(outfile, "%s,%d,%d,%s,%s\n", customer.name, customer.accountNumber, customer.balance - withdrawAmount, customer.phoneNumber, customer.password);
                            withdrawn = TRUE;
                        }
                    }
                }

            }

        }

        csvRow++;
    }


    //display deletion results to the admin
    if(adminAction && transactionNum == 2)
    {
        if(deleted)
        {
            printf("\n\n\t\t\taccount successfully deleted\n\n");
            rename("newData.csv", "customerData.csv");
            remove("newData.csv");

        }
        else
        {
            printf("\n\n\t\t\taccount does't exist\n\n");
            remove("newData.csv");
        }
    }
    //display transfer results
    else if((adminAction && transactionNum == 4) || (!adminAction && transactionNum == 1))
    {
        if(transferred)
        {
            printf("\n\n\t\t\ttransfer complete\n\n");
            rename("newData.csv", "customerData.csv");
            remove("newData.csv");

        }
        else
        {
            if(bothAccountsExist == FALSE)
            {
                printf("\n\t\t\taccounts don't match records\n\n");
            }
            else if(sufficientWithdrawCash == FALSE)
            {
                printf("\n\t\t\tinsufficient balance\n\n");
            }
            remove("newData.csv");
        }
    }
    //display deposit results
    else if((adminAction && transactionNum == 5) || (!adminAction && transactionNum == 2))
    {
        if(deposited)
        {
            printf("\n\n\t\t\t deposit completed\n\n");
            rename("newData.csv", "customerData.csv");
            remove("newData.csv");

        }
        else
        {
            printf("\n\n\t\t\taccount does't exist\n\n");
            remove("newData.csv");
        }
    }
    //display withdraw results
    else if((adminAction && transactionNum == 6) || (!adminAction && transactionNum == 3))
    {
        if(withdrawn)
        {
            printf("\n\n\t\t\t withdrawal completed\n\n");
            rename("newData.csv", "customerData.csv");
            remove("newData.csv");

        }
        else
        {
            if(bothAccountsExist == FALSE)
            {
                printf("\n\t\t\taccounts doesn't exist\n\n");
            }
            else if(sufficientWithdrawCash == FALSE)
            {
                printf("\n\t\t\tinsufficient balance\n\n");
            }
            remove("newData.csv");
        }
    }


    customWait();
    system("clear");
    fclose(file);
    fclose(outfile);

}


/*
*can check check whether one account is present i.e parse a similar value to both argumets
*can also check whether two different accounts are present in database
*returns true or false
* returns TRUE or FALSE
*/
int accountsExist(int fromAccount, int toAccount)
{
    Customer customer;
    char line[BUFSIZ];
    char delim[2] = ",";
    int csvRow = 0;
    int account1 = FALSE, account2 = FALSE;

    FILE *file = fopen("customerData.csv", "r");

    if(file == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }


    while(fgets(line, BUFSIZ, file))
    {
        int csvCol = 0;

        //remove \n
        int len = strlen(line);
        line[len - 1] = '\0';

        //first token
        char *token = strtok(line, delim);

        while(token != NULL)
        {
            //skip label and assign values from csvRow 1
            if((csvRow > 0) && csvCol == 1)
            {
                customer.accountNumber = atoi(token);

                if(customer.accountNumber == fromAccount)
                {
                    account1 = TRUE;
                }

                if(customer.accountNumber == toAccount)
                {
                    account2 = TRUE;
                }
            }

            token = strtok(NULL, delim);
            csvCol++;
        }
        csvRow++;

        if(account1 && account2)
        {
            fclose(file);
            return TRUE;
        }
    }

    fclose(file);
    return FALSE;
}


// returns TRUE or FALSE
int sufficientBalance(int fromAccount, int transferAmount)
{
    Customer customer;
    char line[BUFSIZ];
    char delim[2] = ",";
    int csvRow = 0;
    int enoughCash = FALSE;

    FILE *file = fopen("customerData.csv", "r");

    if(file == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }


    while(fgets(line, BUFSIZ, file))
    {
        int csvCol = 0;

        //remove \n
        int len = strlen(line);
        line[len - 1] = '\0';

        //first token
        char *token = strtok(line, delim);

        while(token != NULL)
        {
            //skip label and assign values from csvRow 1
            if((csvRow > 0) && csvCol == 1)
            {
                customer.accountNumber = atoi(token);
            }
            if((csvRow > 0) && csvCol == 2)
            {
                customer.balance = atoi(token);
            }

            token = strtok(NULL, delim);
            csvCol++;
        }
        csvRow++;


        if(customer.accountNumber == fromAccount && customer.balance >= transferAmount)
        {
             enoughCash = TRUE;

             fclose(file);
             return enoughCash;
        }
    }

    fclose(file);
    return enoughCash;
}


void checkBalance(int accountNum)
{
    Customer customer;
    char line[BUFSIZ];
    char delim[2] = ",";
    int csvRow = 0;
    int balance = 0;

    FILE *file = fopen("customerData.csv", "r");

    if(file == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

    while(fgets(line, BUFSIZ, file))
    {
        int csvCol = 0;

        //remove \n
        int len = strlen(line);
        line[len - 1] = '\0';

        //first token
        char *token = strtok(line, delim);

        while(token != NULL)
        {
            //skip label and assign values from csvRow 1
            if((csvRow > 0) && csvCol == 1)
            {
                customer.accountNumber = atoi(token);
            }
            if((csvRow > 0) && csvCol == 2)
            {
                customer.balance = atoi(token);
            }

            token = strtok(NULL, delim);
            csvCol++;
        }
        csvRow++;


        if(customer.accountNumber == accountNum)
        {
             balance = customer.balance;
        }
    }

    printf("\n\t\t\tBalance: %d\n\n", balance);

    customWait();
    system("clear");
    fclose(file);
}


void turnoffCanonMode()
{
    struct termios settings;

    tcgetattr(0, &settings);                //get currrent settings
    settings.c_lflag &= ~ICANON;            //turn off buffering
    settings.c_cc[VMIN] = 1;                //get 1 character at a time
    tcsetattr(0, TCSANOW, &settings);       //update settings
}


void turnoffEcho()
{
    struct termios settings;

    tcgetattr(0, &settings);                //get currrent settings
    settings.c_lflag &= ~ECHO;              //no echo
    tcsetattr(0, TCSANOW, &settings);       //update settings
}

void getCurrSettOrRestoreSett(int trigger)
{
    static struct termios originalSettings;
    static int originalFlags;

    if(trigger == 0)
    {
        tcgetattr(0, &originalSettings);            //get original settings
        originalFlags = fcntl(0, F_GETFL);          //read current flags
    }
    else
    {
        tcsetattr(0, TCSANOW, &originalSettings);    //restore original settings
        fcntl(0, F_SETFL, originalFlags);            //restore original flags
    }
}

void kboardAsyncSig()
{
    int fdFlag;

    fcntl(0, F_SETOWN, getpid());           //notify kernel to send process's input notifications
    fdFlag = fcntl(0, F_GETFL);             //read current settings
    fcntl(0, F_SETFL, (fdFlag|O_ASYNC));    //update settings
}

int timer(int millisec)
{
    struct itimerval setDuration;

    long n_sec, n_usec;

    n_sec = millisec / 1000;
    n_usec = (millisec % 1000) * 1000;

    setDuration.it_value.tv_sec = n_sec;
    setDuration.it_value.tv_usec = n_usec;

    setDuration.it_interval.tv_sec = 0;
    setDuration.it_interval.tv_usec = 0;

    return setitimer(ITIMER_REAL, &setDuration, NULL);
}

void encrypt(char *password)
{
    FILE *outfile;

	//create a file to store the password
	outfile = fopen("plainPass.txt", "w");
	fprintf(outfile, "%s", password);
	fclose(outfile);


	//process to hash user password using sha256 algorithm
	int pid = fork();

	if(pid < 0)
	{
		perror("Can't create a new process!\n");
		exit(1);
	}

	if(pid == 0)
	{
		//char *argv[] = {"openssl", "aes-256-cbc", "-in", "plainPass.txt", "-out", "hashPass.txt",  "-a", "-salt", "-pbkdf2", "-pass", "pass:CONFIDENTIAL@cipher", NULL};
		execlp("openssl", "openssl", "aes-256-cbc", "-in", "plainPass.txt", "-out", "hashPass.txt",  "-a", "-salt", "-pbkdf2", "-pass", "pass:CONFIDENTIAL@cipher", NULL);
	}

	//printf("\nWaiting for child process to encrypt...\n");
	wait(NULL);
	//printf("Encyption process complete.\n");

    FILE *infile = fopen("hashPass.txt", "r");

    char ciphertext[BUFSIZ];

    fgets(ciphertext, BUFSIZ, infile);

    //remove added line
    int len = strlen(ciphertext);
    ciphertext[len - 1] = '\0';

    //copy the hashed value to password
    strcpy(password, ciphertext);

    fclose(infile);

    /*delete text files*/
    remove("plainPass.txt");
    remove("hashPass.txt");

}

void decrypt(char *password)
{

    FILE *outfile;

	//create a file to store the password
	outfile = fopen("d_ciphertext.txt", "w");
	fprintf(outfile, "%s\n", password);    // (\n)shows where the string ends
	fclose(outfile);

    //process to decrypt customer passwords using sha256 algorithm
	int id = fork();

	if(id < 0)
	{
		perror("Can't create a new process!\n");
		exit(1);
	}

	if(id == 0)
	{
		//char *argv[] = {"openssl", "aes-256-cbc", "-in", "d_ciphertext.txt", "-out", "d_plaintext.txt", "-d", "-a", "-salt", "-pbkdf2", "-pass", "pass:CONFIDENTIAL@cipher", NULL};
		execlp("openssl", "openssl", "aes-256-cbc", "-in", "d_ciphertext.txt", "-out", "d_plaintext.txt", "-d", "-a", "-salt", "-pbkdf2", "-pass", "pass:CONFIDENTIAL@cipher", NULL);
	}

	//printf("\nWaiting for child process to decrypt...\n");
	wait(NULL);
	//printf("decyption process complete.\n");


    FILE *infile = fopen("d_plaintext.txt", "r");

    char plaintext[BUFSIZ];

    fgets(plaintext, BUFSIZ, infile);

    //copy the hashed value to password
    strcpy(password, plaintext);

    fclose(infile);

    /*delete text files*/
    remove("d_plaintext.txt");
    remove("d_ciphertext.txt");

}
