### DAE Parsing ###

~~As XML, DAE is harder to parse, requiring a pushdown automaton. It can't be parsed into a binary representation in
parallel because of this, however it could be tokenized in parallel, parsed in serial, and then re-parsed in parallel
into as close a representation to the final binary as possible, then being recombined later.~~

An XML hierarchy would be travelled up and down via the < (TAG NAME) ... > (INSIDE) < / (TAG NAME)> pattern, and while
this could be preprocessed in parallel before creating the tree in serial, before then top-down or bottom-up parsing,
it's probably similarly fast to have a single serial run, described by "Alg1" below and then a parallel map, before the
final serial combine.

Alg1:
Stack machine:

State       | Action | Next state                  | Comment
------------+--------+-----------------------------+---------
Start       | <      | TagOpened                   |
TagOpened   | not /  | NotEndTag                   |
NotEndTag   | >      | Start, stack++, Start       | start tag, depth++
NotEndTag   | /      | Start                       | empty tag
TagOpened   | /      | EndTag                      |
EndTag      | >      | Start, stack--, Start       | end tag, depth--
Start       | "      | Start, stack++, DQuote      |
TagOpened   | "      | TagOpened, stack++, DQuote  |
NotEndTag   | "      | NotEndTag, stack++, DQuote  |
EndTag      | "      | EndTag, stack++, DQuote     |
Start       | '      | Start, stack++, SQuote      |
TagOpened   | '      | TagOpened, stack++, SQuote  |
NotEndTag   | '      | NotEndTag, stack++, SQuote  |
EndTag      | '      | EndTag, stack++, SQuote     |
DQuote      | '      | DQuote                      |
SQuote      | "      | SQuote                      |
DQuote      | "      | DQuote, stack--, <PREVIOUS> |
SQuote      | '      | SQuote, stack--, <PREVIOUS> |

However while parsing the XML using this stack machine, the raw binary can also be split into (via indexing of course),
different individual nodes which can be parsed in a top-down or bottom-up manner. Some of the nodes can be ignored,
either in parallel or serial, and then finally those that are relevant somehow have to be translated to VAOs & VBOs




Despite extensive research on the COLLADA specification, it's still not clear to me the exact procedure to parse it into
openGLesque data structures.

This seems an accessible resource however I don't have large confidence in the accuracy:
https://www.codeproject.com/Articles/625701/COLLADA-TinyXML-and-OpenGL

