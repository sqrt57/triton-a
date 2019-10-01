mkdir build
del /Q build\*.*
fasm\fasm src\trac.asm build\trac.exe -s build\trac.fas
fasm-tools\listing.exe -a build\trac.fas build\trac.lst
fasm-tools\prepsrc.exe build\trac.fas build\trac.pre
fasm-tools\symbols.exe build\trac.fas build\trac.sym
