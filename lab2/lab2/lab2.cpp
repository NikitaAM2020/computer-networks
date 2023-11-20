#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <conio.h>

//Оголошення глобальних змінних
CRITICAL_SECTION _cs;
const wchar_t eventNameAuto[] = L"sp_lab2_SyncEventAuto";
const wchar_t eventNameManual[] = L"sp_lab2_SyncEventManual";

void PrintPositiveNumbers(int max = 500);
void PrintNegativeNumbers(int min = -500);
void GoThreads(char choose = 0);

bool stopPrimeThread = false;


void main()
{
    /* ЗАВДАННЯ 1 - Створення 2 потоків, які асинхронно друкуватимуть ряд чисел у консоль (від 1 до 500 та від -1 до -500 відповідно) */
    puts("task 1: asynchronous threads flow.");
    GoThreads(0);
    system("pause");

    /* ЗАВДАННЯ 2 - Синхронізувати роботу потоків із завдання 1 так, щоб спочатку були надруковані всі додатні числа, а потім – від’ємні. */
    puts("task 2.1 and 2.3: threads synchronized with CriticalSection or CriticalSectionAndSpinCount.");
    puts("Set spin count (use positive value to initialize spin count OR use 0 to use simple CriticalSection)");
    unsigned int cnt;
    scanf("%u", &cnt);
    puts("Creating CriticalSection");
    if (cnt) InitializeCriticalSectionAndSpinCount(&_cs, (DWORD)cnt);
    else InitializeCriticalSection(&_cs);
    GoThreads(1);
    DeleteCriticalSection(&_cs);
    system("pause");

    puts("task 2.2: threads synchronized with Event (auto-reset)");
    puts("Creating Event using auto-reset");
    HANDLE _event1 = CreateEvent(NULL, FALSE, TRUE, eventNameAuto);
    GoThreads(2);
    Sleep(1000);
    SetEvent(_event1);
    CloseHandle(_event1);
    system("pause");

    /* ЗАВДАННЯ 3 - Змінити пріоритети потоків завдання 1 щоб потік друку від’ємних чисел мав пріоритет більший середнього, а
    потік друку додатних чисел – пріоритет нижче середнього.*/
    puts("task 3: Changing thread priorities");
    GoThreads(3);
    system("pause");

    /* ЗАВДАННЯ 4 - Створити потік для обчислення і друку простих чисел */
    puts("task 4: Create and manage the prime numbers thread");
    GoThreads(4);
    system("pause");

    /* ЗАВДАННЯ 5 - Продемонструвати на довільному прикладі різницю у роботі подій (event) з автооновленням
    та без автооновлення (auto-reset).*/
    puts("task 5: threads synchronized with Event (MANUAL RESET vs AUTO-RESET)");
    puts("Creating Event using manual reset");
    HANDLE _event2 = CreateEvent(NULL, TRUE, TRUE, eventNameManual);
    GoThreads(5);
    SetEvent(_event2);
    CloseHandle(_event2);
    system("pause");

}

DWORD WINAPI Thread0(PVOID pvParam)
{
    int x = (*(int*)pvParam);
    if (x > 0)
        PrintPositiveNumbers();
    else
        PrintNegativeNumbers();
    return(0);
}

DWORD WINAPI Thread1(PVOID pvParam)
{
    int x = (*(int*)pvParam);
    EnterCriticalSection(&_cs);
    if (x > 0)
        PrintPositiveNumbers();
    else
        PrintNegativeNumbers();
    LeaveCriticalSection(&_cs);
    return(0);
}

DWORD WINAPI Thread2(PVOID pvParam)
{
    int x = (*(int*)pvParam);
    HANDLE _ev = OpenEvent(EVENT_ALL_ACCESS, FALSE, eventNameAuto);
    if (!_ev) return (-1);
    printf("Thread 2 (%d)...\n", x);
    WaitForSingleObject(_ev, INFINITE);
    if (x > 0)
        PrintPositiveNumbers();
    else
        PrintNegativeNumbers();
    SetEvent(_ev);
    return(0);
}

DWORD WINAPI Thread5(PVOID pvParam)
{
    int x = (*(int*)pvParam);
    HANDLE _ev = OpenEvent(EVENT_ALL_ACCESS, FALSE, eventNameManual);
    if (!_ev) return (-1);
    printf("Thread 5 (%d)...\n", x);
    WaitForSingleObject(_ev, INFINITE);
    ResetEvent(_ev);
    if (x > 0)
        PrintPositiveNumbers();
    else
        PrintNegativeNumbers();
    SetEvent(_ev);
    return(0);
}

bool IsPrime(int num) {
    if (num <= 1) {
        return false;
    }
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) {
            return false;
        }
    }
    return true;
}

DWORD WINAPI Thread4(PVOID pvParam)
{
    int num = 2;
    int primeCount = 0; // Лічильник простих чисел
    while (primeCount < 20) { 
        if (IsPrime(num)) {
            printf("%d ", num);
            primeCount++;
        }
        num++;

        // затримк, щоб не перевантажувати процесор
        Sleep(10); // Можна змінити затримку на більшу, якщо потрібно
    }
    return 0;
}

void PrintPositiveNumbers(int max)
{
    for (int i = 1; i < max; ++i)
        printf("%d ", i);
}

void PrintNegativeNumbers(int min)
{
    for (int i = -1; i > min; --i)
        printf("%d ", i);
}

void GoThreads(char choose)
{
    DWORD dwThreadID[2];
    HANDLE hThread[2];
    int params[2] = { 1, -1 };

    printf("Creating Threads %u\n", choose);
    for (unsigned short i = 0; i < 2; ++i)
        switch (choose)
        {
        case 5:
            hThread[i] = CreateThread(NULL, 0, Thread5, LPVOID(&params[i]), 0, &dwThreadID[i]);
            break;
        case 2:
            hThread[i] = CreateThread(NULL, 0, Thread2, LPVOID(&params[i]), 0, &dwThreadID[i]);
            break;
        case 1:
            hThread[i] = CreateThread(NULL, 0, Thread1, LPVOID(&params[i]), 0, &dwThreadID[i]);
            break;
        case 3:
            hThread[i] = CreateThread(NULL, 0, Thread0, LPVOID(&params[i]), 0, &dwThreadID[i]);
            if (i == 0) SetThreadPriority(hThread[i], THREAD_PRIORITY_BELOW_NORMAL);
            else SetThreadPriority(hThread[i], THREAD_PRIORITY_ABOVE_NORMAL);
            break;
        case 4:
            hThread[i] = CreateThread(NULL, 0, Thread4, LPVOID(&params[i]), 0, &dwThreadID[i]);
            break;
        case 0:
        default:
            hThread[i] = CreateThread(NULL, 0, Thread0, LPVOID(&params[i]), 0, &dwThreadID[i]);
            break;
        }
    
    printf("Waiting Threads %u\n", choose);
    WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
    printf("\nClosing Threads %u\n", choose);
    for (unsigned short i = 0; i < 2; ++i)
        CloseHandle(hThread[i]);
}
