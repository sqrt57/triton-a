; Parser stub, only prints lexemes
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

lexeme_start_file:
        push lexer_starting_len
        push lexer_starting_str
        call print
        add esp, 8
        ret

lexeme_end_file:
        push lexer_done_len
        push lexer_done_str
        call print
        add esp, 8
        ret

; Processes identifier lexeme
; In: 1) Identifier name address
;     2) Identifier name length
lexeme_identifier:
        .ident_str equ dword [ebp+8]
        .ident_len equ dword [ebp+12]

        push ebp
        mov ebp, esp

        push identifier_len
        push identifier_str
        call print
        add esp, 8
        push .ident_len
        push .ident_str
        call print
        add esp, 8
        call print_quote
        call newline

        mov esp, ebp
        pop ebp
        ret

; Processes identifier lexeme
; In: 1) Special name length, maximum 4
;     2) Special name value on stack, maximum 4 bytes
lexeme_special:
        .name_val equ dword [ebp+8]
        .name_len equ dword [ebp+12]

        push ebp
        mov ebp, esp

        push special_len
        push special_str
        call print
        add esp, 4

        lea edx, .name_val
        push .name_len
        push edx
        call print
        add esp, 8

        call print_quote
        call newline

        mov esp, ebp
        pop ebp
        ret

; Processes char literal lexeme
; In: 1) Character, high 3 bytes are zero
lexeme_char_literal:
        .char equ dword [ebp+8]

        push ebp
        mov ebp, esp

        push char_lexeme_len
        push char_lexeme_str
        call print
        add esp, 4

        lea edx, .char
        push dword 1
        push edx
        call print
        add esp, 8

        call print_apostrophe
        call newline

        mov esp, ebp
        pop ebp
        ret
