# Dump

This is a small C program to dump the content of a file or stdin
in various formats, such as hex bytes with string representation.

Typical output:

    00000940:  20484558 4d4f4445 3a0d0a20 20202070      HEXMODE:..    p
    00000950:  72696e74 48657828 62797465 3e3e3429     rintHex(byte>>4)
    00000960:  3b0d0a20 20202070 72696e74 48657828     ;..    printHex(
    00000970:  62797465 26307866 293b0d0a 20202020     byte&0xf);..
