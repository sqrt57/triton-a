
format PE console
entry start

section '.data' data readable writable
        hello_str db 'Hello, world!', 0ah
        hello_len = $-hello_str

        align 4
        stdin dd ?
        bytes_written dd ?

section '.text' code executable readable

start:
        push -11
        call [GetStdHandle]
        mov [stdin], eax

        push 0
        push bytes_written
        push hello_len
        push hello_str
        push [stdin]
        call [WriteFile]

        push 0
        call [ExitProcess]

section '.idata' import data readable writable

        dd 0, 0, -1, rva kernel32.name, rva kernel32.iat
        dd 0, 0, 0, 0, 0

        kernel32.name db 'kernel32.dll', 0

        align 4
        label kernel32.iat dword
        ExitProcess     dd rva ExitProcess.hint
        GetStdHandle    dd rva GetStdHandle.hint
        WriteFile       dd rva WriteFile.hint
        dd 0

        ExitProcess.hint        db 0, 0, 'ExitProcess', 0
        GetStdHandle.hint       db 0, 0, 'GetStdHandle', 0
        WriteFile.hint          db 0, 0, 'WriteFile', 0
