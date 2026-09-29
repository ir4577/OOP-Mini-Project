#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    using SocketType = SOCKET;
    #define CLOSE_SOCKET(s) closesocket(s)
    #define IS_INVALID_SOCKET(s) ((s) == INVALID_SOCKET)
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    using SocketType = int;
    #define CLOSE_SOCKET(s) close(s)
    #define IS_INVALID_SOCKET(s) ((s) < 0)
#endif

#include "playlist.hpp"

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 8192;

Playlist myPlaylist;

void serveFile(SocketType clientSocket, const std::string& fileName, const std::string& contentType) {
    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        std::string notFound = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        send(clientSocket, notFound.c_str(), static_cast<int>(notFound.length()), 0);
        return;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string fileBuffer = ss.str();

    std::ostringstream header;
    header << "HTTP/1.1 200 OK\r\n"
           << "Content-Type: " << contentType << "\r\n"
           << "Content-Length: " << fileBuffer.length() << "\r\n"
           << "Access-Control-Allow-Origin: *\r\n\r\n";

    std::string headerStr = header.str();
    send(clientSocket, headerStr.c_str(), static_cast<int>(headerStr.length()), 0);
    send(clientSocket, fileBuffer.c_str(), static_cast<int>(fileBuffer.length()), 0);
}

void sendPlaylistJSON(SocketType clientSocket) {
    std::ostringstream json;
    const auto& songs = myPlaylist.getSongs();
    const Song* currentSong = myPlaylist.getCurrentSong();

    json << "{\"size\":" << myPlaylist.getSize()
         << ",\"loop_mode\":" << (myPlaylist.getLoopMode() ? 1 : 0)
         << ",\"songs\":[";

    for (size_t i = 0; i < songs.size(); ++i) {
        bool isCurrent = (currentSong != nullptr && &songs[i] == currentSong);
        if (i > 0) json << ",";
        json << "{"
             << "\"id\":" << i << ","
             << "\"title\":\"" << songs[i].title << "\","
             << "\"artist\":\"" << songs[i].artist << "\","
             << "\"duration\":" << songs[i].duration << ","
             << "\"filename\":\"" << songs[i].filename << "\","
             << "\"is_current\":" << (isCurrent ? 1 : 0)
             << "}";
    }

    json << "]}";

    std::string body = json.str();
    std::ostringstream header;
    header << "HTTP/1.1 200 OK\r\n"
           << "Content-Type: application/json\r\n"
           << "Content-Length: " << body.length() << "\r\n"
           << "Access-Control-Allow-Origin: *\r\n\r\n";

    std::string headerStr = header.str();
    send(clientSocket, headerStr.c_str(), static_cast<int>(headerStr.length()), 0);
    send(clientSocket, body.c_str(), static_cast<int>(body.length()), 0);
}

void parseAndAddTrack(const std::string& requestBody) {
    std::string title = "New Track", artist = "Unknown", filename = "";
    int duration = 180;

    std::stringstream ss(requestBody);
    std::string pair;
    while (std::getline(ss, pair, '&')) {
        size_t eqPos = pair.find('=');
        if (eqPos != std::string::npos) {
            std::string key = pair.substr(0, eqPos);
            std::string val = pair.substr(eqPos + 1);

            if (key == "title") title = val;
            else if (key == "artist") artist = val;
            else if (key == "duration") {
                try { duration = std::stoi(val); } catch (...) {}
            }
            else if (key == "filename") filename = val;
        }
    }
    myPlaylist.addSong(title, artist, duration, filename);
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed.\n";
        return 1;
    }
#endif

    myPlaylist.loadFromFile("library.txt");
    if (myPlaylist.getSize() == 0) {
        myPlaylist.addSong("SoundHelix Track 1", "SoundHelix", 372, "https://www.soundhelix.com/examples/mp3/SoundHelix-Song-1.mp3");
        myPlaylist.addSong("SoundHelix Track 2", "SoundHelix", 423, "https://www.soundhelix.com/examples/mp3/SoundHelix-Song-2.mp3");
        myPlaylist.addSong("SoundHelix Track 3", "SoundHelix", 344, "https://www.soundhelix.com/examples/mp3/SoundHelix-Song-3.mp3");
    }

    SocketType serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
    listen(serverSocket, 5);

    std::cout << "========================================================\n";
    std::cout << " ♪ C++ SERVER RUNNING! Open in browser: http://localhost:" << PORT << "\n";
    std::cout << "========================================================\n";

    while (true) {
        sockaddr_in clientAddr{};
        socklen_t clientAddrLen = sizeof(clientAddr);
        SocketType clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);

        if (IS_INVALID_SOCKET(clientSocket)) continue;

        char buffer[BUFFER_SIZE] = {0};
        recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        std::string req(buffer);

        if (req.find("GET /api/data") != std::string::npos) {
            sendPlaylistJSON(clientSocket);
        } else if (req.find("GET /api/next") != std::string::npos) {
            myPlaylist.nextSong();
            sendPlaylistJSON(clientSocket);
        } else if (req.find("GET /api/prev") != std::string::npos) {
            myPlaylist.prevSong();
            sendPlaylistJSON(clientSocket);
        } else if (req.find("GET /api/delete") != std::string::npos) {
            myPlaylist.deleteCurrentSong();
            sendPlaylistJSON(clientSocket);
        } else if (req.find("GET /api/sort_name") != std::string::npos) {
            myPlaylist.bubbleSort(); // Sort by Name
            sendPlaylistJSON(clientSocket);
        } else if (req.find("GET /api/sort_time") != std::string::npos) {
            myPlaylist.sortByDuration(); // Sort by Duration
            sendPlaylistJSON(clientSocket);
        } else if (req.find("POST /api/add") != std::string::npos) {
            size_t bodyPos = req.find("\r\n\r\n");
            if (bodyPos != std::string::npos) {
                parseAndAddTrack(req.substr(bodyPos + 4));
            }
            sendPlaylistJSON(clientSocket);
        } else if (req.find(".mp3") != std::string::npos) {
            std::string reqFile;
            std::stringstream ss(req);
            std::string method, path;
            ss >> method >> path;
            if (!path.empty() && path[0] == '/') path = path.substr(1);
            serveFile(clientSocket, path, "audio/mpeg");
        } else {
            serveFile(clientSocket, "index.html", "text/html");
        }

        CLOSE_SOCKET(clientSocket);
    }

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}