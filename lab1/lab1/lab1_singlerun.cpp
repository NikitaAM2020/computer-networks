#include<stdio.h>
#include<windows.h>
#include<conio.h>

/* ЗАВДАННЯ 4 - Процес 4 – побудований таким чином, що на одному комп’ютері запускається лише один раз */

const char mutexName[] = "lab1_singlerun\0";

void main(int argc, char* argv[])
{
	DWORD pid = GetCurrentProcessId();

	HANDLE hMutex = CreateMutex(NULL, FALSE, mutexName);
	if (!hMutex || (GetLastError() == ERROR_ALREADY_EXISTS)) printf("Process 4 (%X): ONLY a single copy allowed to run!\n", pid);
	else if (!hMutex && (GetLastError() != NO_ERROR))  printf("Process 4 (%X): Failed to create mutex - Error #%d\n", pid, GetLastError());
	else
	{
		printf("Process 4 (%X): Started!\n", pid);
		printf("Process 4 (%X): Mutex created! Process single copy has been ran!\n", pid);
		// Використовуємо об'єкт таймер для організації паузи
		printf("Process 4 (%X): works...\n", pid);
		HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, "lab1_timer"); // таймер використовуємо той самий, що у процесі 1 - як іменований об'єкт
		if (GetLastError() == ERROR_ALREADY_EXISTS) // ЗАВДАННЯ 2.2 - використати функцію GetLastError()
		{
			printf("Process 4 (%X): Waiting for timer...\n", pid);
			WaitForSingleObject(hTimer, INFINITE); // ЗАВДАННЯ 2.2 - використати функцію WaitForSingleObject()
		}
		printf("Process 4 (%X): Waiting for timer...\n", pid);
		// Запускаємо процес повторно (стартуємо Процес 2) - ілюструємо спільне використання об'єктів
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi;
		char cmdLn[255];
		sprintf_s(cmdLn, "%s\0", argv[0]);
		if (CreateProcess(TEXT("LAB1_SINGLERUN.EXE"), cmdLn/*TEXT("lab1_singlerun.exe")*/, NULL, NULL, /*TRUE*/FALSE, 0, NULL, NULL, &si, &pi))
			printf("Process 4 (%X): Run a copy...\n", pid);
		else
			printf("Process 4 (%X): Failed to start new process.\n", pid);
		// Запускаємо таймер - робимо затримку, щоб показати спільне використання через очікування доступу до спільних об'єктів ядра
		printf("Process 4 (%X): still works...\n", pid);
		LARGE_INTEGER dueTime;
		DWORD Count = 0;
		dueTime.QuadPart = -(LONGLONG)10000 * 1000;
		SetWaitableTimer(hTimer, &dueTime, 1000, NULL, &Count, TRUE);
		WaitForSingleObject(hTimer, INFINITE);
		// Закриваємо дескриптори і вивільняємо об'єкти
		CloseHandle(hTimer);
		printf("Process 4 (%X): finished!\n", pid);
		// Пауза, щоб побачити результати на екрані
		system("pause");
		CloseHandle(hMutex);
	}
}