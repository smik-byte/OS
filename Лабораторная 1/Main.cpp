#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;

struct Employee
{
    int id;
    char name[10];
    double workHours;
};

void replaceString(string& str, const string& stringWhichReplace, const string& stringInstead)
{
    if (stringWhichReplace.empty())
    {
        return;
    }

    size_t startPos = 0;
    while ((startPos = str.find(stringWhichReplace, startPos)) != string::npos)
    {
        str.replace(startPos, stringWhichReplace.length(), stringInstead);
        startPos += stringInstead.length();
    }
}

int main(int argc, char* argv[])
{
    string binaryFileName;
    int recordCount;

    setlocale(LC_ALL, "Rus");

    cout << "Enter the name of the binary file: ";
    cin >> binaryFileName;
    cout << "Enter the number of records: ";
    cin >> recordCount;

    string appPath = argv[0]; // directory where all .exe files
    replaceString(appPath, "Main.exe", "");

    // инфа о том, как запускать новый процесс (размеры окна, дескрипторы ввода/вывода и тд)
    STARTUPINFOA startupInfo; // STARTUPINFOA - версия для ANSI-символов (строки в 1-днобайтовом формате)
    PROCESS_INFORMATION processInfo; // информация о новом процессе и его главном потоке. Она заполняется функцией CreateProcessA после успешного создания процесса.
    ZeroMemory(&startupInfo, sizeof(STARTUPINFOA)); // очищает выделенную память под startupInfo , заполняя все её байты нулями (чтоб все данные были по умолчанию в стракт)
    startupInfo.cb = sizeof(STARTUPINFOA);// поле "cb" содержит инфу о размере struct, нужно установить вручную Т.к. CreateProcessA нужно знать какого размера данные получать


    // функция WinAPI, которая создает новый процесс и, при необходимости, его главный поток (thread).
    // Она запускает исполняемый файл Creator.exe с определенными параметрами.
    if (CreateProcessA((appPath + "Creator.exe").c_str(), // путь к исполняемому файлу, который нужно запустить
        // LPSTR - тип данных, который представляет указатель на строку символов в кодировке ANSI
        (LPSTR)((appPath + "Creator.exe " + binaryFileName + " " + to_string(recordCount)).c_str()), // командная строка, передаваемая новому процессу
        NULL, // lpProcessAttributes, атрибуты безопасности не настраиваются
        NULL, // lpThreadAttributes, атрибуты безопасности для main thread
        FALSE, // bInheritHandles, будут ли дескрипторы унаследованиы новым процессом
        CREATE_NEW_CONSOLE, // dwCreationFlags - флаг для создания процееса (можно NULL)
        NULL, // lpEnvironment - среда окружения для new process, Null = новый процесс наследует environment родительского процесса
        NULL, // lpCurrentDirectory - рабочая директория, процесс будет запущен там же где и род. процесс
        &startupInfo, //lpStartupInfo - указатель на структуру стартапинфо
        &processInfo)) // lpProcessInformation - указатель на структуру процессинфо
    {
        WaitForSingleObject(processInfo.hProcess, INFINITE);
        CloseHandle(processInfo.hThread); // hThread - дескриптор главного потока нового процесса
        CloseHandle(processInfo.hProcess);// hProcess - дескриптор процесса, который был создан
    }
    else
    {
        cerr << "Failed to start Creator.exe" << endl;
        return 1;
    }

    ifstream binaryFile(binaryFileName, ios::in | ios::binary);
    Employee employee;

    cout << "Contents of the binary file:" << endl;
    while (binaryFile.read(reinterpret_cast<char*>(&employee), sizeof(employee)))
    {
        cout << employee.id << " " << employee.name << " " << employee.workHours << "\n";
    }
    binaryFile.close();

    string reportFileName;
    double salary;

    cout << endl << "Enter the name of the report file: ";
    cin >> reportFileName;
    cout << "Enter the salary: ";
    cin >> salary;

    ZeroMemory(&startupInfo, sizeof(STARTUPINFOA));
    if (CreateProcessA((appPath + "Reporter.exe").c_str(),
        (LPSTR)((appPath + "Reporter.exe " + binaryFileName + " " + reportFileName + " " + to_string(salary)).c_str()),
        NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &processInfo))
    {
        WaitForSingleObject(processInfo.hProcess, INFINITE);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
    }
    else
    {
        cerr << "Failed to start Reporter.exe" << endl;
        return 1;
    }

    ifstream reportFile(reportFileName);
    string line;

    if (reportFile.is_open())
    {
        cout << "Report contents:" << endl;
        while (getline(reportFile, line))
        {
            cout << line << endl;
        }
        reportFile.close();
    }
    else
    {
        cerr << "Unable to open report file: " << reportFileName << endl;
    }

    return 0;
}
