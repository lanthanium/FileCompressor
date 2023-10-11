#include <iostream>
#include "HCTree.hpp"
#include "Helper.hpp"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        error("Type in 2 file names");
        return 1;
    }


    /*
    A vector of integer from [0-255].
    i: ASCII Decimal [0-255]
    freq[i]: how many times i appears
    */
    vector<int> freq(256, 0);


    FancyInputStream compressed = FancyInputStream(argv[1]);

    /*
    If empty file, than create another empty file with second argument filename
    */
    if (compressed.filesize() == 0) {
        FancyOutputStream* uncompress = new FancyOutputStream(argv[2]);
        delete uncompress;
        return 0;
    }


    //if file to read from/uncompress doesn't exist 
    if (compressed.filesize() < 0) error("You didn't type out a valid filename to uncompress");


    /*
    The first 4 bytes in compressedFile is the header size
    Parsing this value into numberOfLeafNodes
    */
    int numberOfLeafNodes = compressed.read_int();

    /* Initializing totalChars
    Total number of bytes the original file has
    AKA The sum of all the frequencies in the vector
    This will be updated once we read from compressed file
    */
    int totalChars = 0;

    /* READING HEADER
    Since the header size is now known, aka number of leaves 
    and since we know the way the header is formatted from compress.cpp
    We can read the header and stop at precisely where the header ends
    As we read the header, we are updating our freq vector
    and incrementing the totalChars
    */
    for (int i = 0; i < numberOfLeafNodes; i++) {
        int ascii = compressed.read_byte();
        int count = compressed.read_int();

        freq[ascii] = count;
        totalChars += count;
        //cout << (int)ascii << ": " << count << endl;
    }


    /*
    Rebuild the tree with our frequency vector
    */
    HCTree* tree = new HCTree();
    tree->build(freq);

    FancyOutputStream* uncompress = new FancyOutputStream(argv[2]);    
    for (int i = 0; i < totalChars; i++) {
        uncompress->write_byte(tree->decode(compressed));
    }

    delete tree;
    delete uncompress;

    return 0;

}
