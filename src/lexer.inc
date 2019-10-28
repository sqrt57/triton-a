; Lexer
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

lexer_init:
        xor ecx, ecx
        inc ecx

        mov eax, is_single_char_table
        mov [eax + '{'], cl
        mov [eax + '}'], cl
        mov [eax + '('], cl
        mov [eax + ')'], cl
        mov [eax + '['], cl
        mov [eax + ']'], cl
        mov [eax + ';'], cl
        mov [eax + ','], cl
        mov [eax + '.'], cl
        mov [eax + '^'], cl
        mov [eax + '@'], cl

        mov eax, is_operator_char_table
        mov [eax + '+'], cl
        mov [eax + '-'], cl
        mov [eax + '*'], cl
        mov [eax + '/'], cl
        mov [eax + '<'], cl
        mov [eax + '>'], cl
        mov [eax + '='], cl
        mov [eax + '!'], cl
        mov [eax + ':'], cl
        ret

; Split source stream into lexemes and call token evaluator
; In: 1) Address of memory block with source
;     2) Length of memory bock
lex:
        .source_addr equ dword [ebp+8]
        .source_len equ dword [ebp+12]

        .end_addr equ dword [ebp-4]
        .lexeme_start equ dword [ebp-8]
        ; The following literal variants are exclusive so they share storage
        ; Used for storing string literal
        .str_addr equ dword [ebp-12]
        .str_len equ dword [ebp-16]
        ; Used for storing number literal
        .number equ dword [ebp-12]
        .sign equ dword [ebp-16]

        push ebp
        mov ebp, esp
        sub esp, 16
        push esi

        call token_start_file

        mov esi, .source_addr
        mov eax, esi
        add eax, .source_len
        mov .end_addr, eax

    .next:
        xor eax, eax
    .skip_whitespace:
        mov .lexeme_start, esi
        cmp esi, .end_addr
        jae .ret
        lodsb
        call is_whitespace
        jz .skip_whitespace

        cmp eax, "/"
        jz .dispatch_slash

        cmp eax, "'"
        jz .read_char_literal

        cmp eax, '"'
        jz .read_string_literal

        cmp eax, '-'
        jz .dispatch_minus

        cmp eax, '0'
        jz .read_number

        call is_single_char
        jz .read_single_char

        call is_operator_char
        jz .read_operator

        call is_identifier_start
        jz .read_identifier

        call is_digit
        jz .read_number

        jmp .bad_char

    .dispatch_slash:
        cmp esi, .end_addr
        jae .read_operator
        lodsb
        cmp eax, '/'
        jz .line_comment
        cmp eax, '*'
        jz .comment
        dec esi
        mov eax, '/'
        jmp .read_operator

    .dispatch_minus:
        cmp esi, .end_addr
        jae .read_operator
        lodsb
        call is_digit
        jz .dispatch_minus_number
        dec esi
        mov eax, '-'
        jmp .read_operator
    .dispatch_minus_number:
        dec esi
        mov eax, '-'
        jmp .read_number

    .read_identifier:
        lea edx, [esi-1]
        push edx
    .identifier_char:
        cmp esi, .end_addr
        jae .done_identifier
        lodsb
        call is_identifier_char
        jz .identifier_char
        dec esi
    .done_identifier:
        pop eax
        mov edx, esi
        sub edx, eax
        push edx
        push eax
        call token_identifier
        add esp, 8
        jmp .next

    .read_single_char:
        push 1
        push eax
        call token_special
        add esp, 8
        jmp .next

    .read_operator:
        push dword 1
        mov ecx, eax

        cmp esi, .end_addr
        jae .operator_done_eof
        lodsb
        call is_operator_char
        jnz .operator_done

        shl eax, 8
        or ecx, eax
        mov [esp], dword 2

        cmp esi, .end_addr
        jae .operator_done_eof
        xor eax, eax
        lodsb
        call is_operator_char
        jnz .operator_done

        shl eax, 16
        or ecx, eax
        mov [esp], dword 3

        cmp esi, .end_addr
        jae .operator_done_eof
        xor eax, eax
        lodsb
        call is_operator_char
        jnz .operator_done

        shl eax, 24
        or ecx, eax
        mov [esp], dword 4

        cmp esi, .end_addr
        jae .operator_done_eof
        xor eax, eax
        lodsb
        call is_operator_char
        jnz .operator_done

        add esp, 4
        push operator_too_long_len
        push operator_too_long_str
        call print
        add esp, 8
        call .print_lexeme
        call print_quote
        call newline

        push dword 1
        call sys_exit

    .operator_done:
        dec esi
    .operator_done_eof:
        push ecx
        call token_special
        add esp, 8
        jmp .next

    .negative:
        dec .sign
        lodsb
        jmp .read_decimal

    .read_number:
        xor edx, edx
        mov .number, edx
        mov .sign, edx
        cmp eax, '-'
        je .negative
    .read_decimal:
        cmp eax, '0'
        jne .read_number_digit
        cmp esi, .end_addr
        jae .number_done_zero
        lodsb
        cmp eax, 'x'
        je .number_hex
        cmp eax, 'X'
        je .number_hex
    .read_number_digit:
        call digit_to_int
        jnz .number_check_next_char

        mov ecx, eax
        mov eax, 10
        mul .number
        jc .number_too_big
        add eax, ecx
        jc .number_too_big
        mov .number, eax

        cmp esi, .end_addr
        jae .number_done_eof
        xor eax, eax
        lodsb
        jmp .read_number_digit

    .number_hex:
        cmp esi, .end_addr
        jae .hex_number_literal_digits_expected
        lodsb
        call is_hex_digit
        jnz .hex_number_literal_digits_expected
    .read_hex_number_digit:
        call hex_digit_to_int
        jnz .number_check_next_char

        mov ecx, eax
        mov eax, 16
        mul .number
        jc .number_too_big
        add eax, ecx
        jc .number_too_big
        mov .number, eax

        cmp esi, .end_addr
        jae .number_done_eof
        xor eax, eax
        lodsb
        jmp .read_hex_number_digit

    .number_check_next_char:
        call is_whitespace
        jz .number_next_char_ok
        call is_single_char
        jz .number_next_char_ok
        call is_operator_char
        jz .number_next_char_ok
        jmp .number_literal_bad_char
    .number_next_char_ok:
        dec esi
    .number_done_eof:
        cmp .sign, 0
        jz .number_sign_done
        cmp .number, 0
        jz .number_done_zero
        neg .number
        jns .number_too_big
    .number_sign_done:
        push .sign
        push .number
        call token_number_literal
        add esp, 8
        jmp .next

    .number_done_zero:
        xor eax, eax
        push eax
        push eax
        call token_number_literal
        add esp, 8
        jmp .next

    .number_too_big:
        push number_too_big_len
        push number_too_big_str
        call print
        add esp, 8
        call .print_lexeme
        call newline

        push dword 1
        call sys_exit

    .number_literal_bad_char:
        push number_literal_bad_char_len
        push number_literal_bad_char_str
        call print
        add esp, 8
        call .print_lexeme
        call newline

        push dword 1
        call sys_exit

    .hex_number_literal_digits_expected:
        push hex_number_literal_digits_expected_len
        push hex_number_literal_digits_expected_str
        call print
        add esp, 8
        call .print_lexeme
        call newline

        push dword 1
        call sys_exit

    .read_char_literal:
        cmp esi, .end_addr
        jae .expected_char_literal
        lodsb
        cmp eax, '\'
        jz .char_literal_special
        cmp eax, "'"
        jz .expected_char_literal
        jmp .end_char_literal
    .char_literal_special:
        cmp esi, .end_addr
        jae .expected_char_literal
        lodsb
        call convert_special_char
        jz .end_char_literal
        inc .lexeme_start
        push char_unknown_escape_code_len
        push char_unknown_escape_code_str
        call print
        add esp, 8
        mov eax, esi
        sub eax, 2
        push dword 2
        push eax
        call print
        add esp, 8
        push char_unknown_escape_code_end_len
        push char_unknown_escape_code_end_str
        call print
        add esp, 8
        push dword 1
        call sys_exit
    .end_char_literal:
        mov ecx, eax
        cmp esi, .end_addr
        jae .char_literal_not_closed
        xor eax, eax
        lodsb
        cmp eax, "'"
        jnz .char_literal_not_closed
        push ecx
        call token_char_literal
        add esp, 4
        jmp .next
    .expected_char_literal:
        push expected_char_literal_len
        push expected_char_literal_str
        call print
        add esp, 8
        call .print_lexeme
        call print_quote
        call newline
        push dword 1
        call sys_exit
    .char_literal_not_closed:
        push char_literal_close_quote_expected_len
        push char_literal_close_quote_expected_str
        call print
        add esp, 8
        call .print_lexeme
        call print_quote
        call newline
        push dword 1
        call sys_exit

    .read_string_literal:
        xor ecx, ecx
    .string_length:
        cmp esi, .end_addr
        jae .string_literal_not_closed
        lodsb
        cmp eax, '"'
        jz .string_copy
        inc ecx
        cmp eax, '\'
        jnz .string_length
        cmp esi, .end_addr
        jae .string_literal_not_closed
        inc esi
        jmp .string_length
    .string_copy:
        mov .str_len, ecx
        inc ecx
        push ecx
        call malloc
        add esp, 4
        mov .str_addr, eax
        push edi
        mov esi, .lexeme_start
        inc esi
        mov edi, eax
        xor eax, eax
    .string_copy_load:
        lodsb
        cmp eax, '"'
        jz .string_copy_done
        cmp eax, '\'
        jnz .string_copy_store
        lodsb
        call convert_special_char
        jnz .string_escape_code_error
    .string_copy_store:
        stosb
        jmp .string_copy_load
    .string_copy_done:
        xor eax, eax
        stosb
        pop edi
        push .str_len
        push .str_addr
        call token_string_literal
        add esp, 8
        push .str_addr
        call free
        add esp, 4
        jmp .next
    .string_literal_not_closed:
        push string_literal_close_quote_expected_len
        push string_literal_close_quote_expected_str
        call print
        add esp, 8
        call .print_lexeme
        call print_apostrophe
        call newline
        push dword 1
        call sys_exit
    .string_escape_code_error:
        push string_unknown_escape_code_len
        push string_unknown_escape_code_str
        call print
        add esp, 8
        mov eax, esi
        sub eax, 2
        push dword 2
        push eax
        call print
        add esp, 8
        push string_unknown_escape_code_end_len
        push string_unknown_escape_code_end_str
        call print
        add esp, 8
        push dword 1
        call sys_exit

    .line_comment:
        cmp esi, .end_addr
        jae .ret
        lodsb
        cmp eax, 0x0a
        jnz .line_comment
        jmp .next

    .comment:
        mov ecx, 1
        xor eax, eax
    .comment_next:
        cmp esi, .end_addr
        jae .comment_not_closed
        shl eax, 8
        lodsb
        cmp ax, '/*'
        jz .comment_close
        cmp ax, '*/'
        jz .comment_open
        jmp .comment_next
    .comment_open:
        inc ecx
        xor eax, eax
        jmp .comment_next
    .comment_close:
        dec ecx
        jz .next
        xor eax, eax
        jmp .comment_next
    .comment_not_closed:
        push comment_not_closed_len
        push comment_not_closed_str
        call print
        add esp, 8
        push dword 1
        call sys_exit

    .ret:
        call token_end_file

        pop esi
        mov esp, ebp
        pop ebp
        ret

    .bad_char:
        push bad_char_len
        push bad_char_str
        call print
        add esp, 8

        call .print_lexeme
        call print_quote
        call newline

        push dword 1
        call sys_exit

    .print_lexeme:
        mov eax, .lexeme_start
        mov edx, esi
        sub edx, eax
        push edx
        push eax
        call print
        add esp, 8
        ret

