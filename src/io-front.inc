; Frontend I/O utilities
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

; Prints newline to stdandard output
newline:
        push dword newline_len
        push dword newline_str
        call print
        add esp, 8
        ret

; Prints quote to stdandard output
print_quote:
        push dword quote_len
        push dword quote_str
        call print
        add esp, 8
        ret

; Prints quote to stdandard output
print_apostrophe:
        push dword apostrophe_len
        push dword apostrophe_str
        call print
        add esp, 8
        ret

; Prints unsigned number to standard output
; In: 1) Unsigned integer value
print_u32:
        .number equ dword [ebp+8]
        .buffer_end equ dword [ebp]
        .buffer_last_byte equ dword [ebp-1]
        .buffer equ dword [ebp-12]

        push ebp
        mov ebp, esp
        sub esp, 12
        push edi
        mov eax, .number
        mov ecx, 10
        lea edi, .buffer_last_byte
    .digit:
        xor edx, edx
        div ecx
        add edx, '0'
        mov [edi], dl
        dec edi
        test eax, eax
        jnz .digit

        lea eax, .buffer_end
        sub eax, edi
        inc edi
        push eax
        push edi
        call print
        add esp, 8

        pop edi
        mov esp, ebp
        pop ebp
        ret

; Prints signed number to standard output
; In: 1) Signed integer value
print_s32:
        .number equ dword [ebp+8]
        .buffer_end equ dword [ebp]
        .buffer_last_byte equ dword [ebp-1]
        .buffer equ dword [ebp-12]
        .sign equ dword [ebp-16]

        push ebp
        mov ebp, esp
        sub esp, 16
        push edi
        mov eax, .number
        mov .sign, 0
        test eax, eax
        jns .positive
        dec .sign
        neg eax
    .positive:
        mov ecx, 10
        lea edi, .buffer_last_byte
    .digit:
        xor edx, edx
        div ecx
        add edx, '0'
        mov [edi], dl
        dec edi
        test eax, eax
        jnz .digit

        cmp .sign, 0
        jz .done
        mov [edi], byte '-'
        dec edi

    .done:
        lea eax, .buffer_end
        sub eax, edi
        inc edi
        push eax
        push edi
        call print
        add esp, 8

        pop edi
        mov esp, ebp
        pop ebp
        ret
