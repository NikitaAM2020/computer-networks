#include <iostream>
#include <windows.h>
#include <string>

// Глобальні змінні для зберігання дескрипторів об'єктів ядра
HANDLE hMutex = nullptr;
HANDLE hSemaphore = nullptr;
HANDLE hTimer = nullptr;

// Глобальні змінні для імен об'єктів
const wchar_t* mutexName = L"MyMutex";
const wchar_t* semaphoreName = L"MySemaphore";
const wchar_t* timerName = L"MyTimer";

// Функція для створення об'єктів ядра в процесі 1
void Process1()
{
    std::wcout << L"Process 1 is initializing objects" << std::endl;

    hMutex = CreateMutexW(NULL, FALSE, mutexName);
    if (hMutex)
    {
        std::wcout << L"\tMutex created successfully!" << std::endl;
    }
    else
    {
        std::wcerr << L"\tFailed to create the mutex! Error code: " << GetLastError() << std::endl;
        return;
    }

    hSemaphore = CreateSemaphoreW(NULL, 1, 1, semaphoreName);
    if (hSemaphore)
    {
        std::wcout << L"\tSemaphore created successfully!" << std::endl;
    }
    else
    {
        std::wcerr << L"\tFailed to create the semaphore! Error code: " << GetLastError() << std::endl;
        CloseHandle(hMutex);
        return;
    }

    hTimer = CreateWaitableTimerW(NULL, FALSE, timerName);
    if (hTimer)
    {
        std::wcout << L"\tTimer created successfully!" << std::endl;
    }
    else
    {
        std::wcerr << L"\tFailed to create the timer! Error code: " << GetLastError() << std::endl;
        CloseHandle(hMutex);
        CloseHandle(hSemaphore);
        return;
    }

    // Очікувати певний час для запуску процесу 2 (або виконайте його безпосередньо тут)
    std::wcout << L"Process 1 is waiting..." << std::endl;
    Sleep(5000);
}

// Функція для відкриття об'єктів ядра в процесі 2
void Process2()
{
    std::wcout << L"Process 2 started" << std::endl;

    HANDLE hMutex = OpenMutexW(SYNCHRONIZE, FALSE, mutexName);
    HANDLE hSemaphore = OpenSemaphoreW(SYNCHRONIZE, FALSE, semaphoreName);
    HANDLE hTimer = OpenWaitableTimerW(SYNCHRONIZE, FALSE, timerName);

    if (hMutex && hSemaphore && hTimer)
    {
        std::wcout << L"\tObjects opened successfully in Process 2!" << std::endl;

        // Використовуємо WaitForSingleObject для очікування на Mutex
        DWORD waitResult = WaitForSingleObject(hMutex, INFINITE);
        if (waitResult == WAIT_OBJECT_0)
        {
            std::wcout << L"\tMutex acquired successfully in Process 2!" << std::endl;
            // Тут можна виконати дії, які потрібно виконати, коли Mutex доступний

            // Після завершення роботи з Mutex вивільняємо його
            ReleaseMutex(hMutex);
        }
        else
        {
            DWORD lastError = GetLastError();
            std::wcerr << L"\tFailed to acquire the Mutex in Process 2! Error code: " << lastError << std::endl;
        }
    }
    else
    {
        DWORD lastError = GetLastError();
        std::wcerr << L"\tFailed to open one or more objects in Process 2! Error code: " << lastError << std::endl;
    }
}

// Функція для демонстрації роботи семафора в процесі 3
void Process3()
{
    std::wcout << L"Process 3 started" << std::endl;

    // Відкрити семафор, який був створений в процесі 1
    if (hSemaphore)
    {
        HANDLE hObjects[2];  // Масив для зберігання дескрипторів об'єктів
        hObjects[0] = hSemaphore;
        hObjects[1] = hMutex;

        // Очікувати на декілька об'єктів, використовуючи WaitForMultipleObjects
        DWORD waitResult = WaitForMultipleObjects(2, hObjects, FALSE, INFINITE);

        if (waitResult == WAIT_OBJECT_0) // Семафор був затриманий
        {
            std::wcout << L"\tSemaphore acquired successfully in Process 3!" << std::endl;
            // Тут можна виконати дії, які потрібно виконати, коли семафор доступний
        }
        else if (waitResult == WAIT_OBJECT_0 + 1) // Мютекс був затриманий
        {
            std::wcout << L"\tMutex signaled successfully in Process 3!" << std::endl;
            // Тут можна виконати дії, пов'язані з мютексом
        }
        else
        {
            DWORD lastError = GetLastError();
            std::wcerr << L"\tWait for multiple objects failed in Process 3! Error code: " << lastError << std::endl;
        }

        // Після використання об'єктів, їх слід вивільнити
        ReleaseSemaphore(hSemaphore, 1, NULL);
        std::wcout << L"\tSemaphore released in Process 3!" << std::endl;
        ReleaseMutex(hMutex);
        std::wcout << L"\tMutex released in Process 3!" << std::endl;
    }
    else
    {
        DWORD lastError = GetLastError();
        std::wcerr << L"\tFailed to open the semaphore or mutex in Process 3! Error code: " << lastError << std::endl;
    }
}

