#include <iostream>
#include <thread>
#include <random>
#include <winsock2.h>
#include "argparse.hpp"
#pragma comment(lib, "ws2_32.lib")

using namespace std;
using namespace argparse;

string VERSION = "0.1.2.1";
string SPRITES = R"(-\|/)";
bool useAnimations = true;
bool singleResponse = false;

int len(string str) {
    int length = 0;
    for (int i = 0; str[i] != '\0'; i++) length++;
    return length;
}

std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream tokenStream(s);
    std::string token;
    while (std::getline(tokenStream, token, delimiter)) tokens.push_back(token);
    return tokens;
}

std::string inttimesstr(int num, char ch) {
    std::string result;
    for (int i = 0; i <= num; i++) result += ch;
    return result;
}

std::vector<int> range(int start, int stop, int step = 1) {
    std::vector<int> result;
    for (int i = start; (step > 0) ? (i < stop) : (i > stop); i += step) result.push_back(i);
    return result;
}

void sendToPort(int port, const char* message, int udpsocket, int scpsocket, sockaddr_in sockaddrIn, int destinationPort) {
    sockaddrIn.sin_port = htons(port);
    sendto(udpsocket, message, len(message), 0, reinterpret_cast<sockaddr*>(&sockaddrIn), sizeof(sockaddrIn));

    sockaddrIn.sin_port = htons(destinationPort);
    int connection = connect(scpsocket, (struct sockaddr*)&sockaddrIn, sizeof(sockaddrIn));
    char buffer[1024];
    int bytesReceived = recv(scpsocket, buffer, sizeof(buffer), 0);

    if (bytesReceived > 0) {
        cout << "[!] Response:" << buffer << endl;
        if (singleResponse) exit(0);
    }
}

int main(int argc, char* argv[]) {
    cout << R"(
  ___                       .___                                          _____       /\
 / _ \_/\    ________ __  __| _/____   _______  _____            ________/ ____\     / /
 \/ \___/   /  ___/  |  \/ __ |/  _ \  \_  __ \/     \    ______ \_  __ \   __\     / /
            \___ \|  |  / /_/ (  <_> )  |  | \/  Y Y  \  /_____/  |  | \/|  |      / /
           /____  >____/\____ |\____/   |__|  |__|_|  /           |__|   |__|     / /
                \/           \/                     \/                            \/
)";
    string MESSAGE = "PortKnocking tool by Jerzy W (https://github.com/JamJestJerzy)";
    if (useAnimations) {
        string ASSEMBLED; int y = 0;
        for (int i = 0; i < len(MESSAGE); i++) {
            if (y > 4) y = 0;
            ASSEMBLED += MESSAGE[i];
            cout << '\r' << ASSEMBLED << inttimesstr((len(MESSAGE) - i), ' ') << inttimesstr(10, ' ') << SPRITES[y];
            this_thread::sleep_for(std::chrono::milliseconds(10));
            if (i % 5 == 0) y++;
        } cout << "\b" << " " << endl;
    } else cout << MESSAGE << endl;
    ArgumentParser program("PortKnocker", VERSION);

    program.add_argument("--rate").help("How many times a second send packets.").metavar("TIMEOUT").default_value(60);
    program.add_argument("--single").help("Stops after recieving response").flag();

    program.add_argument("address").help("IP (hostname) you want to kick the door of.\t\t(e.g. 52.0.14.116)").metavar("IP");
    program.add_argument("target").help("Range of ports you want to knock to.\t\t\t(e.g. 100-825)").metavar("PORT_RANGE");
    program.add_argument("destination").help("Final port you want to open connection with.\t\t(e.g. 22)").metavar("PORT");

    program.add_epilog("Example usage:\n\tPortKnocker-" + VERSION + ".exe 10.0.43.117 8000-8100 6060");

    try { program.parse_args(argc, argv); }
    catch (const std::exception& err) { std::cerr << err.what() << std::endl; std::cerr << program; return 1; }

    auto address = program.get<string>("address");
    auto target = program.get<string>("target");
    auto destination = program.get<string>("destination");
    auto rate = program.get<int>("--rate");
    singleResponse = (program["--single"] == true);

    auto addressCharArray = new char[len(address) + 1]; strcpy(addressCharArray, address.c_str());
    std::vector<std::string> targetPortArray = split(target, '-');
    int minPort = stoi(targetPortArray[0]);
    int maxPort = stoi(targetPortArray[1]);
    int destinationPort = stoi(destination);


    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Failed to initialize Winsock." << endl;
        return 1;
    }

    SOCKET udpsocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpsocket == INVALID_SOCKET) {
        cerr << "Error creating socket" << endl;
        WSACleanup();
        return 1;
    }

    SOCKET scpsocket = socket(AF_INET, SOCK_STREAM, 0);
    if (scpsocket == INVALID_SOCKET) {
        cerr << "Error creating socket." << endl;
        WSACleanup();
        return 1;
    }

    const char* message = "Hi there.\n";
    sockaddr_in sockaddrIn{};
    sockaddrIn.sin_family = AF_INET;
    sockaddrIn.sin_addr.s_addr = inet_addr(addressCharArray);

    char buffer[1024];
    int bytesReceived;

    vector<thread> threads;

    for (int a : range(minPort, maxPort+1, 1)) {
        for (int b : range(minPort, maxPort+1, 1)) {
            for (int c : range(minPort, maxPort+1, 1)) {
                threads.emplace_back(sendToPort, a, message, udpsocket, scpsocket, sockaddrIn, destinationPort);
                threads.emplace_back(sendToPort, b, message, udpsocket, scpsocket, sockaddrIn, destinationPort);
                threads.emplace_back(sendToPort, c, message, udpsocket, scpsocket, sockaddrIn, destinationPort);
                this_thread::sleep_for(chrono::milliseconds(1000/rate));
            }
        }
    }

    for (auto& thread : threads) {
        thread.join();
    }

    closesocket(scpsocket);
    closesocket(udpsocket);
    WSACleanup();

    return 0;
}


