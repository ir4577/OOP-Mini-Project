#include "playlist.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

void Playlist::addSong(const std::string& title, const std::string& artist, int duration, const std::string& filename) {
    songs.emplace_back(title, artist, duration, filename);
}

bool Playlist::deleteCurrentSong() {
    if (songs.empty()) return false;

    songs.erase(songs.begin() + static_cast<ptrdiff_t>(currentIndex));
    
    if (currentIndex >= songs.size() && !songs.empty()) {
        currentIndex = songs.size() - 1;
    }
    return true;
}

void Playlist::nextSong() {
    if (songs.empty()) return;

    if (currentIndex + 1 < songs.size()) {
        currentIndex++;
    } else if (loopMode) {
        currentIndex = 0;
    }
}

void Playlist::prevSong() {
    if (songs.empty()) return;

    if (currentIndex > 0) {
        currentIndex--;
    } else if (loopMode) {
        currentIndex = songs.size() - 1;
    }
}

const Song* Playlist::getCurrentSong() const {
    if (songs.empty()) return nullptr;
    return &songs[currentIndex];
}

int Playlist::linearSearch(const std::string& title) const {
    for (size_t i = 0; i < songs.size(); ++i) {
        if (songs[i].title == title) {
            return static_cast<int>(i + 1); // 1-based index matching original implementation
        }
    }
    return -1;
}

int Playlist::binarySearch(const std::string& title) const {
    // Binary search requires sorted container. Assumes sorted order by title.
    int low = 0;
    int high = static_cast<int>(songs.size()) - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (songs[mid].title == title) {
            return mid + 1;
        }
        if (songs[mid].title < title) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

void Playlist::bubbleSort() {
    if (songs.empty()) return;
    for (size_t i = 0; i < songs.size() - 1; ++i) {
        for (size_t j = 0; j < songs.size() - i - 1; ++j) {
            if (songs[j].title > songs[j + 1].title) {
                std::swap(songs[j], songs[j + 1]);
            }
        }
    }
}

void Playlist::mergeSort() {
    // Idiomatic C++ sort implementation (typically IntroSort / O(N log N))
    std::sort(songs.begin(), songs.end(), [](const Song& a, const Song& b) {
        return a.title < b.title;
    });
}

void Playlist::saveToFile(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) return;

    for (const auto& song : songs) {
        outFile << song.title << ";"
                << song.artist << ";"
                << song.duration << ";"
                << song.filename << "\n";
    }
}

void Playlist::loadFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) return;

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string title, artist, durationStr, fn;

        std::getline(ss, title, ';');
        std::getline(ss, artist, ';');
        std::getline(ss, durationStr, ';');
        std::getline(ss, fn, ';');

        int duration = 180;
        try {
            if (!durationStr.empty()) {
                duration = std::stoi(durationStr);
            }
        } catch (...) {
            duration = 180;
        }

        if (!title.empty()) {
            addSong(title, artist, duration, fn);
        }
    }
}

void Playlist::renderUI() const {
    std::cout << "\n================ CURRENT PLAYLIST ================\n";
    if (songs.empty()) {
        std::cout << "               [Playlist is empty]\n";
    } else {
        for (size_t i = 0; i < songs.size(); ++i) {
            std::cout << (i == currentIndex ? " > " : "   ")
                      << (i + 1) << ". " << songs[i].title << " - " << songs[i].artist
                      << " (" << songs[i].duration << "s)\n";
        }
    }
    std::cout << "Loop Mode: " << (loopMode ? "ON" : "OFF") << "\n";
    std::cout << "==================================================\n";
}