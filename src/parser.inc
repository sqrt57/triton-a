; Parse token stream into abstract syntax tree
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

parser_state.global = 1
parser_state.expect_module_name = 2
parser_state.expect_module_open_brace = 3
parser_state.module = 4

token_start_file:
        push ebp
        mov ebp, esp

        mov [parser_state], parser_state.global

        mov esp, ebp
        pop ebp
        ret

token_end_file:
        push ebp
        mov ebp, esp

        mov eax, [parser_state]
        cmp eax, parser_state.global
        jz .global
        cmp eax, parser_state.expect_module_name
        jz .expect_module_name
        cmp eax, parser_state.expect_module_open_brace
        jz .expect_module_open_brace
        cmp eax, parser_state.module
        jz .module
        jmp .default

    .global:
        jmp .ret
    .expect_module_name:
    .expect_module_open_brace:
    .module:
    .parser_unexpected_keyword:
    .default:
        jmp .unexpected_eof

    .unexpected_eof:
        push parser_unexpected_eof_len
        push parser_unexpected_eof_str
        call print
        add esp, 8
        push dword 1
        call sys_exit

    .ret:
        mov esp, ebp
        pop ebp
        ret

; Processes identifier token
; In: 1) Identifier name address
;     2) Identifier name length
token_identifier:
        .ident_str equ dword [ebp+8]
        .ident_len equ dword [ebp+12]

        push ebp
        mov ebp, esp

        mov eax, [parser_state]
        cmp eax, parser_state.global
        jz .global
        cmp eax, parser_state.expect_module_name
        jz .expect_module_name
        cmp eax, parser_state.expect_module_open_brace
        jz .expect_module_open_brace
        cmp eax, parser_state.module
        jz .module
        jmp .default

    .global:
        jmp .unexpected_identifier
    .expect_module_name:
        jmp .module_name
    .expect_module_open_brace:
    .module:
    .parser_unexpected_keyword:
    .default:
        jmp .unexpected_identifier

    .unexpected_identifier:
        push parser_unexpected_identifier_len
        push parser_unexpected_identifier_str
        call print
        add esp, 8
        push .ident_len
        push .ident_str
        call print
        add esp, 8
        call newline
        push dword 1
        call sys_exit

    .module_name:
        mov [parser_state], parser_state.expect_module_open_brace

        push module_name_len
        push module_name_str
        call print
        add esp, 8
        push .ident_len
        push .ident_str
        call print
        add esp, 8
        call newline
        jmp .ret

    .ret:
        mov esp, ebp
        pop ebp
        ret

parser_unexpected_keyword:
        .ident_str equ dword [ebp+8]
        .ident_len equ dword [ebp+12]

        push parser_unexpected_keyword_len
        push parser_unexpected_keyword_str
        call print
        add esp, 8
        push .ident_len
        push .ident_str
        call print
        add esp, 8
        call newline
        push dword 1
        call sys_exit

; Processes module keyword token
; In: 1) Identifier name address
;     2) Identifier name length
token_keyword_module:
        .ident_str equ dword [ebp+8]
        .ident_len equ dword [ebp+12]

        push ebp
        mov ebp, esp

        mov eax, [parser_state]
        cmp eax, parser_state.global
        jz .global
        cmp eax, parser_state.expect_module_name
        jz .expect_module_name
        cmp eax, parser_state.expect_module_open_brace
        jz .expect_module_open_brace
        cmp eax, parser_state.module
        jz .module
        jmp .default

    .global:
        mov [parser_state], parser_state.expect_module_name
        jmp .ret
    .expect_module_name:
    .expect_module_open_brace:
    .module:
    .default:
        jmp parser_unexpected_keyword

    .ret:
        mov esp, ebp
        pop ebp
        ret

parser_unexpected_special:
        .name_val equ dword [ebp+8]
        .name_len equ dword [ebp+12]

        push parser_unexpected_special_len
        push parser_unexpected_special_str
        call print
        add esp, 8
        lea edx, .name_val
        push .name_len
        push edx
        call print
        add esp, 8
        call newline
        push dword 1
        call sys_exit

; Processes left curly brace token
; In: 1) Special name length, maximum 4
;     2) Special name value on stack, maximum 4 bytes
token_left_curly:
        .name_val equ dword [ebp+8]
        .name_len equ dword [ebp+12]

        push ebp
        mov ebp, esp

        mov eax, [parser_state]
        cmp eax, parser_state.global
        jz .global
        cmp eax, parser_state.expect_module_name
        jz .expect_module_name
        cmp eax, parser_state.expect_module_open_brace
        jz .expect_module_open_brace
        cmp eax, parser_state.module
        jz .module
        jmp .default

    .global:
    .expect_module_name:
        jmp parser_unexpected_special
    .expect_module_open_brace:
        mov [parser_state], parser_state.module
        jmp .ret
    .module:
    .default:
        jmp parser_unexpected_special

    .ret:
        mov esp, ebp
        pop ebp
        ret

; Processes right curly brace token
; In: 1) Special name length, maximum 4
;     2) Special name value on stack, maximum 4 bytes
token_right_curly:
        .name_val equ dword [ebp+8]
        .name_len equ dword [ebp+12]

        push ebp
        mov ebp, esp

        mov eax, [parser_state]
        cmp eax, parser_state.global
        jz .global
        cmp eax, parser_state.expect_module_name
        jz .expect_module_name
        cmp eax, parser_state.expect_module_open_brace
        jz .expect_module_open_brace
        cmp eax, parser_state.module
        jz .module
        jmp .default

    .global:
    .expect_module_name:
    .expect_module_open_brace:
        jmp parser_unexpected_special
    .module:
        mov [parser_state], parser_state.global
        jmp .ret
    .default:
        jmp parser_unexpected_special

    .ret:
        mov esp, ebp
        pop ebp
        ret

; Processes char literal token
; In: 1) Character, high 3 bytes are zero
token_char_literal:
        .char equ dword [ebp+8]

        push ebp
        mov ebp, esp

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

        mov esp, ebp
        pop ebp
        ret
