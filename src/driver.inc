; Main compiler driver
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

; Drives the whole compilation process
; In: 1) Address of command line
;     2) Length of command line
; Out: EAX - program exit code, 0 means success
drive:
        .cmdline equ [ebp+8]
        .cmdline_len equ [ebp+12]
        .cur equ esi
        .end equ [ebp-4]

        push ebp
        mov ebp, esp
        sub esp, 4
        push esi

        ; Print command line
        push dword .cmdline_len
        push dword .cmdline
        call print
        add esp, 4

        call newline

        ; Initialize locals
        mov eax, .cmdline
        mov .cur, eax
        add eax, .cmdline_len
        mov .end, eax

    .arg_loop:
        ; Get arg
        call drive_get_arg
        ; Check for end
        cmp eax, 0
        je .done
        ; Push result
        push ecx
        push eax
        ; Print quote
        push quote_len
        push quote_str
        call print
        add esp, 8
        ; Print arg
        call print
        add esp, 8
        ; Print quote
        push quote_len
        push quote_str
        call print
        add esp, 8
        ; Print newline
        call newline
        ; Loop again
        jmp .arg_loop

    .done:
        ; Print "Done"
        push done_len
        push done_str
        call print
        add esp, 8

        pop esi
        xor eax, eax
        mov esp, ebp
        pop ebp
        ret

; Inner function of drive
; Gets next argument from command line
; Uses .cur, .end
; Out: EAX - address of argument
;      ECX - length of argument
drive_get_arg:
        xor eax, eax
    .skip_whitespace:
        cmp .cur, .end
        jae .ret_null
        mov edx, .cur
        lodsb
        cmp eax, ' '
        je .skip_whitespace
        cmp eax, '\t'
        je .skip_whitespace
        cmp eax, '"'
        jne .unquoted_loop
        mov edx, .cur
        cmp .cur, .end
        jae .found_end
        lodsb
    .quoted_loop:
        cmp .cur, .end
        jae .found_end
        cmp eax, '"'
        je .done
        lodsb
        jmp .quoted_loop
    .unquoted_loop:
        cmp .cur, .end
        jae .found_end
        cmp eax, ' '
        je .done
        cmp eax, '\t'
        je .done
        lodsb
        jmp .unquoted_loop
    .found_end:
        mov eax, edx
        mov ecx, .cur
        sub ecx, eax
        ret
    .done:
        mov eax, edx
        mov ecx, .cur
        sub ecx, eax
        dec ecx
        ret
    .ret_null:
        xor eax, eax
        xor ecx, ecx
        ret
