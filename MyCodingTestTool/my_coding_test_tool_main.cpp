#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <psapi.h>
#include <vector>
#include <chrono>

using MemoryUsage = unsigned long long int;

const std::string USER_EXECUTABLE_FILE
= "./temp/user_executable.exe";

const std::string OUTPUT_TEXT_FILE
= "./temp/output.txt";

// 테스트 케이스 구조체
struct TestCase {
    std::string inputFileName;
    std::string solutionFileName;

    TestCase(std::string& inputFileName, std::string& solutionFileName)
        :inputFileName(inputFileName), solutionFileName(solutionFileName) {}
};


MemoryUsage getMemoryUsage(HANDLE hProcess) {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS_EX);
    MemoryUsage result = 0; // Bytes

	// 프로세스 메모리 정보 가져오기
	if (GetProcessMemoryInfo(
        hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))
		) {
        result = pmc.PrivateUsage; // Bytes
	}
	else {
		std::cerr << "Failed to get process memory info." << std::endl;
	}

    return result;
}

bool run(
    const TestCase& tc, 
    double& executionTime, 
    MemoryUsage& memoryUsage) 
{
    std::string command = USER_EXECUTABLE_FILE + " < " + tc.inputFileName
        + " > " + tc.solutionFileName;

    // startupinfo
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    // process info
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    // init
	executionTime = 0;
	memoryUsage = 0;
    bool bSuccess = true;

    // start timer
    auto start = std::chrono::steady_clock::now();

    // 사용자가 제출한 코드를 별도의 프로세스로 실행
	if (CreateProcess(
		NULL,           // No module name
		(LPWSTR)command.c_str(),        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,           // Pointer to STARTUPINFO structure
		&pi            // Pointer to PROCESS_INFORMATION structure
	)) {
		// Wait until child process exits
		WaitForSingleObject(pi.hProcess, INFINITE);

		auto end = std::chrono::steady_clock::now();
		executionTime = std::chrono::duration<double, std::milli>(end - start).count();

		// 메모리 사용량 측정
		memoryUsage = getMemoryUsage(pi.hProcess);
    }
    else {
        std::cerr << "CreateProcess failed" << std::endl;
        bSuccess = false;
    }

    // terminate
    TerminateProcess(pi.hProcess,PROCESS_TERMINATE);
    TerminateProcess(pi.hThread, THREAD_TERMINATE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
    return bSuccess;
}

bool compareTextFile(const std::string& a, const std::string& b) {
    // TODO
    return true;
}

int main() {
    std::string USER_CODE_FILE;
    std::string TEST_CASE_DIRECTORY;
    std::vector<TestCase> TEST_CASES;

    // 코드 주소를 입력받음
    std::cin >> USER_CODE_FILE;

    // 테스트 케이스 주소를 입력받음
    std::cin >> TEST_CASE_DIRECTORY;

    // 각 테스트 케이스 인풋 아웃풋 쌍을 찾음
    // TODO

    // 코드를 컴파일하여 실행 가능한 프로그램으로 만듦
    std::string command 
        = "g++ " + USER_CODE_FILE + " -o " + USER_EXECUTABLE_FILE;
    if (system(command.c_str()) != 0) {
        std::cerr << "Error: Compilation failed." << std::endl;
        return 1;
    }


    // 각 테스트 케이스를 입력하여 사용자의 코드가 올바른 결과를 출력하는지 확인
    for (const auto& testCase : TEST_CASES) {
        double excutionTime = 0.0;
		MemoryUsage memoryUsage = 0;
        bool bSuccess;

        // 사용자의 프로그램에 테스트 케이스를 입력하고 실행
        bSuccess = run(testCase, excutionTime, memoryUsage);
        if (bSuccess == false) {
            std::cerr << "fail to run test case: " 
                << testCase.inputFileName << std::endl;
            return 1;
        }

        // 사용자의 프로그램이 올바른 출력을 생성하는지 확인
        bSuccess = compareTextFile(testCase.solutionFileName, OUTPUT_TEXT_FILE);

        if (bSuccess) {
            std::cout << "맞았습니다!";
        }
        else {
            std::cout << "틀렸습니다.";
        }
        std::cout << "\t" << excutionTime << "\t" << memoryUsage << std::endl;
    }

    return 0;
}

