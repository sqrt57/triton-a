; Parser stub, only prints tokens
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

token_start_file:
        push lexer_starting_len
        push lexer_starting_str
        call print
        add esp, 8
        ret

token_end_file:
        push lexer_done_len
        push lexer_done_str
        call print
        add esp, 8
        ret

; Processes identifier token
; In: 1) Identifier name address
;     2) Identifier name length
token_identifier:
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

; Processes identifier token
; In: 1) Special name length, maximum 4
;     2) Special name value on stack, maximum 4 bytes
token_special:
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

; Processes char literal token
; In: 1) Character, high 3 bytes are zero
token_char_literal:
        .char equ dword [ebp+8]

        push ebp
        mov ebp, esp

        push char_token_len
        push char_token_str
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

; Processes string literal token
; In: 1) String literal length
;     2) String literal address
token_string_literal:
        .str equ dword [ebp+8]
        .str_len equ dword [ebp+12]

        push ebp
        mov ebp, esp

        push string_token_len
        push string_token_str
        call print
        add esp, 4

        push .str_len
        push .str
        call print
        add esp, 8

        call print_quote
        call newline

        mov esp, ebp
        pop ebp
        ret

; Processes number literal token
; In: 1) Number literal value
;     2) Sign of number literal:
;         0: positive
;         0xffffffff: negative
; SIGN=0 means that NUMBER contains unsigned 32-bit value
; SIGN=-1 means that NUMBER contains signed 32-bit value
token_number_literal:
        .number equ dword [ebp+8]
        .sign equ dword [ebp+12]

        push ebp
        mov ebp, esp

        push number_token_len
        push number_token_str
        call print
        add esp, 8

        cmp .sign, 0
        jne .signed

        push .number
        call print_u32
        add esp, 4

        jmp .done

    .signed:
        push .number
        call print_s32
        add esp, 4

    .done:

        call newline

        mov esp, ebp
        pop ebp
        ret
