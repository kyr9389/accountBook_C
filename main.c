#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#define BUF_SIZE 1000000    

void indexPage();
void inputData();
void writeData(int newDate, int money, char* description);
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

    switch (selector) {
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
    // 먼저 yyyymmdd 형태로 수입 또는 지출한 날짜를 입력받는다
    printf("\nEnter your consumption or income.\n# Please enter the date. yyyymmdd (If write 0, the current time is entered.)\n> ");

    int newDate;
    scanf("%d", &newDate);
    getchar();

    // 날짜에 0을 적으면 자동으로 현재 로컬데이터로 변경한다
    if (newDate == 0) {
        time_t nowTime = time(NULL);
        struct tm* timeData = localtime(&nowTime);
        //data 타입에 맞게 년월일 변경한다
        int year = timeData->tm_year + 1900;
        int month = timeData->tm_mon + 1;
        int day = timeData->tm_mday;

        newDate = 10000 * year + 100 * month + day;
    }

    //input 값이 날짜에 맞는 범위인지 확인한다
    if ((newDate % 100 > 0 && newDate % 100 < 32) && ((newDate / 100) % 100 > 0 && (newDate / 100) % 100 < 13) && newDate < 99991232) {

        // 수입 지출은 +-부호로 구분, 금액을 입력 받는다
        printf("\n# Please enter your income or expenses. (Income is positive and expenditure is negative.)\n> ");
        int money;
        scanf("%d", &money);
        getchar();

        // 해당 수입 지출에 대한 상세 설명을 적는다 
        printf("\n# Please enter description about this income or expenses.\n>");
        char description[300] = { 0 };
        gets(description);
        getchar();

        // data 파일에 해당 내용을 적는다
        writeData(newDate, money, description);

        printf("\n# Input Success!\n");
    }
    else printf("\n# You entered an incorrect value. Please check again.\n");

    indexPage();
}

void writeData(int newDate, int money, char* description) {
    // 수익 지출을 기록할 data 파일을 읽기+쓰기 모드로 연다 ( 처음 작성시 data 파일이 생성된다 )
    int fd = open("data", O_RDWR | O_CREAT, 0644);

    // 기존 data 파일의 작성한 내용의 마지막 지점부터 적기를 시작한다. 먼저 'S' 를 적는다
    lseek(fd, 0, SEEK_END);
    char start[1] = "S";
    write(fd, start, 1);

    lseek(fd, 0, SEEK_END);
    char char_date[10] = { 0 };
    // int 형으로 받아온 date를 문자로 변경해준다.
    itoa(newDate, char_date, 10);
    // yyyymmdd + 'M' 의 형태로 변경해 준 후 해당 9글자를 기록한다.
    char_date[8] = 'M';
    write(fd, char_date, 9);
    printf("# date write complete!\n");

    lseek(fd, 0, SEEK_END);
    char char_money[100] = { 0 };
    // int 형으로 받아온 money를 문자로 변경해준다.
    itoa(money, char_money, 10);

    // money 글자 길이를 세어주고 money 문자열 뒤에 N을 적어준 후 기록한다.
    int moneySize = strlen(char_money);
    char_money[moneySize] = 'N';
    write(fd, char_money, moneySize + 1);
    printf("# money write complete!\n");

    lseek(fd, 0, SEEK_END);
    // desciprtion 글자 길이를 세어주고 descirption 문자열 뒤에 D를 적어준 후 기록한다.
    int descriptionSize = strlen(description);
    description[descriptionSize] = 'D';
    write(fd, description, descriptionSize + 1);
    printf("# description write complete!\n");

    printf("# all data write complete!\n");
    close(fd);
}

void printData() {
    // 수익 지출을 기록한 data 파일을 읽기모드로 연다
    int fd = open("data", O_RDONLY, 0644);
    if (fd < 0) {
        printf("\n# file is missing!\n");
        exit(-1);
    }

    // buf 문자열에 fd의 기록을 담는다
    char buf[BUF_SIZE] = { 0 };
    read(fd, buf, BUF_SIZE);

    printf("\n##date## #amount# #description#\n");
    char money[30];
    int sum = 0, j;

    // buf 내용이 끝날 때까지 for 문을 돈다
    for (int i = 0; buf[i] != '\0'; i++) {

        // S : 시작점 M : 날짜 기록 종료 지점 N : money 기록 종료 지점 D : description 기록 종료 지점 
        // 4개의 구분자로 데이터를 알맞게 처리, 출력한다.
        if (buf[i] == 'M') {
            printf(" ");
            i++;
            // M - N 사이에 있는 money 글자를 숫자로 변환하고 최종 합계금액에 더한다
            for (j = 0; buf[i + j] != 'N'; j++) {
                printf("%c", buf[i + j]);
                money[j] = buf[i + j];
            }
            money[j + 1] = '\0';
            sum += atoi(money);
            i = i + j;
            printf(" ");
        }
        else if (buf[i] == 'D')
            printf("\n");
        else if (buf[i] == 'S')
            printf("");
        else
            printf("%c", buf[i]);
    }

    // 금액 합계를 보여준다
    printf("# sum of amount : %d\n\n", sum);
    close(fd);
    indexPage();
}

void outputData() {
    // 1을 입력하면 data 파일의 정보를 json 파일로 받을 수 있게한다.
    printf("\n# You can publish your account book data. please select the file format.\n1. json file\n> ");
    int outputSelector;
    scanf("%d", &outputSelector);

    switch (outputSelector) {
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
    // data 파일을 읽기 전용으로 연다
    int fd = open("data", O_RDONLY, 0644);
    if (fd < 0) {
        printf("\n# Source file is missing!\n");
        exit(-1);
    }

    // 작성할 파일 이름을 입력받고  확장자를 json으로 해준다.
    printf("\n# Enter a name for the new file (file name cannot be overlap)\n> ");
    char filename[3000] = { 0 };
    scanf("%s", filename);
    strcat(filename, ".json");

    // json 형태로 기록될 outputBuf를 만든다
    char outputBuf[BUF_SIZE] = { 0 };
    strcat(outputBuf, "{\"data\":[");

    // data 파일기록을 buf에 담는다
    char buf[BUF_SIZE] = { 0 };
    read(fd, buf, BUF_SIZE);

    // json 형태에 맞게 buf에 담긴 내용을 outputBuf에 정리해 준다
    for (int i = 0; buf[i] != '\0'; i++) {
        if (buf[i] == 'S')
            strcat(outputBuf, "{\"date\":\"");
        else if (buf[i] == 'M')
            strcat(outputBuf, "\",\"money\":\"");
        else if (buf[i] == 'N')
            strcat(outputBuf, "\",\"description\":\"");
        else if (buf[i] == 'D')
            strcat(outputBuf, "\"},");
        else {
            char temp[2] = { 0 };
            temp[0] = buf[i];
            temp[1] = '\0';
            strcat(outputBuf, temp);
        }
    }

    strcat(outputBuf, "]}");

    int bufSize = strlen(outputBuf);
    // json 파일을 만든다
    int newfd = open(filename, O_RDWR | O_CREAT, 0644);

    write(newfd, outputBuf, bufSize);

    printf("%s", outputBuf);
    printf("\n# json file publish complete! check the %s\n", filename);
}