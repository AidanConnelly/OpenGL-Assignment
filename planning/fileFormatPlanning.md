# DOT-FUZ #
 * Distribution
 * Optimization
 * Three-D
 * Format
 * Used for
 * Zipping

Encoding:
 * Make float array
 * Dedup float array via indexes
 * Fit GMM
 * Write via huffman coding
 * Now have byte vector
 * Make int index
 * Write int index to precision needed into byte vector
 * At top of file, indicate all needed precisions
 * Then write float array
 * Then write index array


```
All floats => deduplicate ~> deduped floats => GMM to argmin(message length) ~ parameters
                 |                                          |   
                 v                                          v
All ints => all ints & float indexes                  huffman coding
                                     \               /
                                      \--> bytes <--/
```

32 bit float 
average 2.92 bits + 13 bits ~= 15.9 bits

that's half the bits in a float32!

upper bound on standard deviation of 2-dirac delta distributions: 0.5
largest section with std-dev of 1.37: 0.757
adjusting largest section for upper bound on worst case distribtuion: 0.552

alternatively - dynamically set number of bits based on standard deviation?

anyway:
 * bit for which gaussian
 * bit for sign
 * bit for where in gaussian
 * bit (set on a per-gaussian basis) for where in band it is