// Функція для реалізації "Процесу 4" (запускатиметься лише один раз на одному комп'ютері)
void Process4()
{
    const wchar_t* sharedMemoryName = L"Global\\MyNewSharedMemory";

    // Перевірка існування глобальної змінної (shared memory)
    HANDLE hSharedMemory = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, sharedMemoryName);

    if (hSharedMemory == NULL)
    {
        // Глобальна змінна не існує, створюємо її
        hSharedMemory = CreateFileMapping(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            sizeof(bool),
            sharedMemoryName
        );

        if (hSharedMemory == NULL)
        {
            std::wcerr << L"Failed to create shared memory! Error code: " << GetLastError() << std::endl;
            return;
        }
    }
    else
    {
        // Глобальна змінна існує вже
        CloseHandle(hSharedMemory);
        std::wcout << L"Process 4 is already running on this computer." << std::endl;
        return;
    }

    // Перевірка, чи глобальна змінна була успішно створена
    if (hSharedMemory)
    {
        // Отримуємо доступ до глобальної змінної (shared memory)
        bool* isProcess4Running = (bool*)MapViewOfFile(
            hSharedMemory,
            FILE_MAP_READ | FILE_MAP_WRITE,
            0,
            0,
            sizeof(bool)
        );

        if (isProcess4Running == NULL)
        {
            std::wcerr << L"Failed to access shared memory! Error code: " << GetLastError() << std::endl;
            CloseHandle(hSharedMemory);
            return;
        }

        // Позначаємо, що процес 4 запущений
        *isProcess4Running = true;

        // Виконуємо код для "Процесу 4"
        std::wcout << L"Process 4 started" << std::endl;

        // Звільнюємо ресурси
        UnmapViewOfFile(isProcess4Running);
        CloseHandle(hSharedMemory);
    }
    else
    {
        std::wcerr << L"Failed to create or open shared memory!" << std::endl;
    }
}


// Функція для відкриття об'єктів ядра в процесі 5 та їх наслідування з процесу 1
void Process5()
{
    HANDLE hMutex5 = nullptr;
    HANDLE hSemaphore5 = nullptr;
    HANDLE hTimer5 = nullptr;
    std::wcout << L"Process 5 started" << std::endl;

    // Отримуємо дескриптори об'єктів з "Процесу 1" та наслідуємо їх
    if (hMutex)
    {
        if (DuplicateHandle(GetCurrentProcess(), hMutex, GetCurrentProcess(), &hMutex5, 0, FALSE, DUPLICATE_SAME_ACCESS))
        {
            std::wcout << L"\tMutex inherited successfully in Process 5!" << std::endl;
        }
        else
        {
            DWORD lastError = GetLastError();
            std::wcerr << L"\tFailed to inherit the mutex in Process 5! Error code: " << lastError << std::endl;
        }
    }

    if (hSemaphore)
    {
        if (DuplicateHandle(GetCurrentProcess(), hSemaphore, GetCurrentProcess(), &hSemaphore5, 0, FALSE, DUPLICATE_SAME_ACCESS))
        {
            std::wcout << L"\tSemaphore inherited successfully in Process 5!" << std::endl;
        }
        else
        {
            DWORD lastError = GetLastError();
            std::wcerr << L"\tFailed to inherit the semaphore in Process 5! Error code: " << lastError << std::endl;
        }
    }

    if (hTimer)
    {
        if (DuplicateHandle(GetCurrentProcess(), hTimer, GetCurrentProcess(), &hTimer5, 0, FALSE, DUPLICATE_SAME_ACCESS))
        {
            std::wcout << L"\tTimer inherited successfully in Process 5!" << std::endl;
        }
        else
        {
            DWORD lastError = GetLastError();
            std::wcerr << L"\tFailed to inherit the timer in Process 5! Error code: " << lastError << std::endl;
        }
    }

    // Закриваємо дескриптори об'єктів після використання
    if (hMutex5)
        CloseHandle(hMutex5);
    if (hSemaphore5)
        CloseHandle(hSemaphore5);
    if (hTimer5)
        CloseHandle(hTimer5);

    // Закриваємо дескриптори об'єктів після використання
    if (hMutex)
        CloseHandle(hMutex);
    if (hSemaphore)
        CloseHandle(hSemaphore);
    if (hTimer)
        CloseHandle(hTimer);
}

int main()
{
    Process1();
    Process2();
    Process3();
    Process4();
    Process5();

    std::wcout << L"End of the lab1!" << std::endl;
    return 0;
}
