#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#define BUF_SIZE 1000000

void indexPage();
void inputData();
void writeData(int newDate, int money, char *description);
void printData();
void outputData();
void publish_json();

int main() {
    indexPage();
    return 0;
}

void indexPage() {
    printf("\n#\n# ACCOUNT BOOK\n#\n");
    printf("1. input   2. print   3. output  4.quit\n");
    printf("please input the number...\n> ");

    int selector;
    scanf("%d", &selector);
    getchar();

    switch(selector) {
        case 1:
            inputData();
            break;
        case 2:
            printData();
            break;
        case 3:
            outputData();
            break;
        case 4:
            break;
        default:
            break;
    }
}

void inputData() {
    printf("\nEnter your consumption or income.\n# Please enter the date. yyyymmdd (If write 0, the current time is entered.)\n> ");

    int newDate;
    scanf("%d", &newDate);
    getchar();

    if (newDate == 0) {
        time_t nowTime = time(NULL);
        struct tm *timeData = localtime(&nowTime);

        int year = timeData->tm_year + 1900;
        int month = timeData->tm_mon + 1;
        int day = timeData->tm_mday;

        newDate = 10000*year + 100*month + day;
    }

    if ((newDate%100 > 0 && newDate%100 < 32) && ((newDate/100)%100 > 0 && (newDate/100)%100 < 13) && newDate < 99991232 ) {

        printf("\n# Please enter your income or expenses. (Income is positive and expenditure is negative.)\n> ");
        int money;
        scanf("%d", &money);
        getchar();

        printf("\n# Please enter description about this income or expenses.\n>");
        char description[300] = {0};
        gets(description);
        getchar();

        writeData(newDate, money, description);

        printf("\n# Input Success!\n");
    }
    else printf("\n# You entered an incorrect value. Please check again.\n");

    indexPage();
}

void writeData(int newDate, int money, char *description) {
    int fd = open("data", O_RDWR | O_CREAT, 0644);

    lseek(fd, 0, SEEK_END);
    char start[1] = "S";
    write(fd, start, 1);

    lseek(fd, 0, SEEK_END);
    char char_date[10] = {0};
    itoa(newDate, char_date, 10);
    char_date[8] = 'M';
    write(fd, char_date, 9);
    printf("# date write complete!\n");

    lseek(fd, 0, SEEK_END);
    char char_money[100] = {0};

    itoa(money, char_money, 10);
    int moneySize = strlen(char_money);

    char_money[moneySize] = 'N';

    write(fd, char_money, moneySize+1);
    printf("# money write complete!\n");


    lseek(fd, 0, SEEK_END);
    int descriptionSize = strlen(description);

    description[descriptionSize] = 'D';

    write(fd, description, descriptionSize+1);
    printf("# description write complete!\n");

    printf("# all data write complete!\n");

    close(fd);
}

void printData() {
    int fd = open("data", O_RDONLY, 0644);
    if(fd < 0) {
        printf("\n# file is missing!\n");
        exit(-1);
    }

    char buf[BUF_SIZE] = {0};
    read(fd, buf, BUF_SIZE);

    printf("\n##date## #amount# #description#\n");
    char money[30];
    int sum = 0, j;

    for(int i = 0; buf[i] != '\0'; i++) {
        if (buf[i] == 'E')
            printf(" ");
        else if (buf[i] == 'M') {
            printf(" ");
            i++;
            for(j = 0; buf[i+j] != 'N'; j++) {
                printf("%c", buf[i+j]);
                money[j] = buf[i+j];
            }
            money[j+1] = '\0';
            sum += atoi(money);
            i = i+j;
            printf(" ");
        }
        else if (buf[i] == 'D')
            printf("\n");
        else if (buf[i] == 'S')
            printf("");
        else
            printf("%c", buf[i]);
    }

    printf("# sum of amount : %d\n\n", sum);
    close(fd);
    indexPage();
}

void outputData() {
    printf("\n# You can publish your account book data. please select the file format.\n1. json file\n> ");
    int outputSelector;
    scanf("%d", &outputSelector);

    switch(outputSelector) {
        case 1:
            publish_json();
            break;
        default:
            printf("\n# invalid input. please check again.\n");
            break;
    }

    indexPage();
}

void publish_json() {
    int fd = open("data", O_RDONLY, 0644);
    if(fd < 0) {
        printf("\n# Source file is missing!\n");
        exit(-1);
    }

    printf("\n# Enter a name for the new file (file name cannot be overlap)\n> ");
    char filename[3000] = {0};
    scanf("%s", filename);
    strcat(filename, ".json");

    char outputBuf[BUF_SIZE] = {0};
    strcat(outputBuf,"{\"data\":[");

    char buf[BUF_SIZE] = {0};
    read(fd, buf, BUF_SIZE);

    for(int i = 0; buf[i] != '\0'; i++) {
        if (buf[i] == 'S')
            strcat(outputBuf,"{\"date\":\"");
        else if (buf[i] == 'M')
            strcat(outputBuf,"\",\"money\":\"");
        else if (buf[i] == 'N')
            strcat(outputBuf,"\",\"description\":\"");
        else if (buf[i] == 'D')
            strcat(outputBuf,"\"},");
        else {
            char temp[2] = {0};
            temp[0] = buf[i];
            temp[1] = '\0';
            strcat(outputBuf, temp);
        }
    }

    strcat(outputBuf,"]}");

    int bufSize = strlen(outputBuf);
    int newfd = open(filename, O_RDWR | O_CREAT, 0644);

    write(newfd, outputBuf, bufSize);

    printf("%s", outputBuf);
    printf("\n# json file publish complete! check the %s\n", filename);
}