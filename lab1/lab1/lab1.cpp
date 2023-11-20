#include <stdio.h>
#include <conio.h>
#include <windows.h>

int main(int argc, LPTSTR argv[])
{
	bool runOnce = false;
	DWORD pid = GetCurrentProcessId();
	/* ЗАВДАННЯ 1 - Створити три об’єкти ядра: мьютекс, семафор, таймер */
	HANDLE hMyMutex = CreateMutex(NULL, FALSE, L"lab1_mutex"); // іменований мьютекс
	HANDLE hMyTimer = CreateWaitableTimer(NULL, FALSE, L"lab1_timer"); // (іменований) таймер
	HANDLE hMySemaphore = CreateSemaphore(NULL, 2, 2, L"lab1_semaphore"); // (іменований) семафор
	
	/* ЗАВДАННЯ 2 - Відкрити всі три створені у Процесі 1 об’єкти ядра та довести, що вони є спільними для Процесів 1 та 2. */
	if (GetLastError() == ERROR_ALREADY_EXISTS) /* ЗАВДАННЯ 2.1 - використати функцію GetLastError() */
	{
		printf("PROC.%X: Mutex (0x%X), Timer (0x%X) & Semaphore (0x%X) - ERROR_ALREADY_EXISTS!\n", pid, hMyMutex, hMyTimer, hMySemaphore);
		runOnce = true;
		const unsigned short numObjs = 4; // кількість об'єктів, очікуваних від Процесу 1
		/* ЗАВДАННЯ 5 - Процес 5. Наслідує всі об’єкти ядра Процесу 1. */
		HANDLE hMyMutexInherited = (HANDLE)atoi(argv[1]); // отримуємо дескриптор успадкованого анонімного мьютекса
		printf("PROC.%X: %s (%d) : 0x%X\n", pid, argv[0], argc, hMyMutexInherited);
		HANDLE hObjs[numObjs] = { hMyMutex, hMyTimer, hMySemaphore, hMyMutexInherited };
		// Якщо об'єкти вже існують, тоді очікуємо на їх звільнення
		WaitForMultipleObjects(numObjs, hObjs, TRUE, INFINITE); /* ЗАВДАННЯ 2.2 - використати функцію WaitForSingleObject() - замінили аналогом WaitForMultipleObjects() */
		printf("PROC.%X: All named objects were opened and anonumous objects were inherited successfuly!\n", pid);
		CloseHandle(hMyMutexInherited);
	}

	if (hMyMutex && hMyTimer && hMySemaphore)
	{
		printf("PROC.%X: Mutex (0x%X), Timer (0x%X) & Semaphore (0x%X) have been created!\n", pid, hMyMutex, hMyTimer, hMySemaphore);
		if (!runOnce)
		{
			// Створюємо анонімний мьютекс для ілюстрації наслідування об1єктів для Завдання 5
			SECURITY_ATTRIBUTES sa;
			sa.nLength = sizeof(sa);
			sa.lpSecurityDescriptor = NULL;
			sa.bInheritHandle = TRUE/*FALSE*/; // TRUE - дозволяє успадкування дескрипторів об'єктів
			HANDLE hAnonymousMutex = CreateMutex(&sa, FALSE, NULL);
			if (hAnonymousMutex) printf("PROC.%X: Anonymous mutex (0x%X) created.\n", pid, hAnonymousMutex);
			// Запускаємо процес повторно (стартуємо процес завдань 2 та 5 як один процес) - ілюструємо спільне використання об'єктів
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;
			char cmdLn[255];
			sprintf_s(cmdLn, "%s %d\0", argv[0], hAnonymousMutex);
			if (CreateProcess(TEXT("LAB1.EXE"), cmdLn, NULL, NULL, TRUE/*FALSE*/, 0, NULL, NULL, &si, &pi)) /* ЗАВДАННЯ 5 - Процес 5. Наслідує всі об’єкти ядра Процесу 1. - TRUE дозволяє наслідування дескрипторів */
				printf("PROC.%X: New process (%X) started.\n", pid, pi.dwProcessId);
			else
				printf("PROC.%X: Failed to start new process.\n", pid);
			CloseHandle(hAnonymousMutex);
		}
		
		// Запускаємо таймер - робимо затримку, щоб показати спільне використання через очікування доступу до спільних об'єктів ядра
		LARGE_INTEGER dueTime;
		DWORD Count = 0;
		dueTime.QuadPart = -(LONGLONG)5000 * 1000;
		printf("PROC.%X: Timer start ticking.\n", pid);
		SetWaitableTimer(hMyTimer, &dueTime, 1000, NULL, &Count, TRUE);
		WaitForSingleObject(hMyTimer, INFINITE);
		printf("PROC.%X: Timer finished ticking.\n", pid);
		// Закриваємо дескриптори і вивільняємо об'єкти
		CloseHandle(hMyMutex);
		CloseHandle(hMyTimer);
		CloseHandle(hMySemaphore);
	}
	else printf("PROC.%X: Error: mutex OR timer OR sepaphore have not been created.\n", pid);

	// Запускаємо процес завдання 4 - він повинен запуститися лише один раз, хоча тут ми робимо 2 спроби його запустити
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	char cmdLn[255] = "LAB1_SINGLERUN\0";
	if (CreateProcess(TEXT("LAB1_SINGLERUN.EXE"), cmdLn, NULL, NULL, /*TRUE*/FALSE, 0, NULL, NULL, &si, &pi))
		printf("PROC.%X: New process (%X) started - Task 4.\n", pid, pi.dwProcessId);
	else
		printf("PROC.%X: Failed to start new process - Task 4.\n", pid);

	// Робимо штучну паузу у роботі
	printf("PROC.%X: PRESS ANY KEY TO EXIT...\n", pid);
	while (!_kbhit());

	//	system("pause");
	return 0;
}