; String messages
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

    newline_str db 0x0a
    newline_len = 1

    done_str db 'Done.', 0x0a
    done_len = $-done_str

    quote_str db '"'
    quote_len = $-quote_str
