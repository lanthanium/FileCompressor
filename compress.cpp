#include <iostream>
#include "HCTree.hpp"
#include "Helper.hpp"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        error("Type in 2 file names");
        return 1;
    }

    /*
    Opens input file "original" for reading
    Opens outputfile "compress" for writing
    */
    FancyInputStream original = FancyInputStream(argv[1]);

    /*if empty return function*/
    if (original.filesize() == 0)
    {
        FancyOutputStream* compress = new FancyOutputStream(argv[2]);
        delete compress;
        return 0;
    }

    /*
    A vector of integer from [0-255].
    i: ASCII Decimal [0-255]
    freq[i]: how many times i appears
    */
    vector<int> freq(256, 0);
    int totalChars = 0;

    /*
    updating freq vector using the original file
    */
    for (int i = 0; i < original.filesize(); i++) {
        int byteRead = original.read_byte();
        freq[byteRead]++;
        totalChars++;
    }

    if (totalChars == 0) {
        error("You didn't enter a valid file to read from.");
    } 


    /*
    Moving back to beginning on input file to read again
    */
    original.reset();

    /*
    Building Huffman Tree from the frequency table
    */
    HCTree* tree = new HCTree();
    tree->build(freq);

    /*
    Writing a header
    The first thing written to header file is the header size as an INT
    the rest of header is all the leaves in the format
    char (BYTE)
    count (INT)
    repeat
    */

    //First step is to create map of the leaves
    //leaves[symbol] = frequency
    map<char, int> leaves = tree->getNonZeroLeavesVector();

    //Amount of leafNodes is the size of this map
    int numberOfLeafNodes = leaves.size();

    /*
    WRITING THE numberOfLeafNodes INTO compressedFile
    uncompress.cpp will parse this value and then iterate through compressedFile
    this many times to extract only the header to rebuild the frequency vector
    */
    FancyOutputStream* compress = new FancyOutputStream(argv[2]);
    compress->write_int(numberOfLeafNodes); //4 bytes


    /*
    WRITING THE HEADERFILE
    Iterating through the map of leaves
    For each leaf
     write into compressedFile the char symbol
     write into compressedFile the corresponding frequency
    */
    for (auto i : leaves) {
        //cout << "ASCII: " << i.first << " COUNT: " << i.second << endl;
        compress->write_byte(i.first);
        compress->write_int(i.second);
    }

    /*
    READING ORIGINAL FILE BYTE BY BYTE
    For each byte read 
    ENCODE this byte via corresponding tree method into an encoded format
    */
    for (int i = 0; i < original.filesize(); i++) {
        unsigned char charRead = original.read_byte();
        //compress->write_byte(charRead);
        tree->encode(charRead, *compress);
    }

    //Flush
    compress->flush_bitwise();

    delete tree;
    delete compress;

    return 0;

}
