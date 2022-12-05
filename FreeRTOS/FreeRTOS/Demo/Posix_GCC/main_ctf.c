/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
 * main() creates all the demo application tasks, then starts the scheduler.
 * The web documentation provides more details of the standard demo application
 * tasks, which provide no particular functionality but do provide a good
 * example of how to use the FreeRTOS API.
 *
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "Check" task - This only executes every five seconds but has a high priority
 * to ensure it gets processor time.  Its main function is to check that all the
 * standard demo tasks are still operational.  While no errors have been
 * discovered the check task will print out "OK" and the current simulated tick
 * time.  If an error is discovered in the execution of a task then the check
 * task will print out an appropriate error message.
 *
 */


/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"       /* RTOS firmware */
#include "task.h"           /* Task */
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include <time.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h> 
#include "hash.h"
#include "tea.h"
#include "currentTime.h"

#define MAX_STDIN_BUF   100000
#define MAX_GAMES_BUF   100
#define LONG_TERM_KEY   42

static int input_validation();
static void test_caesar();

char ownername[UCHAR_MAX];
int hacked_ownername = 0;
int hacked_get_debug_inf = 0;
int caesar_key = 1;
char game_buf[MAX_GAMES_BUF];
uint64_t short_key_buf[2];
char correct_pass[4] = {'a', 'b', 'c', 'd'};
uint16_t daily_secret;
TickType_t criticial_section_time = 250;
TickType_t med_wait = 5000;
static SemaphoreHandle_t lock;
uint64_t forward_secret_key;


typedef struct Encrypt_xor_params
{
    uint64_t data[3];
} GenericEncrypt_xor_params;
GenericEncrypt_xor_params tea_params;
GenericEncrypt_xor_params keyExchange;

void take_keyboard_input(char* str) {
    char c = 0;
    int i = 0;
    while (c != '\n')
    {
        c = getchar();
        if (c != EOF)
        {
            str[i] = c;
            i++;
        }
    }
    str[i] = '\0';
}

void take_safe_keyboard_input(char* str, int n) {
    char c = 0;
    int i = 0;
    while (c != '\n')
    {
        c = getchar();
        if(i == n && c != '\n') {
            printf("Only %d characters allowed~", n);
            return;
        }
        if (c != EOF)
        {
            str[i] = c;
            i++;
        }
    }
    str[i] = '\0';
}

static int get_debug_info() {
    return 0;
}

static void hacked() {
    printf("\nCongratulations! You captured the FLAG!\n");
    exit(0);
}

static void test_realtime_tasks() {
    int lock_type = 0;
    char buf_med_wait[100];
    char buf_priority[100];
    printf("\nEnter duration in ms that medium priortiy task spends working:");
    take_safe_keyboard_input(buf_med_wait, 100);
    med_wait = atoi(buf_med_wait);
    printf("\nmed wait = %d", med_wait);
    printf("\nChoose one lock type");
    printf("\n1. xSemaphoreCreateBinary()");
    printf("\n2. xSemaphoreCreateMutex()\n");
    take_safe_keyboard_input(buf_priority, 100);
    lock_type = atoi(buf_priority);
    if(lock_type == 1) {
        lock = xSemaphoreCreateBinary();
    } else if(lock_type == 2) {
        lock = xSemaphoreCreateMutex();
    } else {
        printf("\nWrong selection");
        return;
    }
    priority_inversion();
}

static int input_validation() {
    char name[UCHAR_MAX];
    int get_debug_inf =  get_debug_info();
    printf("\n\nEnter New Name: ");
    take_keyboard_input(name);
    unsigned char name_len = strlen(name);
    if(name_len < UCHAR_MAX) {
        strncpy(ownername, name, name_len);
        ownername[name_len] = '\0';
        printf("New owner's Name = %s", ownername);
        if(strncmp(ownername, "Bournvita", 9) == 0) {
            hacked_ownername = 1;
        }
    } else {
        printf("\nERROR");
        return;
    }
    if(get_debug_inf) {
        hacked_get_debug_inf = 1;
    }
    if(hacked_ownername == 1 && hacked_get_debug_inf == 1) {
        hacked();
    } else {
        printf("No flag yet. Try harder!~");

    }
    // if(get_debug_inf) {
    //     // test_caesar();
    //     test_realtime_tasks();
    // }
}

