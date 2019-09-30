; Main compiler driver
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

; Drives the whole compilation process
; In: 1) Address of command line
;     2) Length of command line
; Out: EAX - program execution result, 0 means success
drive:
        push ebp
        mov ebp, esp

        push dword [ebp+12]
        push dword [ebp+8]
        call print
        add esp, 8

        call newline

        push done_len
        push done_str
        call print

        xor eax, eax
        mov esp, ebp
        pop ebp
        ret
