# Steganography
Steganography is the practice of concealing a file, message, image, or video within another file, message, image, or video. The word steganography combines the Greek words steganos (στεγανός), meaning "covered, concealed, or protected," and graphein (γράφειν) meaning "writing".

# How to run

1. run make

2. then use:
./hide -m 5 baseNameIn baseNameOut
./hide -p fileToReadFrom
./hide -s fileIn fileOut

the house_2.ppm file provided can be used for testing

3. Or unhide with:
./unhide file
./unhide -m baseName

# Attributes
- Signal's end of hidden message with 8 bits of 0, this means if the user is sending nulls throughout
their messages, it may end early, or any other strange arrangement that makes 8 bits of zero assign in a row.
- Howwever it does support sending 0.
- Multi-file unhide just reads until there's no more files with the appropriate name left (which should work for all use cases).
- When displaying images the top one is before encoding, the second one is after encoding - So that the user can see the differences

# ASSUMPTIONS
1. Parallel program takes no more than 50 processes, each line being 80 or less characters
2. No comment before P6 declaration or after max color value
   (the examples given on paulbourke.net/dataformats/ppm/ suggests this is correct behavior)
  
# RESTRICTIONS
1. Header is always output as following (comments will be copied over too)
    P6
    Width Height
    Maximum color
    (doesn't support headers that aren't structured on three lines)
2. When hiding messages with parallel it won't exit on error, instead it goes to the next line in the file and tries that one.
   (Error messages will be communicated still - I'm not sure this is a restriction, it's more a benefit)
3. Truncated image files don't work