; Checks whether byte is whitespace character
; In: AL - input byte
; Out: ZF=1 iff input is whitespace
;      AL - preserves input
is_whitespace:
        ; We consider 09-0d, 20 whitespace
        cmp eax, 0x20
        jne .range_low
        ret
    .range_low:
        cmp eax, 0x09
        jae .range_high
        ret
    .range_high:
        cmp eax, 0x0d
        jbe .return_true
        ret
    .return_true:
        cmp eax, eax
        ret

; Checks whether byte is start of identifier
; In: EAX - input byte, 3 high bytes should be zero
; Out: ZF=1 iff input is start of identifier
;      EAX - preserves input
is_identifier_start:
        ; Identifiear can start with a-z A-Z _
        cmp eax, '_'
        jne .range1_low
        ret
    .range1_low:
        cmp eax, 'a'
        jae .range1_high
        jmp .range2_low
    .range1_high:
        cmp eax, 'z'
        jbe .return_true
        ; jmp .range2_low
    .range2_low:
        cmp eax, 'A'
        jae .range2_high
        ret
    .range2_high:
        cmp eax, 'Z'
        jbe .return_true
        ret
    .return_true:
        cmp eax, eax
        ret

; Checks whether byte is part of identifier
; In: EAX - input byte, 3 high bytes should be zero
; Out: ZF=1 iff input is part of identifier
;      EAX - preserves input
is_identifier_char:
        ; Identifier consists of a-z A-Z 0-9 _
        cmp eax, '_'
        jne .range1_low
        ret
    .range1_low:
        cmp eax, 'a'
        jae .range1_high
        jmp .range2_low
    .range1_high:
        cmp eax, 'z'
        jbe .return_true
        ; jmp .range2_low
    .range2_low:
        cmp eax, 'A'
        jae .range2_high
        jmp .range3_low
    .range2_high:
        cmp eax, 'Z'
        jbe .return_true
        ; jmp .range3_low
    .range3_low:
        cmp eax, '0'
        jae .range3_high
        ret
    .range3_high:
        cmp eax, '9'
        jbe .return_true
        ret
    .return_true:
        cmp eax, eax
        ret