static void caesar_encrypt(char message[], int key, int i) {
    if(i == strlen(message)) {
        return;
    }
    key = key%26;
    if (key > 0) {
        char ch = message[i];
        if(ch >= 'a' && ch <= 'z'){
                ch = ch + key;
                if(ch > 'z') {
                    ch = ch - 'z' + 'a' - 1;
                }
                message[i] = ch;
            }
            else if(ch >= 'A' && ch <= 'Z'){
                ch = ch + key;
                if(ch > 'Z') {
                    ch = ch - 'Z' + 'A' - 1;
                }
                message[i] = ch;
            }
        caesar_encrypt(message, key, i + 1);
    } else {
        printf("Only positive integer keys allowed");
    }
}

static void test_caesar() {
    char message[MAX_STDIN_BUF] = "";
    char ch;
    int i;
    printf("\nEnter a message to encrypt: ");
    take_safe_keyboard_input(message, MAX_STDIN_BUF); // TODO::create a safe function
    printf("\nYou entered string of length %lu\n", strlen(message));
    caesar_encrypt(message, caesar_key, 0);
    printf("\nEncrypted message: %s", message);
}


static uint64_t get_public_IP() {
    return 0x47cef5ea;
}


static uint64_t generate_short_term_key(i) {
    unsigned char random = rand();
    uint64_t short_term_key = (uint64_t)daily_secret ^ random;
    short_key_buf[i] = short_term_key;
    return short_term_key;
}

void exchange_short_term_key_using_long_term_key(int i) {
    forward_secret_key  = generate_short_term_key(i);
    keyExchange.data[0] = getEpochSeconds();
    keyExchange.data[1] = forward_secret_key;
    tea_encrypt(&keyExchange, LONG_TERM_KEY, 0);
}

static void forward_secret_message() {
    char short_key_input[65];
    char long_key_input[65];
    uint64_t short_key_ans;
    uint64_t long_key_ans;
    exchange_short_term_key_using_long_term_key(0);
    for(int i = 0; i < 60; i++) {
        printf("\n%d", i);
        tea_params.data[0] = getEpochSeconds();
        tea_params.data[1] = get_public_IP();
        tea_encrypt(&tea_params, forward_secret_key, 1);
        vTaskDelay(1000);
    }
    exchange_short_term_key_using_long_term_key(1);
    printf("\nWhat's the short term key in decimal?");
    take_safe_keyboard_input(short_key_input, 64);
    printf("What's the long term key in decimal?");
    take_safe_keyboard_input(long_key_input, 64);
    short_key_ans = atoi(short_key_buf);
    long_key_ans = atoi(long_key_input);
    if((short_key_ans == short_key_buf[0] || short_key_ans == short_key_buf[1]) && long_key_ans == LONG_TERM_KEY) {
        hacked();
    } else {
        printf("\nTry harder!~");
    }
}

static void settings_menu() {
    printf("=============SETTINGS MENU=============\n");
    printf("1. Change owner's name\n");
    char c = 0;
    char str[2];
    int i = 0;
    while (c != '\n')
    {
        c = getchar();
        if(i == 2 && c != '\n') {
            return;
        }
        if (c != EOF)
        {
            str[i] = c;
            i++;
        }
    }
    str[i] = '\0';
    if(strncmp(str, "1", 1) == 0) {
        input_validation();
    } else {
        printf("Invalid option, try again later");
        return;
    }
}

static void games() {
    printf("\nYAY gaming! Enter random game string:\n");
    for(int i = 0; i < MAX_GAMES_BUF; i++) {
        game_buf[i] = 0;
    }
    char c = 0;
    int i = 0;
    while (c != '\n')
    {
        if(i == MAX_GAMES_BUF-1) {
            printf("\nGame ended!\n");
            return;
        }
        c = getchar();
        if (c != EOF)
        {
            game_buf[i] = c;
            i++;
        }
    }
    game_buf[i] = '\0';
    daily_secret = hash_pFastVLTS_16b(game_buf, i);
    printf("Hash of game data = %x", daily_secret);
    forward_secret_message();
}

static void authenticated_screen() {
    printf("=============WELCOME MENU=============");
    printf("\n1. Play Games");
    printf("\n2. Settings\n");
    char c = 0;
    char str[2];
    int i = 0;
    while (c != '\n')
    {
        c = getchar();
        if(i == 2 && c != '\n') {
            return;
        }
        if (c != EOF)
        {
            str[i] = c;
            i++;
        }
    }
    str[i] = '\0';
    if(strncmp(str, "1", 1) == 0) {
        games();
    } else if(strncmp(str, "2", 1) == 0) {
        settings_menu();
    } else {
        printf("Invalid option, try again later");
        return;
    }
}

