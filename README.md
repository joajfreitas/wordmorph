# Wordmorph
Word path finding utility

# How?
Djikstra (using binary heap), on an adjacency list weighted graph of words (the 
edge weights are the hamming distance between each word).

## Instructions:
```
git clone https://github.com/joajfreitas/wordmorph && cd wordmorph
cd src && make
```

## Usage:
```
./wordmorph dic.txt pal.txt
```
Where the first parameter is a dictionary file and the second parameter is a
problem file with the format:
```
"first word" "second word" "number of permutations allowed"
```
Wordmorph will find the shortest path between the first and second words and
write it to a .path file.

### Developed by:
  * [pineman](https://www.github.com/pineman)
  * [joajfreitas](https://www.github.com/joajfreitas)
