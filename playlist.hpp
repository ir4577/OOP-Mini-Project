#ifndef PLAYLIST_HPP
#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>

// Represents an individual track in the playlist
struct Song {
    std::string title;
    std::string artist;
    int duration{0}; // Duration in seconds
    std::string filename;

    Song(std::string t, std::string a, int d, std::string fn = "")
        : title(std::move(t)), artist(std::move(a)), duration(d), filename(std::move(fn)) {}
};

// Playlist managed class using std::vector for safety and native sorting/searching
class Playlist {
private:
    std::vector<Song> songs;
    size_t currentIndex{0};
    bool loopMode{false};

public:
    Playlist() = default;

    // Core Playlist Operations
    void addSong(const std::string& title, const std::string& artist, int duration, const std::string& filename = "");
    bool deleteCurrentSong();
    void nextSong();
    void prevSong();
    
    // Getters and Setters
    [[nodiscard]] size_t getSize() const { return songs.size(); }
    [[nodiscard]] bool getLoopMode() const { return loopMode; }
    void toggleLoopMode() { loopMode = !loopMode; }
    [[nodiscard]] const Song* getCurrentSong() const;

    // Search and Sort Utilities
    [[nodiscard]] int linearSearch(const std::string& title) const;
    [[nodiscard]] int binarySearch(const std::string& title) const;
    void bubbleSort();
    void mergeSort();

    // File I/O
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);

    // UI Helper
    void renderUI() const;
};

#endif // PLAYLIST_HPP