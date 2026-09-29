#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "playlist.hpp"

// ANSI Escape Codes
constexpr const char* COLOR_RESET   = "\033[0m";
constexpr const char* COLOR_CYAN    = "\033[1;36m";
constexpr const char* COLOR_GREEN   = "\033[1;32m";
constexpr const char* COLOR_YELLOW  = "\033[1;33m";
constexpr const char* COLOR_RED     = "\033[1;31m";
constexpr const char* COLOR_MAGENTA = "\033[1;35m";

void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void renderUI(const Playlist& pl) {
    clearScreen();
    std::cout << COLOR_CYAN << "===================================================\n" << COLOR_RESET;
    std::cout << COLOR_MAGENTA << "    ♪ MUSIC PLAYLIST MANAGER (C++ ENGINE) ♪        \n" << COLOR_RESET;
    std::cout << COLOR_CYAN << "===================================================\n" << COLOR_RESET;

    std::cout << " Total Tracks: " << COLOR_YELLOW << pl.getSize() << COLOR_RESET
              << " | Loop Mode: " << (pl.getLoopMode() ? COLOR_GREEN : COLOR_RED)
              << (pl.getLoopMode() ? "ON" : "OFF") << COLOR_RESET << "\n";
    std::cout << "---------------------------------------------------\n";

    if (pl.getSize() == 0) {
        std::cout << "\n  " << COLOR_RED << "[ Playlist is empty. Add tracks or load library! ]" << COLOR_RESET << "\n\n";
        std::cout << COLOR_CYAN << "===================================================\n" << COLOR_RESET;
        return;
    }

    const Song* current = pl.getCurrentSong();
    if (current) {
        std::cout << COLOR_GREEN << " NOW PLAYING: " << COLOR_RESET
                  << COLOR_YELLOW << current->title << COLOR_RESET << " by "
                  << COLOR_CYAN << current->artist << COLOR_RESET << " ["
                  << (current->duration / 60) << "m " << (current->duration % 60) << "s]\n";
        std::cout << "---------------------------------------------------\n";
    }

    const auto& songs = pl.getSongs();
    for (size_t i = 0; i < songs.size(); ++i) {
        bool isPlaying = (current != nullptr && &songs[i] == current);
        if (isPlaying) {
            std::cout << COLOR_GREEN << " -> " << std::setw(2) << (i + 1) << ". "
                      << std::left << std::setw(20) << songs[i].title << " | "
                      << std::setw(15) << songs[i].artist << " [*PLAYING*]" << COLOR_RESET << "\n";
        } else {
            std::cout << "    " << std::setw(2) << (i + 1) << ". "
                      << std::left << std::setw(20) << songs[i].title << " | "
                      << std::setw(15) << songs[i].artist << "\n";
        }
    }
    std::cout << COLOR_CYAN << "===================================================\n" << COLOR_RESET;
}