; Checks whether char is decimal digit
; In: EAX - input byte, 3 high bytes should be zero
; Out: ZF=1 iff input is decimal digit
;      EAX - preserves input
is_digit:
        cmp eax, '0'
        jae .range_high
        ret
    .range_high:
        cmp eax, '9'
        jbe .return_true
        ret
    .return_true:
        cmp eax, eax
        ret

; Converts decimal digit from char to numeric value
; In: EAX - input byte, 3 high bytes should be zero
; Out: ZF=1 iff input is decimal digit
;      EAX - numeric value
digit_to_int:
        cmp eax, '0'
        jae .range_high
        ret
    .range_high:
        cmp eax, '9'
        jbe .return_true
        ret
    .return_true:
        sub eax, '0'
        cmp eax, eax
        ret

; Checks whether char is hexadecimal digit
; In: EAX - input byte, 3 high bytes should be zero
; Out: ZF=1 iff input is hexadecimal digit
;      EAX - preserves input
is_hex_digit:
        cmp eax, '0'
        jb .alpha_lower
        cmp eax, '9'
        ja .alpha_lower
        cmp eax, eax
        ret
    .alpha_lower:
        cmp eax, 'a'
        jb .alpha_upper
        cmp eax, 'f'
        ja .alpha_upper
        cmp eax, eax
        ret
    .alpha_upper:
        cmp eax, 'A'
        jb .false
        cmp eax, 'F'
        ja .false
        cmp eax, eax
        ret
    .false:
        ret

