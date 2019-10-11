### DAE Parsing ###

As XML, DAE is harder to parse, requiring a pushdown automaton. It can't be parsed into a binary representation in
parallel because of this, however it could be tokenized in parallel, parsed in serial, and then re-parsed in parallel
into as close a representation to the final binary as possible, then being recombined later.