static void start_screen() {
    printf("\nENTER PASS:\n");
    int badpass = 0;
    char pass[4];
    char c = 0;
    int i = 0;
    while (c != '\n')
    {
        c = getchar();
        if(i == 4 && c != '\n') {
            printf("Only 4 characters allowed~");
            return;
        }
        if (c != EOF)
        {
            if(c == 'A') c = 'a';
            if(c == 'B') c = 'b';
            if(c == 'C') c = 'c';
            if(c == 'D') c = 'd';
            pass[i] = c;
            i++;
        }
    }
    long time1 = getNanotime();
    for(volatile int i = 0; i < 4; i++) {
        if(correct_pass[i] != pass[i]) {
            badpass = 1;
            sleep(1);
            break;
        } else {
            sleep(1);
        }
    }
    long time2 = getNanotime();
    long time_diff = time2 - time1;
    printf("\nTime_diff = %ld", time_diff);
    if(badpass) {
        printf("\nWrong Pass! :(\n");
    } else {
        printf("\nWELCOME\n");
        hacked();
        // authenticated_screen();
    }
}

static void low() {
  TickType_t timestamp;
  int i = 0;
  //Superloop
  while (1) {
    // Take lock
    console_print("\nL priority task starts...");
    xSemaphoreTake(lock, portMAX_DELAY);

    // Do some work ...
    printf("\nLow P grabbed the lock");
    // Utilize the processor
    timestamp = xTaskGetTickCount();
    while (xTaskGetTickCount() - timestamp < criticial_section_time) {}

    // Release lock
    console_print("\nL priority task done");
    xSemaphoreGive(lock);
    // Go to sleep
    vTaskDelay(500);
  }
  vTaskDelete(NULL);
}

static void medium() {
  TickType_t timestamp;
  while (1) {
    console_print("\nLoooooong M priority task starts...\n");
    timestamp = xTaskGetTickCount();
    while ( xTaskGetTickCount() - timestamp < med_wait) {}
    console_print("\nM priority task done");
    vTaskDelay(500);
  }
  vTaskDelete(NULL);
}

static void high() {
  TickType_t timestamp;
  while (1) {

    // Take lock
    console_print("\nHigh priority task starts...");
    timestamp = xTaskGetTickCount();
    xSemaphoreTake(lock, portMAX_DELAY);
    if((xTaskGetTickCount() - timestamp) >= 5000) {
        printf("\nSystem Restart!!");
        hacked();
    }

    // Do some work
    printf("\nHigh P grabbed the lock");
    timestamp = xTaskGetTickCount();
    while ( xTaskGetTickCount() - timestamp < criticial_section_time) {}

    // Release lock
    console_print("\nHigh P task ends");
    xSemaphoreGive(lock);
    // Sleep
    vTaskDelay(500);
  }
  vTaskDelete(NULL);
}

void priority_inversion () {
    xSemaphoreGive(lock);
    xTaskCreate(low, "Task L", 1024, NULL, 1, NULL);
    vTaskDelay(100);
    xTaskCreate(high, "Task H", 1024, NULL, 3, NULL);
    xTaskCreate(medium, "Task M", 1024, NULL, 2, NULL);
    vTaskDelete(NULL);
}


static void superloop_1() {
    while (1)
    {
        start_screen();
    }
    
}

static void superloop_2() {
    while(1) {
        input_validation();
    }
}

static void superloop_3() {
    while(1) {
        test_caesar();
    }
}

static void superloop_4() {
    while(1) {
        games();
    }
}

static void superloop_5() {
    while(1) {
        test_realtime_tasks();
    }
}

int main_ctfs(int argc, char** argv) {
    if(argc != 2) {
        printf("\nIncorrect way to use. Correct usage: posix_demo <CTF#>\n");
        return;
    }
    int ctfNum = atoi(argv[1]);
    if(ctfNum == 1) {
        xTaskCreate(superloop_1, "super_loop1", PTHREAD_STACK_MIN*2, NULL, 5, NULL);
    }
    else if(ctfNum == 2) {
        xTaskCreate(superloop_2, "super_loop2", PTHREAD_STACK_MIN*2, NULL, 5, NULL);
    }
    else if(ctfNum == 3) {
        xTaskCreate(superloop_3, "super_loop3", PTHREAD_STACK_MIN*2, NULL, 5, NULL);
    }
    else if(ctfNum == 4) {
        xTaskCreate(superloop_4, "super_loop4", PTHREAD_STACK_MIN*2, NULL, 5, NULL);
    }
    else if(ctfNum == 5) {
        lock = xSemaphoreCreateMutex();
        xTaskCreate(superloop_5, "super_loop5", PTHREAD_STACK_MIN*2, NULL, 5, NULL);
    }
    else {
        printf("Incorrect way to use. Correct usage: posix_demo <CTF#>\nAllowed CTF# [1, 2, 3, 4, 5]\n");
        return;
    }
    vTaskStartScheduler();
}