; Converts hexadecimal digit from char to numeric value
; In: EAX - input byte, 3 high bytes should be zero
; Out: ZF=1 iff input is hexadecimal digit
;      EAX - numeric value
hex_digit_to_int:
        cmp eax, '0'
        jb .alpha_lower
        cmp eax, '9'
        ja .alpha_lower
        sub eax, '0'
        cmp eax, eax
        ret
    .alpha_lower:
        cmp eax, 'a'
        jb .alpha_upper
        cmp eax, 'f'
        ja .alpha_upper
        sub eax, 'a'-10
        cmp eax, eax
        ret
    .alpha_upper:
        cmp eax, 'A'
        jb .false
        cmp eax, 'F'
        ja .false
        sub eax, 'A'-10
        cmp eax, eax
        ret
    .false:
        ret

; Checks whether char is part of operator
; In: EAX - input byte, 3 high bytes should be zero
; Out: ZF=1 iff input is part of operator
;      EAX - preserves input
; Notes: trashes EDX value
is_operator_char:
        movzx edx, byte [is_operator_char_table+eax]
        cmp edx, 1
        ret

; Checks whether char consitutes a separate lexeme
; In: EAX - input byte, 3 high bytes should be zero
; Out: ZF=1 iff input consitutes a separate lexeme
;      EAX - preserves input
; Notes: trashes EDX value
is_single_char:
        movzx edx, byte [is_single_char_table+eax]
        cmp edx, 1
        ret

; In: EAX - char after '\', 3 high bytes should be zero
; Out: EAX - special char corresponding to the code
;      ZF - 1 iff successfully coonverted
convert_special_char:
        cmp eax, 't'
        jnz .char1
        mov eax, 0x09
        ret
    .char1:
        cmp eax, 'n'
        jnz .char2
        mov eax, 0x0a
        ret
    .char2:
        cmp eax, "'"
        jnz .char3
        ret
    .char3:
        cmp eax, '"'
        jnz .char4
        ret
    .char4:
        cmp eax, '\'
        jnz .char5
        ret
    .char5:
        cmp eax, '0'
        jnz .char6
        mov eax, 0
        ret
    .char6:
        ret
