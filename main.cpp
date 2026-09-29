#include <iostream>
#include <string>
#include <limits>
#include "playlist.hpp"

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void handleSearchMenu(Playlist& pl) {
    std::string searchTitle;
    int choice = 0;
    int index = -1;

    std::cout << "\n=== SEARCH MENU ===\n";
    std::cout << "Enter Song Title: ";
    std::getline(std::cin >> std::ws, searchTitle);

    std::cout << "1. Linear Search  O(N)\n";
    std::cout << "2. Binary Search  O(log N)\n";
    std::cout << "Choice: ";
    if (!(std::cin >> choice)) {
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (choice == 1) index = pl.linearSearch(searchTitle);
    else if (choice == 2) index = pl.binarySearch(searchTitle);

    if (index != -1) {
        std::cout << "\n[SUCCESS] Track found at Position #" << index << "!\n";
    } else {
        std::cout << "\n[NOT FOUND] Track '" << searchTitle << "' not found.\n";
    }

    std::cout << "Press ENTER to continue...";
    std::cin.get();
}

void handleSortMenu(Playlist& pl) {
    int choice = 0;

    std::cout << "\n=== SORT MENU ===\n";
    std::cout << "1. Bubble Sort O(N^2)\n";
    std::cout << "2. Merge Sort  O(N log N)\n";
    std::cout << "Choice: ";
    if (!(std::cin >> choice)) {
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (choice == 1) pl.bubbleSort();
    else if (choice == 2) pl.mergeSort();

    std::cout << "\n[SUCCESS] Playlist sorted!\n";
    std::cout << "Press ENTER to continue...";
    std::cin.get();
}

int main() {
    Playlist myPlaylist;
    myPlaylist.loadFromFile("library.txt");

    if (myPlaylist.getSize() == 0) {
        myPlaylist.addSong("Hotel California", "Eagles", 390);
        myPlaylist.addSong("Bohemian Rhapsody", "Queen", 354);
        myPlaylist.addSong("Starboy", "The Weeknd", 230);
        myPlaylist.addSong("Africa", "Toto", 295);
    }

    int choice = 0;
    std::string title, artist;
    int duration = 0;

    while (true) {
        myPlaylist.renderUI();
        std::cout << "\nControls:\n";
        std::cout << " 1. Next Track      2. Prev Track      3. Add Song\n";
        std::cout << " 4. Delete Current  5. Search Playlist 6. Sort Playlist\n";
        std::cout << " 7. Toggle Loop     8. Save Library    9. Exit\n";
        std::cout << "\nSelect Action: ";

        if (!(std::cin >> choice)) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: myPlaylist.nextSong(); break;
            case 2: myPlaylist.prevSong(); break;
            case 3:
                std::cout << "\nSong Title: ";
                std::getline(std::cin >> std::ws, title);

                std::cout << "Artist Name: ";
                std::getline(std::cin >> std::ws, artist);

                std::cout << "Duration (seconds): ";
                std::cin >> duration;
                clearInputBuffer();

                myPlaylist.addSong(title, artist, duration);
                break;
            case 4: myPlaylist.deleteCurrentSong(); break;
            case 5: handleSearchMenu(myPlaylist); break;
            case 6: handleSortMenu(myPlaylist); break;
            case 7: myPlaylist.toggleLoopMode(); break;
            case 8: myPlaylist.saveToFile("library.txt"); break;
            case 9:
                myPlaylist.saveToFile("library.txt");
                return 0;
            default: break;
        }
    }
}