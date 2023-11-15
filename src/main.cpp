#include <iostream>
#include <thread>
#include <random>
#include <winsock2.h>
#include <windows.h>
#include "argparse.hpp"
#pragma comment(lib, "ws2_32.lib")

using namespace std;
using namespace argparse;

string VERSION = "0.1.2.68";
string SPRITES = R"(-\|/)";
bool useAnimations = true;

SOCKET scpsocket;
SOCKET udpsocket;

int len(string str) {
    int length = 0;
    for (int i = 0; str[i] != '\0'; i++) length++;
    return length;
}

vector<string> split(const string &s, char delimiter) {
    vector<string> tokens;
    istringstream tokenStream(s);
    string token;
    while (getline(tokenStream, token, delimiter)) tokens.push_back(token);
    return tokens;
}

string inttimesstr(int num, char ch) {
    string result;
    for (int i = 0; i <= num; i++) result += ch;
    return result;
}

void SendUDPConnection(sockaddr_in clientService, int port) {
    const char *sendbuf = to_string(port).c_str();
    clientService.sin_port = htons(port);
    if (connect(udpsocket, (SOCKADDR *) & clientService, sizeof (clientService)) == SOCKET_ERROR) return;
    send(udpsocket, sendbuf, (int)strlen(sendbuf), 0);
}

void GetTCPConnection(sockaddr_in clientService, int port) {
    char *sendbuf = (char*)"Do you have data for me?\n";
    int iResult;

    char recvbuf[1024];
    int recvbuflen = 1024;

    clientService.sin_port = htons(port);

    iResult = connect(scpsocket, (SOCKADDR*) &clientService, sizeof(clientService));
    if (iResult == SOCKET_ERROR) return;

    iResult = send(scpsocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) return;

    iResult = shutdown(scpsocket, SD_SEND);
    if (iResult == SOCKET_ERROR) return;

    do {
        iResult = recv(scpsocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            cout << "[!] Response: " << recvbuf << endl;
            // Optionally, you can continue sending additional requests here if needed.
            // Example:
            // iResult = send(scpsocket, sendbuf, (int)strlen(sendbuf), 0);
            // if (iResult == SOCKET_ERROR) return;
        }
    } while(iResult >= 0); // Modify the loop condition to continue even if recv returns 0
}

void cleanUpBeforeExit() {
    closesocket(scpsocket);
    closesocket(udpsocket);
    WSACleanup();
}

int main(int argc, char* argv[]) {
    // Hardcoded logo ;p
    cout << R"(
  ___                       .___                                          _____       /\
 / _ \_/\    ________ __  __| _/____   _______  _____            ________/ ____\     / /
 \/ \___/   /  ___/  |  \/ __ |/  _ \  \_  __ \/     \    ______ \_  __ \   __\     / /
            \___ \|  |  / /_/ (  <_> )  |  | \/  Y Y  \  /_____/  |  | \/|  |      / /
           /____  >____/\____ |\____/   |__|  |__|_|  /           |__|   |__|     / /
                \/           \/                     \/                            \/
)";
    // Author and aminations
    string MESSAGE = "PortKnocking tool by Jerzy W (https://github.com/JamJestJerzy)";
    if (useAnimations) {
        string ASSEMBLED; int y = 0;
        for (int i = 0; i < len(MESSAGE); i++) {
            if (y > 4) y = 0;
            ASSEMBLED += MESSAGE[i];
            cout << '\r' << ASSEMBLED << inttimesstr((len(MESSAGE) - i), ' ') << inttimesstr(10, ' ') << SPRITES[y];
            this_thread::sleep_for(chrono::milliseconds(10));
            if (i % 5 == 0) y++;
        } cout << "\b" << " " << endl;
    } else cout << MESSAGE << endl;


    // Agrs handler (parser)
    ArgumentParser program("PortKnocker", VERSION);

    // Optional
    program.add_argument("--rate").help("How many times a second send packets.").metavar("TIMEOUT").default_value(1000);

    // Positionals
    program.add_argument("address").help("IP (hostname) you want to kick the door of.\t\t(e.g. 52.0.14.116)").metavar("IP");
    program.add_argument("target").help("Range of ports you want to knock to.\t\t\t(e.g. 100-825)").metavar("PORT_RANGE");
    program.add_argument("destination").help("Final port you want to open connection with.\t\t(e.g. 22)").metavar("PORT");

    // Flags
    program.add_argument("singlethread").help("Disabled usage of multiple threads").flag();

    // Epilog
    program.add_epilog("Example usage:\n\tPortKnocker-" + VERSION + ".exe 10.0.43.117 8000-8100 6060");

    try { program.parse_args(argc, argv); }
    catch (const exception& err) { cerr << err.what() << endl; cerr << program; return 1; }

    // Defining agrs-initialized variables
    auto address = program.get<string>("address");
    auto target = program.get<string>("target");
    auto destination = program.get<string>("destination");
    auto rate = program.get<int>("--rate");
    bool useSingleThread = (program["--singlethread"] == true);

    auto addressCharArray = new char[len(address) + 1]; strcpy(addressCharArray, address.c_str());
    vector<string> targetPortArray = split(target, '-');
    int minPort = stoi(targetPortArray[0]);
    int maxPort = stoi(targetPortArray[1]);
    int destinationPort = stoi(destination);
    // End of defining args


    // Initializing webSocket
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Failed to initialize Winsock." << endl;
        return 1;
    }

    scpsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    udpsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpsocket == INVALID_SOCKET || scpsocket == INVALID_SOCKET) {
        cerr << "Error creating socket" << endl;
        WSACleanup();
        return 1;
    }

    // Defining variables for sockets
    sockaddr_in sockaddrIn{};
    sockaddrIn.sin_family = AF_INET;
    sockaddrIn.sin_addr.s_addr = inet_addr(addressCharArray);

    vector<thread> threads;

    for (int a = minPort; a <= maxPort; a++) {
        for (int b = minPort; b <= maxPort; b++) {
            for (int c = minPort; c <= maxPort; c++) {
                if (!useSingleThread) {
                    threads.emplace_back(SendUDPConnection, sockaddrIn, a);
                    threads.emplace_back(SendUDPConnection, sockaddrIn, b);
                    threads.emplace_back(SendUDPConnection, sockaddrIn, c);

                    threads.emplace_back(GetTCPConnection, sockaddrIn, destinationPort);
                } else {
                    SendUDPConnection(sockaddrIn, a);
                    SendUDPConnection(sockaddrIn, b);
                    SendUDPConnection(sockaddrIn, c);

                    GetTCPConnection(sockaddrIn, destinationPort);
                }
            }
        }
    }

    if (!useSingleThread) {
        // Join threads after creating them all
        for (auto& thread : threads) {
            thread.join();
        }
        threads.clear();
    }

    cleanUpBeforeExit();

    return 0;
}

