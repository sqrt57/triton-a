; Windows-specific stuff, entry point
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

format PE console
entry start

section '.rdata' data readable
include 'messages.inc'

section '.data' data readable writable
include 'variables.inc'
        stdin dd ?
        bytes_written dd ?

section '.text' code executable readable

; Entry point
start:
        nop
        cld ; Clear direction flag according to calling conventions

        push -11
        call [GetStdHandle]
        mov [stdin], eax

        call [GetCommandLineA]
        push eax

        ; Calculate length of command line
        push eax
        call length
        add esp, 4

        ; Call driver
        pop ecx
        push eax
        push ecx
        call drive
        add esp, 8

        push eax
        call [ExitProcess]

; Prints string to stdandard output
; In: 1) Address of string
;     2) Length of string
print:
        push ebp
        mov ebp, esp
        push 0
        push bytes_written
        push dword [ebp+12]
        push dword [ebp+8]
        push [stdin]
        call [WriteFile]
        mov esp, ebp
        pop ebp
        ret

; Prints newline to stdandard output
newline:
        push 0
        push bytes_written
        push dword newline_len
        push dword newline_str
        push [stdin]
        call [WriteFile]
        ret

include 'driver.inc'
include 'string.inc'

section '.idata' import data readable writable

        dd 0, 0, 0, rva kernel32.name, rva kernel32.iat
        dd 0, 0, 0, 0, 0

        kernel32.name db 'kernel32.dll', 0

        align 4
    kernel32.iat:
        ExitProcess     dd rva ExitProcess.hint
        GetCommandLineA dd rva GetCommandLineA.hint
        GetStdHandle    dd rva GetStdHandle.hint
        WriteFile       dd rva WriteFile.hint
        dd 0

        ExitProcess.hint        db 0, 0, 'ExitProcess', 0
        GetCommandLineA.hint    db 0, 0, 'GetCommandLineA', 0
        GetStdHandle.hint       db 0, 0, 'GetStdHandle', 0
        WriteFile.hint          db 0, 0, 'WriteFile', 0
