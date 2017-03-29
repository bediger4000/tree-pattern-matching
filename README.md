# Matching Patterns of binary trees

An implementation of:
Hoffman, C. and O'Donnel, J., "Pattern matching in trees", JACM, 29(1), 68–95, 1982.

Which in turn uses Aho/Corasick multiple string matching:
Aho, Alfred V.; Margaret J. Corasick (June 1975).
"Efficient string matching: An aid to bibliographic search".
Communications of the ACM 18 (6): 333–340. 

Implemented in ANSI C89, I believe.

## TEST INPUT

Test program input trees are represented textually as lisp-style 2-element
lists.  Must be fully parenthesized.

    (a b)
    (one (two three))
    ((one two) (three four))

Internally, the algorithm uses conventional binary trees:

	struct tree {
		enum node_type type;
		char   label[64];
		int    labelsz;
		int tree_size;
		struct tree *left;
		struct tree *right;
	};


### Patterns

Pattern lists can contain '\*' characters which represent "match anything",
globbing-style wildcards.  A '\*' matches any subtree, or any leaf. Any other
character or string (besides parentheses and whitespace) matches a node name only.

	$ ./test6 -p '(a *)' -s '((a b) (a c))'
    
    Match pattern with subtree:
    (a b)
    
    Match pattern with subtree:
    (a c)

	$ ./test6 -p '(b *)' -s '((a b) (a c))'
    
    $

## BUILDING

    make test6
    sh ./test.input

Uses gcc as C compiler, but Clang and pcc also work.
