SECTION .text		; code section
global main		  ; make label available to linker
SYS_write:
  push rbp
  mov rbp, rsp
  sub rsp, 20
  mov dword [rbp-4], eax
  mov qword [rbp-12], rdi
  mov dword [rbp-16], esi
  mov rax, 1
  movsx rdi, dword [rbp-4]
  mov rsi, qword [rbp-12]
  movsx rdx, dword [rbp-16]
  syscall
  mov dword [rbp-24], eax
.exit:
  mov eax, dword [rbp-24]
  add rsp, 20
  pop rbp
  ret
SYS_read:
  push rbp
  mov rbp, rsp
  sub rsp, 20
  mov dword [rbp-4], eax
  mov qword [rbp-12], rdi
  mov dword [rbp-16], esi
  mov rax, 0
  movsx rdi, dword [rbp-4]
  mov rsi, qword [rbp-12]
  movsx rdx, dword [rbp-16]
  syscall
  mov dword [rbp-24], eax
.exit:
  mov eax, dword [rbp-24]
  add rsp, 20
  pop rbp
  ret
SYS_openfd:
  push rbp
  mov rbp, rsp
  sub rsp, 20
  mov qword [rbp-8], rax
  mov dword [rbp-12], edi
  mov dword [rbp-16], esi
  mov rax, 2
  mov rdi, qword [rbp-8]
  movsx rsi, dword [rbp-12]
  movsx rdx, dword [rbp-16]
  syscall
  mov dword [rbp-24], eax
.exit:
  mov eax, dword [rbp-24]
  add rsp, 20
  pop rbp
  ret
SYS_closefd:
  push rbp
  mov rbp, rsp
  sub rsp, 4
  mov dword [rbp-4], eax
  mov rax, 3
  movsx rdi, dword [rbp-4]
  syscall
.exit:
  add rsp, 4
  pop rbp
  ret
SYS_fstat:
  push rbp
  mov rbp, rsp
  sub rsp, 12
  mov dword [rbp-4], eax
  mov qword [rbp-12], rdi
  mov rax, 5
  movsx rdi, dword [rbp-4]
  mov rsi, qword [rbp-12]
  syscall
.exit:
  add rsp, 12
  pop rbp
  ret
SYS_exit:
  push rbp
  mov rbp, rsp
  sub rsp, 4
  mov dword [rbp-4], eax
  mov rax, 60
  movsx rdi, dword [rbp-4]
  syscall
.exit:
  add rsp, 4
  pop rbp
  ret
sizeof_file:
  push rbp
  mov rbp, rsp
  sub rsp, 176
  mov dword [rbp-4], eax
  lea rax, [rbp-152]
  mov qword [rbp-160], rax
  movsx rax, dword [rbp-4]
  mov rdi, qword [rbp-160]
  call SYS_fstat
  mov rax, qword [rbp-160]
  mov rdx, 48
  add rax, rdx
  mov qword [rbp-160], rax
  mov rax, qword [rbp-160]
  mov edx, dword [rax]
  mov dword [rbp-164], edx
.exit:
  mov eax, dword [rbp-164]
  add rsp, 176
  pop rbp
  ret
printu:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov dword [rbp-4], eax
  mov dword [rbp-8], edi
  mov    eax, edi
  mov    ecx, 10
  push   rcx
  mov    rsi, rsp
  sub    rsp, 16
.toascii_digit:
  xor    edx, edx
  div    ecx
  add    edx, 48
  dec    rsi
  mov    [rsi], dl
  test   eax,eax
  jnz  .toascii_digit
  mov    eax, 1
  lea    edi, [rsp+16]
  sub    edi, esi
  mov    rax, rsi
  mov    esi, edi
  mov    rdi, rax
  mov    eax, dword [rbp-4]
  call SYS_write
  add rsp, 24
.exit:
  add rsp, 24
  pop rbp
  ret
printi:
  push rbp
  mov rbp, rsp
  sub rsp, 37
  mov dword [rbp-4], eax
  mov dword [rbp-8], edi
  mov eax, dword [rbp-8]
  mov edx, 4294967294
  cmp eax, edx
  jge .if1
  mov eax, 4294967294
  mov edx, dword [rbp-8]
  sub eax, edx
  mov dword [rbp-8], eax
  mov eax, dword [rbp-8]
  mov edx, 2
  add eax, edx
  mov dword [rbp-8], eax
  mov byte [rbp-13], "-"
  lea rax, [rbp-13]
  mov qword [rbp-21], rax
  movsx rax, dword [rbp-4]
  mov rdi, qword [rbp-21]
  mov rsi, 1
  call SYS_write
.if1:
  movsx rax, dword [rbp-4]
  movsx rdi, dword [rbp-8]
  call printu
  mov dword [rbp-25], eax
.exit:
  mov eax, dword [rbp-25]
  add rsp, 37
  pop rbp
  ret
printc:
  push rbp
  mov rbp, rsp
  sub rsp, 33
  mov dword [rbp-4], eax
  mov byte [rbp-5], dil
  lea rax, [rbp-5]
  mov qword [rbp-17], rax
  movsx rax, dword [rbp-4]
  mov rdi, qword [rbp-17]
  mov rsi, 1
  call SYS_write
  mov dword [rbp-21], eax
.exit:
  mov eax, dword [rbp-21]
  add rsp, 33
  pop rbp
  ret
strlen:
  push rbp
  mov rbp, rsp
  sub rsp, 21
  mov qword [rbp-8], rax
  mov rax, qword [rbp-8]
  mov qword [rbp-20], rax
  mov rax, qword [rbp-8]
  mov dl, byte [rax]
  mov byte [rbp-21], dl
  jmp .whilecmp1
.while1:
  inc qword [rbp-8]
  mov rax, qword [rbp-8]
  mov dl, byte [rax]
  mov byte [rbp-21], dl
.whilecmp1:
  cmp byte [rbp-21], 0
  jne .while1
  mov rax, qword [rbp-8]
  mov rdx, qword [rbp-20]
  sub rax, rdx
  mov dword [rbp-25], eax
.exit:
  mov eax, dword [rbp-25]
  add rsp, 21
  pop rbp
  ret
strcmp_speclens:
  push rbp
  mov rbp, rsp
  sub rsp, 30
  mov qword [rbp-8], rax
  mov dword [rbp-12], edi
  mov qword [rbp-20], rsi
  mov dword [rbp-24], edx
  mov eax, dword [rbp-12]
  mov edx, dword [rbp-24]
  cmp eax, edx
  sete al
  mov byte [rbp-31], al
  cmp byte [rbp-31], 0
  je .if2
  mov byte [rbp-26], 1
  mov byte [rbp-27], 0
  jmp .whilecmp2
.while2:
  mov rax, qword [rbp-8]
  mov dl, byte [rax]
  mov byte [rbp-28], dl
  mov rax, qword [rbp-20]
  mov dl, byte [rax]
  mov byte [rbp-29], dl
  mov al, byte [rbp-28]
  mov dl, byte [rbp-29]
  cmp al, dl
  sete al
  mov byte [rbp-31], al
  movsx eax, byte [rbp-27]
  mov edx, dword [rbp-12]
  cmp eax, edx
  setl al
  mov byte [rbp-30], al
  mov al, byte [rbp-31]
  mov dl, byte [rbp-30]
  and al, dl
  shr al, 1
  setc al
  mov byte [rbp-26], al
  inc byte [rbp-27]
  inc qword [rbp-8]
  inc qword [rbp-20]
.whilecmp2:
  cmp byte [rbp-26], 0
  jne .while2
.if2:
.exit:
  mov al, byte [rbp-31]
  add rsp, 30
  pop rbp
  ret
strcpy:
  push rbp
  mov rbp, rsp
  sub rsp, 17
  mov qword [rbp-8], rax
  mov qword [rbp-16], rdi
  mov rax, qword [rbp-8]
  mov dl, byte [rax]
  mov byte [rbp-17], dl
  jmp .whilecmp3
.while3:
  mov rax, qword [rbp-16]
  mov dl, byte [rbp-17]
  mov byte [rax], dl
  inc qword [rbp-8]
  inc qword [rbp-16]
  mov rax, qword [rbp-8]
  mov dl, byte [rax]
  mov byte [rbp-17], dl
.whilecmp3:
  cmp byte [rbp-17], 0
  jne .while3
  mov rax, qword [rbp-16]
  mov edx, 0
  mov dword [rax], edx
.exit:
  add rsp, 17
  pop rbp
  ret
strcpylen:
  push rbp
  mov rbp, rsp
  sub rsp, 25
  mov qword [rbp-8], rax
  mov qword [rbp-16], rdi
  mov dword [rbp-20], esi
  mov rax, qword [rbp-8]
  mov dl, byte [rax]
  mov byte [rbp-21], dl
  mov dword [rbp-25], 0
  jmp .whilecmp4
.while4:
  mov rax, qword [rbp-16]
  mov dl, byte [rbp-21]
  mov byte [rax], dl
  inc qword [rbp-8]
  inc qword [rbp-16]
  mov rax, qword [rbp-8]
  mov dl, byte [rax]
  mov byte [rbp-21], dl
  inc dword [rbp-25]
.whilecmp4:
  mov eax, dword [rbp-25]
  mov edx, dword [rbp-20]
  cmp eax, edx
  jl .while4
  mov rax, qword [rbp-16]
  mov edx, 0
  mov dword [rax], edx
.exit:
  add rsp, 25
  pop rbp
  ret
prints:
  push rbp
  mov rbp, rsp
  sub rsp, 36
  mov dword [rbp-4], eax
  mov qword [rbp-12], rdi
  mov rax, qword [rbp-12]
  call strlen
  mov dword [rbp-20], eax
  movsx rax, dword [rbp-4]
  mov rdi, qword [rbp-12]
  movsx rsi, dword [rbp-20]
  call SYS_write
  mov dword [rbp-24], eax
.exit:
  mov eax, dword [rbp-24]
  add rsp, 36
  pop rbp
  ret
__print_error:
  push rbp
  mov rbp, rsp
  sub rsp, 296
  mov dword [rbp-4], eax
  mov eax, 4294967294
  mov edx, dword [rbp-4]
  sub eax, edx
  mov dword [rbp-4], eax
  mov eax, dword [rbp-4]
  mov edx, 2
  add eax, edx
  mov dword [rbp-4], eax
  lea rax, [rbp-260]
  mov qword [rbp-268], rax
  mov qword [rbp-276], 0
  mov dword [rbp-280], 0
  jmp .whilecmp5
.while5:
  mov rax, qword [rbp-268]
  mov rdx, qword [rbp-276]
  mov qword [rax], rdx
  mov rax, qword [rbp-268]
  mov rdx, 8
  add rax, rdx
  mov qword [rbp-268], rax
  mov eax, dword [rbp-280]
  mov edx, 8
  add eax, edx
  mov dword [rbp-280], eax
.whilecmp5:
  mov eax, dword [rbp-280]
  mov edx, 256
  cmp eax, edx
  jl .while5
  lea rax, [rbp-260]
  mov qword [rbp-268], rax
  mov eax, dword [rbp-4]
  mov edx, 1
  cmp eax, edx
  jne .if3
  mov rax, "Operatio"
  mov qword [rbp-260], rax
  mov rax, "n not pe"
  mov qword [rbp-252], rax
  mov rax, "rmitted."
  mov qword [rbp-244], rax
  jmp .escape4
.if3:
  mov eax, dword [rbp-4]
  mov edx, 2
  cmp eax, edx
  jne .elif1
  mov rax, "No such "
  mov qword [rbp-260], rax
  mov rax, "file or "
  mov qword [rbp-252], rax
  mov rax, "director"
  mov qword [rbp-244], rax
  mov ax, "y."
  mov word [rbp-236], ax
  jmp .escape4
.elif1:
  mov eax, dword [rbp-4]
  mov edx, 3
  cmp eax, edx
  jne .elif2
  mov rax, "No such "
  mov qword [rbp-260], rax
  mov rax, "process."
  mov qword [rbp-252], rax
  jmp .escape4
.elif2:
  mov eax, dword [rbp-4]
  mov edx, 4
  cmp eax, edx
  jne .elif3
  mov rax, "Interrup"
  mov qword [rbp-260], rax
  mov rax, "ted syst"
  mov qword [rbp-252], rax
  mov rax, "em call."
  mov qword [rbp-244], rax
  jmp .escape4
.elif3:
  mov eax, dword [rbp-4]
  mov edx, 5
  cmp eax, edx
  jne .elif4
  mov rax, "I/O erro"
  mov qword [rbp-260], rax
  mov ax, "r."
  mov word [rbp-252], ax
  jmp .escape4
.elif4:
  mov eax, dword [rbp-4]
  mov edx, 6
  cmp eax, edx
  jne .elif5
  mov rax, "No such "
  mov qword [rbp-260], rax
  mov rax, "device o"
  mov qword [rbp-252], rax
  mov rax, "r addres"
  mov qword [rbp-244], rax
  mov ax, "s."
  mov word [rbp-236], ax
  jmp .escape4
.elif5:
  mov eax, dword [rbp-4]
  mov edx, 7
  cmp eax, edx
  jne .elif6
  mov rax, "Argument"
  mov qword [rbp-260], rax
  mov rax, " list to"
  mov qword [rbp-252], rax
  mov eax, "o lo"
  mov dword [rbp-244], eax
  mov ax, "ng"
  mov word [rbp-240], ax
  mov al, "."
  mov byte [rbp-238], al
  jmp .escape4
.elif6:
  mov eax, dword [rbp-4]
  mov edx, 8
  cmp eax, edx
  jne .elif7
  mov rax, "Exec for"
  mov qword [rbp-260], rax
  mov rax, "mat erro"
  mov qword [rbp-252], rax
  mov ax, "r."
  mov word [rbp-244], ax
  jmp .escape4
.elif7:
  mov eax, dword [rbp-4]
  mov edx, 9
  cmp eax, edx
  jne .elif8
  mov rax, "Bad file"
  mov qword [rbp-260], rax
  mov rax, " number."
  mov qword [rbp-252], rax
  jmp .escape4
.elif8:
  mov eax, dword [rbp-4]
  mov edx, 10
  cmp eax, edx
  jne .elif9
  mov rax, "No child"
  mov qword [rbp-260], rax
  mov rax, " process"
  mov qword [rbp-252], rax
  mov ax, "es"
  mov word [rbp-244], ax
  mov al, "."
  mov byte [rbp-242], al
  jmp .escape4
.elif9:
  mov eax, dword [rbp-4]
  mov edx, 11
  cmp eax, edx
  jne .elif10
  mov rax, "Try agai"
  mov qword [rbp-260], rax
  mov ax, "n."
  mov word [rbp-252], ax
  jmp .escape4
.elif10:
  mov eax, dword [rbp-4]
  mov edx, 12
  cmp eax, edx
  jne .elif11
  mov rax, "Out of m"
  mov qword [rbp-260], rax
  mov eax, "emor"
  mov dword [rbp-252], eax
  mov ax, "y."
  mov word [rbp-248], ax
  jmp .escape4
.elif11:
  mov eax, dword [rbp-4]
  mov edx, 13
  cmp eax, edx
  jne .elif12
  mov rax, "Permissi"
  mov qword [rbp-260], rax
  mov rax, "on denie"
  mov qword [rbp-252], rax
  mov ax, "d."
  mov word [rbp-244], ax
  jmp .escape4
.elif12:
  mov eax, dword [rbp-4]
  mov edx, 14
  cmp eax, edx
  jne .elif13
  mov rax, "Bad addr"
  mov qword [rbp-260], rax
  mov eax, "ess."
  mov dword [rbp-252], eax
  jmp .escape4
.elif13:
  mov eax, dword [rbp-4]
  mov edx, 15
  cmp eax, edx
  jne .elif14
  mov rax, "Block de"
  mov qword [rbp-260], rax
  mov rax, "vice req"
  mov qword [rbp-252], rax
  mov eax, "uire"
  mov dword [rbp-244], eax
  mov ax, "d."
  mov word [rbp-240], ax
  jmp .escape4
.elif14:
  mov eax, dword [rbp-4]
  mov edx, 16
  cmp eax, edx
  jne .elif15
  mov rax, "Device o"
  mov qword [rbp-260], rax
  mov rax, "r resour"
  mov qword [rbp-252], rax
  mov rax, "ce busy."
  mov qword [rbp-244], rax
  jmp .escape4
.elif15:
  mov eax, dword [rbp-4]
  mov edx, 17
  cmp eax, edx
  jne .elif16
  mov rax, "File exi"
  mov qword [rbp-260], rax
  mov eax, "sts."
  mov dword [rbp-252], eax
  jmp .escape4
.elif16:
  mov eax, dword [rbp-4]
  mov edx, 18
  cmp eax, edx
  jne .elif17
  mov rax, "Cross-de"
  mov qword [rbp-260], rax
  mov rax, "vice lin"
  mov qword [rbp-252], rax
  mov ax, "k."
  mov word [rbp-244], ax
  jmp .escape4
.elif17:
  mov eax, dword [rbp-4]
  mov edx, 19
  cmp eax, edx
  jne .elif18
  mov rax, "No such "
  mov qword [rbp-260], rax
  mov eax, "devi"
  mov dword [rbp-252], eax
  mov ax, "ce"
  mov word [rbp-248], ax
  mov al, "."
  mov byte [rbp-246], al
  jmp .escape4
.elif18:
  mov eax, dword [rbp-4]
  mov edx, 20
  cmp eax, edx
  jne .elif19
  mov rax, "Not a di"
  mov qword [rbp-260], rax
  mov rax, "rectory."
  mov qword [rbp-252], rax
  jmp .escape4
.elif19:
  mov eax, dword [rbp-4]
  mov edx, 21
  cmp eax, edx
  jne .elif20
  mov rax, "Is a dir"
  mov qword [rbp-260], rax
  mov eax, "ecto"
  mov dword [rbp-252], eax
  mov ax, "ry"
  mov word [rbp-248], ax
  mov al, "."
  mov byte [rbp-246], al
  jmp .escape4
.elif20:
  mov eax, dword [rbp-4]
  mov edx, 22
  cmp eax, edx
  jne .elif21
  mov rax, "Invalid "
  mov qword [rbp-260], rax
  mov rax, "argument"
  mov qword [rbp-252], rax
  mov al, "."
  mov byte [rbp-244], al
  jmp .escape4
.elif21:
  mov eax, dword [rbp-4]
  mov edx, 23
  cmp eax, edx
  jne .elif22
  mov rax, "File tab"
  mov qword [rbp-260], rax
  mov rax, "le overf"
  mov qword [rbp-252], rax
  mov eax, "low."
  mov dword [rbp-244], eax
  jmp .escape4
.elif22:
  mov eax, dword [rbp-4]
  mov edx, 24
  cmp eax, edx
  jne .elif23
  mov rax, "Too many"
  mov qword [rbp-260], rax
  mov rax, " open fi"
  mov qword [rbp-252], rax
  mov eax, "les."
  mov dword [rbp-244], eax
  jmp .escape4
.elif23:
  mov eax, dword [rbp-4]
  mov edx, 25
  cmp eax, edx
  jne .elif24
  mov rax, "Not a ty"
  mov qword [rbp-260], rax
  mov rax, "pewriter"
  mov qword [rbp-252], rax
  mov al, "."
  mov byte [rbp-244], al
  jmp .escape4
.elif24:
  mov eax, dword [rbp-4]
  mov edx, 26
  cmp eax, edx
  jne .elif25
  mov rax, "Text fil"
  mov qword [rbp-260], rax
  mov eax, "e bu"
  mov dword [rbp-252], eax
  mov ax, "sy"
  mov word [rbp-248], ax
  mov al, "."
  mov byte [rbp-246], al
  jmp .escape4
.elif25:
  mov eax, dword [rbp-4]
  mov edx, 27
  cmp eax, edx
  jne .elif26
  mov rax, "File too"
  mov qword [rbp-260], rax
  mov eax, " lar"
  mov dword [rbp-252], eax
  mov ax, "ge"
  mov word [rbp-248], ax
  mov al, "."
  mov byte [rbp-246], al
  jmp .escape4
.elif26:
  mov eax, dword [rbp-4]
  mov edx, 28
  cmp eax, edx
  jne .elif27
  mov rax, "No space"
  mov qword [rbp-260], rax
  mov rax, " left on"
  mov qword [rbp-252], rax
  mov rax, " device."
  mov qword [rbp-244], rax
  jmp .escape4
.elif27:
  mov eax, dword [rbp-4]
  mov edx, 29
  cmp eax, edx
  jne .elif28
  mov rax, "Illegal "
  mov qword [rbp-260], rax
  mov eax, "seek"
  mov dword [rbp-252], eax
  mov al, "."
  mov byte [rbp-248], al
  jmp .escape4
.elif28:
  mov eax, dword [rbp-4]
  mov edx, 30
  cmp eax, edx
  jne .elif29
  mov rax, "Read-onl"
  mov qword [rbp-260], rax
  mov rax, "y file s"
  mov qword [rbp-252], rax
  mov eax, "yste"
  mov dword [rbp-244], eax
  mov ax, "m."
  mov word [rbp-240], ax
  jmp .escape4
.elif29:
  mov eax, dword [rbp-4]
  mov edx, 31
  cmp eax, edx
  jne .elif30
  mov rax, "Too many"
  mov qword [rbp-260], rax
  mov eax, " lin"
  mov dword [rbp-252], eax
  mov ax, "ks"
  mov word [rbp-248], ax
  mov al, "."
  mov byte [rbp-246], al
  jmp .escape4
.elif30:
  mov eax, dword [rbp-4]
  mov edx, 32
  cmp eax, edx
  jne .elif31
  mov rax, "Broken p"
  mov qword [rbp-260], rax
  mov eax, "ipe."
  mov dword [rbp-252], eax
  jmp .escape4
.elif31:
  mov eax, dword [rbp-4]
  mov edx, 33
  cmp eax, edx
  jne .elif32
  mov rax, "Math arg"
  mov qword [rbp-260], rax
  mov rax, "ument ou"
  mov qword [rbp-252], rax
  mov rax, "t of dom"
  mov qword [rbp-244], rax
  mov rax, "ain of f"
  mov qword [rbp-236], rax
  mov eax, "unc."
  mov dword [rbp-228], eax
  jmp .escape4
.elif32:
  mov eax, dword [rbp-4]
  mov edx, 34
  cmp eax, edx
  jne .elif33
  mov rax, "Math res"
  mov qword [rbp-260], rax
  mov rax, "ult not "
  mov qword [rbp-252], rax
  mov rax, "represen"
  mov qword [rbp-244], rax
  mov eax, "tabl"
  mov dword [rbp-236], eax
  mov ax, "e."
  mov word [rbp-232], ax
  jmp .escape4
.elif33:
  mov eax, dword [rbp-4]
  mov edx, 35
  cmp eax, edx
  jne .elif34
  mov rax, "Resource"
  mov qword [rbp-260], rax
  mov rax, " deadloc"
  mov qword [rbp-252], rax
  mov rax, "k would "
  mov qword [rbp-244], rax
  mov eax, "occu"
  mov dword [rbp-236], eax
  mov ax, "r."
  mov word [rbp-232], ax
  jmp .escape4
.elif34:
  mov eax, dword [rbp-4]
  mov edx, 36
  cmp eax, edx
  jne .elif35
  mov rax, "File nam"
  mov qword [rbp-260], rax
  mov rax, "e too lo"
  mov qword [rbp-252], rax
  mov ax, "ng"
  mov word [rbp-244], ax
  mov al, "."
  mov byte [rbp-242], al
  jmp .escape4
.elif35:
  mov eax, dword [rbp-4]
  mov edx, 37
  cmp eax, edx
  jne .elif36
  mov rax, "No recor"
  mov qword [rbp-260], rax
  mov rax, "d locks "
  mov qword [rbp-252], rax
  mov rax, "availabl"
  mov qword [rbp-244], rax
  mov ax, "e."
  mov word [rbp-236], ax
  jmp .escape4
.elif36:
  mov eax, dword [rbp-4]
  mov edx, 38
  cmp eax, edx
  jne .elif37
  mov rax, "Function"
  mov qword [rbp-260], rax
  mov rax, " not imp"
  mov qword [rbp-252], rax
  mov rax, "lemented"
  mov qword [rbp-244], rax
  mov al, "."
  mov byte [rbp-236], al
  jmp .escape4
.elif37:
  mov eax, dword [rbp-4]
  mov edx, 39
  cmp eax, edx
  jne .elif38
  mov rax, "Director"
  mov qword [rbp-260], rax
  mov rax, "y not em"
  mov qword [rbp-252], rax
  mov eax, "pty."
  mov dword [rbp-244], eax
  jmp .escape4
.elif38:
  mov eax, dword [rbp-4]
  mov edx, 40
  cmp eax, edx
  jne .elif39
  mov rax, "Too many"
  mov qword [rbp-260], rax
  mov rax, " symboli"
  mov qword [rbp-252], rax
  mov rax, "c links "
  mov qword [rbp-244], rax
  mov rax, "encounte"
  mov qword [rbp-236], rax
  mov eax, "red."
  mov dword [rbp-228], eax
  jmp .escape4
.elif39:
  mov eax, dword [rbp-4]
  mov edx, 42
  cmp eax, edx
  jne .elif40
  mov rax, "No messa"
  mov qword [rbp-260], rax
  mov rax, "ge of de"
  mov qword [rbp-252], rax
  mov rax, "sired ty"
  mov qword [rbp-244], rax
  mov ax, "pe"
  mov word [rbp-236], ax
  mov al, "."
  mov byte [rbp-234], al
  jmp .escape4
.elif40:
  mov eax, dword [rbp-4]
  mov edx, 43
  cmp eax, edx
  jne .elif41
  mov rax, "Identifi"
  mov qword [rbp-260], rax
  mov rax, "er remov"
  mov qword [rbp-252], rax
  mov ax, "ed"
  mov word [rbp-244], ax
  mov al, "."
  mov byte [rbp-242], al
  jmp .escape4
.elif41:
  mov eax, dword [rbp-4]
  mov edx, 44
  cmp eax, edx
  jne .elif42
  mov rax, "Channel "
  mov qword [rbp-260], rax
  mov rax, "number o"
  mov qword [rbp-252], rax
  mov rax, "ut of ra"
  mov qword [rbp-244], rax
  mov eax, "nge."
  mov dword [rbp-236], eax
  jmp .escape4
.elif42:
  mov eax, dword [rbp-4]
  mov edx, 45
  cmp eax, edx
  jne .elif43
  mov rax, "Level 2 "
  mov qword [rbp-260], rax
  mov rax, "not sync"
  mov qword [rbp-252], rax
  mov rax, "hronized"
  mov qword [rbp-244], rax
  mov al, "."
  mov byte [rbp-236], al
  jmp .escape4
.elif43:
  mov eax, dword [rbp-4]
  mov edx, 46
  cmp eax, edx
  jne .elif44
  mov rax, "Level 3 "
  mov qword [rbp-260], rax
  mov eax, "halt"
  mov dword [rbp-252], eax
  mov ax, "ed"
  mov word [rbp-248], ax
  mov al, "."
  mov byte [rbp-246], al
  jmp .escape4
.elif44:
  mov eax, dword [rbp-4]
  mov edx, 47
  cmp eax, edx
  jne .elif45
  mov rax, "Level 3 "
  mov qword [rbp-260], rax
  mov eax, "rese"
  mov dword [rbp-252], eax
  mov ax, "t."
  mov word [rbp-248], ax
  jmp .escape4
.elif45:
  mov eax, dword [rbp-4]
  mov edx, 48
  cmp eax, edx
  jne .elif46
  mov rax, "Link num"
  mov qword [rbp-260], rax
  mov rax, "ber out "
  mov qword [rbp-252], rax
  mov rax, "of range"
  mov qword [rbp-244], rax
  mov al, "."
  mov byte [rbp-236], al
  jmp .escape4
.elif46:
  mov eax, dword [rbp-4]
  mov edx, 49
  cmp eax, edx
  jne .elif47
  mov rax, "Protocol"
  mov qword [rbp-260], rax
  mov rax, " driver "
  mov qword [rbp-252], rax
  mov rax, "not atta"
  mov qword [rbp-244], rax
  mov eax, "ched"
  mov dword [rbp-236], eax
  mov al, "."
  mov byte [rbp-232], al
  jmp .escape4
.elif47:
  mov eax, dword [rbp-4]
  mov edx, 50
  cmp eax, edx
  jne .elif48
  mov rax, "No CSI s"
  mov qword [rbp-260], rax
  mov rax, "tructure"
  mov qword [rbp-252], rax
  mov rax, " availab"
  mov qword [rbp-244], rax
  mov ax, "le"
  mov word [rbp-236], ax
  mov al, "."
  mov byte [rbp-234], al
  jmp .escape4
.elif48:
  mov eax, dword [rbp-4]
  mov edx, 51
  cmp eax, edx
  jne .elif49
  mov rax, "Level 2 "
  mov qword [rbp-260], rax
  mov eax, "halt"
  mov dword [rbp-252], eax
  mov ax, "ed"
  mov word [rbp-248], ax
  mov al, "."
  mov byte [rbp-246], al
  jmp .escape4
.elif49:
  mov eax, dword [rbp-4]
  mov edx, 52
  cmp eax, edx
  jne .elif50
  mov rax, "Invalid "
  mov qword [rbp-260], rax
  mov rax, "exchange"
  mov qword [rbp-252], rax
  mov al, "."
  mov byte [rbp-244], al
  jmp .escape4
.elif50:
  mov eax, dword [rbp-4]
  mov edx, 53
  cmp eax, edx
  jne .elif51
  mov rax, "Invalid "
  mov qword [rbp-260], rax
  mov rax, "request "
  mov qword [rbp-252], rax
  mov rax, "descript"
  mov qword [rbp-244], rax
  mov ax, "or"
  mov word [rbp-236], ax
  mov al, "."
  mov byte [rbp-234], al
  jmp .escape4
.elif51:
  mov eax, dword [rbp-4]
  mov edx, 54
  cmp eax, edx
  jne .elif52
  mov rax, "Exchange"
  mov qword [rbp-260], rax
  mov eax, " ful"
  mov dword [rbp-252], eax
  mov ax, "l."
  mov word [rbp-248], ax
  jmp .escape4
.elif52:
  mov eax, dword [rbp-4]
  mov edx, 55
  cmp eax, edx
  jne .elif53
  mov rax, "No anode"
  mov qword [rbp-260], rax
  mov al, "."
  mov byte [rbp-252], al
  jmp .escape4
.elif53:
  mov eax, dword [rbp-4]
  mov edx, 56
  cmp eax, edx
  jne .elif54
  mov rax, "Invalid "
  mov qword [rbp-260], rax
  mov rax, "request "
  mov qword [rbp-252], rax
  mov eax, "code"
  mov dword [rbp-244], eax
  mov al, "."
  mov byte [rbp-240], al
  jmp .escape4
.elif54:
  mov eax, dword [rbp-4]
  mov edx, 57
  cmp eax, edx
  jne .elif55
  mov rax, "Invalid "
  mov qword [rbp-260], rax
  mov eax, "slot"
  mov dword [rbp-252], eax
  mov al, "."
  mov byte [rbp-248], al
  jmp .escape4
.elif55:
  mov eax, dword [rbp-4]
  mov edx, 59
  cmp eax, edx
  jne .elif56
  mov rax, "Bad font"
  mov qword [rbp-260], rax
  mov rax, " file fo"
  mov qword [rbp-252], rax
  mov eax, "rmat"
  mov dword [rbp-244], eax
  mov al, "."
  mov byte [rbp-240], al
  jmp .escape4
.elif56:
  mov eax, dword [rbp-4]
  mov edx, 60
  cmp eax, edx
  jne .elif57
  mov rax, "Device n"
  mov qword [rbp-260], rax
  mov rax, "ot a str"
  mov qword [rbp-252], rax
  mov eax, "eam."
  mov dword [rbp-244], eax
  jmp .escape4
.elif57:
  mov eax, dword [rbp-4]
  mov edx, 61
  cmp eax, edx
  jne .elif58
  mov rax, "No data "
  mov qword [rbp-260], rax
  mov rax, "availabl"
  mov qword [rbp-252], rax
  mov ax, "e."
  mov word [rbp-244], ax
  jmp .escape4
.elif58:
  mov eax, dword [rbp-4]
  mov edx, 62
  cmp eax, edx
  jne .elif59
  mov rax, "Timer ex"
  mov qword [rbp-260], rax
  mov eax, "pire"
  mov dword [rbp-252], eax
  mov ax, "d."
  mov word [rbp-248], ax
  jmp .escape4
.elif59:
  mov eax, dword [rbp-4]
  mov edx, 63
  cmp eax, edx
  jne .elif60
  mov rax, "Out of s"
  mov qword [rbp-260], rax
  mov rax, "treams r"
  mov qword [rbp-252], rax
  mov rax, "esources"
  mov qword [rbp-244], rax
  mov al, "."
  mov byte [rbp-236], al
  jmp .escape4
.elif60:
  mov eax, dword [rbp-4]
  mov edx, 64
  cmp eax, edx
  jne .elif61
  mov rax, "Machine "
  mov qword [rbp-260], rax
  mov rax, "is not o"
  mov qword [rbp-252], rax
  mov rax, "n the ne"
  mov qword [rbp-244], rax
  mov eax, "twor"
  mov dword [rbp-236], eax
  mov ax, "k."
  mov word [rbp-232], ax
  jmp .escape4
.elif61:
  mov eax, dword [rbp-4]
  mov edx, 65
  cmp eax, edx
  jne .elif62
  mov rax, "Package "
  mov qword [rbp-260], rax
  mov rax, "not inst"
  mov qword [rbp-252], rax
  mov eax, "alle"
  mov dword [rbp-244], eax
  mov ax, "d."
  mov word [rbp-240], ax
  jmp .escape4
.elif62:
  mov eax, dword [rbp-4]
  mov edx, 66
  cmp eax, edx
  jne .elif63
  mov rax, "Object i"
  mov qword [rbp-260], rax
  mov rax, "s remote"
  mov qword [rbp-252], rax
  mov al, "."
  mov byte [rbp-244], al
  jmp .escape4
.elif63:
  mov eax, dword [rbp-4]
  mov edx, 67
  cmp eax, edx
  jne .elif64
  mov rax, "Link has"
  mov qword [rbp-260], rax
  mov rax, " been se"
  mov qword [rbp-252], rax
  mov eax, "vere"
  mov dword [rbp-244], eax
  mov ax, "d."
  mov word [rbp-240], ax
  jmp .escape4
.elif64:
  mov eax, dword [rbp-4]
  mov edx, 68
  cmp eax, edx
  jne .elif65
  mov rax, "Advertis"
  mov qword [rbp-260], rax
  mov rax, "e error."
  mov qword [rbp-252], rax
  jmp .escape4
.elif65:
  mov eax, dword [rbp-4]
  mov edx, 69
  cmp eax, edx
  jne .elif66
  mov rax, "Srmount "
  mov qword [rbp-260], rax
  mov eax, "erro"
  mov dword [rbp-252], eax
  mov ax, "r."
  mov word [rbp-248], ax
  jmp .escape4
.elif66:
  mov eax, dword [rbp-4]
  mov edx, 70
  cmp eax, edx
  jne .elif67
  mov rax, "Communic"
  mov qword [rbp-260], rax
  mov rax, "ation er"
  mov qword [rbp-252], rax
  mov rax, "ror on s"
  mov qword [rbp-244], rax
  mov eax, "end."
  mov dword [rbp-236], eax
  jmp .escape4
.elif67:
  mov eax, dword [rbp-4]
  mov edx, 71
  cmp eax, edx
  jne .elif68
  mov rax, "Protocol"
  mov qword [rbp-260], rax
  mov eax, " err"
  mov dword [rbp-252], eax
  mov ax, "or"
  mov word [rbp-248], ax
  mov al, "."
  mov byte [rbp-246], al
  jmp .escape4
.elif68:
  mov eax, dword [rbp-4]
  mov edx, 72
  cmp eax, edx
  jne .elif69
  mov rax, "Multihop"
  mov qword [rbp-260], rax
  mov rax, " attempt"
  mov qword [rbp-252], rax
  mov ax, "ed"
  mov word [rbp-244], ax
  mov al, "."
  mov byte [rbp-242], al
  jmp .escape4
.elif69:
  mov eax, dword [rbp-4]
  mov edx, 73
  cmp eax, edx
  jne .elif70
  mov rax, "RFS spec"
  mov qword [rbp-260], rax
  mov rax, "ific err"
  mov qword [rbp-252], rax
  mov ax, "or"
  mov word [rbp-244], ax
  mov al, "."
  mov byte [rbp-242], al
  jmp .escape4
.elif70:
  mov eax, dword [rbp-4]
  mov edx, 74
  cmp eax, edx
  jne .elif71
  mov rax, "Not a da"
  mov qword [rbp-260], rax
  mov rax, "ta messa"
  mov qword [rbp-252], rax
  mov ax, "ge"
  mov word [rbp-244], ax
  mov al, "."
  mov byte [rbp-242], al
  jmp .escape4
.elif71:
  mov eax, dword [rbp-4]
  mov edx, 75
  cmp eax, edx
  jne .elif72
  mov rax, "Value to"
  mov qword [rbp-260], rax
  mov rax, "o large "
  mov qword [rbp-252], rax
  mov rax, "for defi"
  mov qword [rbp-244], rax
  mov rax, "ned data"
  mov qword [rbp-236], rax
  mov eax, " typ"
  mov dword [rbp-228], eax
  mov ax, "e."
  mov word [rbp-224], ax
  jmp .escape4
.elif72:
  mov eax, dword [rbp-4]
  mov edx, 76
  cmp eax, edx
  jne .elif73
  mov rax, "Name not"
  mov qword [rbp-260], rax
  mov rax, " unique "
  mov qword [rbp-252], rax
  mov rax, "on netwo"
  mov qword [rbp-244], rax
  mov ax, "rk"
  mov word [rbp-236], ax
  mov al, "."
  mov byte [rbp-234], al
  jmp .escape4
.elif73:
  mov eax, dword [rbp-4]
  mov edx, 77
  cmp eax, edx
  jne .elif74
  mov rax, "File des"
  mov qword [rbp-260], rax
  mov rax, "criptor "
  mov qword [rbp-252], rax
  mov rax, "in bad s"
  mov qword [rbp-244], rax
  mov eax, "tate"
  mov dword [rbp-236], eax
  mov al, "."
  mov byte [rbp-232], al
  jmp .escape4
.elif74:
  mov eax, dword [rbp-4]
  mov edx, 78
  cmp eax, edx
  jne .elif75
  mov rax, "Remote a"
  mov qword [rbp-260], rax
  mov rax, "ddress c"
  mov qword [rbp-252], rax
  mov eax, "hang"
  mov dword [rbp-244], eax
  mov ax, "ed"
  mov word [rbp-240], ax
  mov al, "."
  mov byte [rbp-238], al
  jmp .escape4
.elif75:
  mov eax, dword [rbp-4]
  mov edx, 79
  cmp eax, edx
  jne .elif76
  mov rax, "Can not "
  mov qword [rbp-260], rax
  mov rax, "access a"
  mov qword [rbp-252], rax
  mov rax, " needed "
  mov qword [rbp-244], rax
  mov rax, "shared l"
  mov qword [rbp-236], rax
  mov eax, "ibra"
  mov dword [rbp-228], eax
  mov ax, "ry"
  mov word [rbp-224], ax
  mov al, "."
  mov byte [rbp-222], al
  jmp .escape4
.elif76:
  mov eax, dword [rbp-4]
  mov edx, 80
  cmp eax, edx
  jne .elif77
  mov rax, "Accessin"
  mov qword [rbp-260], rax
  mov rax, "g a corr"
  mov qword [rbp-252], rax
  mov rax, "upted sh"
  mov qword [rbp-244], rax
  mov rax, "ared lib"
  mov qword [rbp-236], rax
  mov eax, "rary"
  mov dword [rbp-228], eax
  mov al, "."
  mov byte [rbp-224], al
  jmp .escape4
.elif77:
  mov eax, dword [rbp-4]
  mov edx, 81
  cmp eax, edx
  jne .elif78
  mov rax, "lib sect"
  mov qword [rbp-260], rax
  mov rax, "ion in a"
  mov qword [rbp-252], rax
  mov rax, " out cor"
  mov qword [rbp-244], rax
  mov eax, "rupt"
  mov dword [rbp-236], eax
  mov ax, "ed"
  mov word [rbp-232], ax
  mov al, "."
  mov byte [rbp-230], al
  jmp .escape4
.elif78:
  mov eax, dword [rbp-4]
  mov edx, 82
  cmp eax, edx
  jne .elif79
  mov rax, "Attempti"
  mov qword [rbp-260], rax
  mov rax, "ng to li"
  mov qword [rbp-252], rax
  mov rax, "nk in to"
  mov qword [rbp-244], rax
  mov rax, "o many s"
  mov qword [rbp-236], rax
  mov rax, "hared li"
  mov qword [rbp-228], rax
  mov rax, "braries."
  mov qword [rbp-220], rax
  jmp .escape4
.elif79:
  mov eax, dword [rbp-4]
  mov edx, 83
  cmp eax, edx
  jne .elif80
  mov rax, "Cannot e"
  mov qword [rbp-260], rax
  mov rax, "xec a sh"
  mov qword [rbp-252], rax
  mov rax, "ared lib"
  mov qword [rbp-244], rax
  mov rax, "rary dir"
  mov qword [rbp-236], rax
  mov eax, "ectl"
  mov dword [rbp-228], eax
  mov ax, "y."
  mov word [rbp-224], ax
  jmp .escape4
.elif80:
  mov eax, dword [rbp-4]
  mov edx, 84
  cmp eax, edx
  jne .elif81
  mov rax, "Illegal "
  mov qword [rbp-260], rax
  mov rax, "byte seq"
  mov qword [rbp-252], rax
  mov eax, "uenc"
  mov dword [rbp-244], eax
  mov ax, "e."
  mov word [rbp-240], ax
  jmp .escape4
.elif81:
  mov eax, dword [rbp-4]
  mov edx, 85
  cmp eax, edx
  jne .elif82
  mov rax, "Interrup"
  mov qword [rbp-260], rax
  mov rax, "ted syst"
  mov qword [rbp-252], rax
  mov rax, "em call "
  mov qword [rbp-244], rax
  mov rax, "should b"
  mov qword [rbp-236], rax
  mov rax, "e restar"
  mov qword [rbp-228], rax
  mov eax, "ted."
  mov dword [rbp-220], eax
  jmp .escape4
.elif82:
  mov eax, dword [rbp-4]
  mov edx, 86
  cmp eax, edx
  jne .elif83
  mov rax, "Streams "
  mov qword [rbp-260], rax
  mov rax, "pipe err"
  mov qword [rbp-252], rax
  mov ax, "or"
  mov word [rbp-244], ax
  mov al, "."
  mov byte [rbp-242], al
  jmp .escape4
.elif83:
  mov eax, dword [rbp-4]
  mov edx, 87
  cmp eax, edx
  jne .elif84
  mov rax, "Too many"
  mov qword [rbp-260], rax
  mov eax, " use"
  mov dword [rbp-252], eax
  mov ax, "rs"
  mov word [rbp-248], ax
  mov al, "."
  mov byte [rbp-246], al
  jmp .escape4
.elif84:
  mov eax, dword [rbp-4]
  mov edx, 88
  cmp eax, edx
  jne .elif85
  mov rax, "Socket o"
  mov qword [rbp-260], rax
  mov rax, "peration"
  mov qword [rbp-252], rax
  mov rax, " on non-"
  mov qword [rbp-244], rax
  mov eax, "sock"
  mov dword [rbp-236], eax
  mov ax, "et"
  mov word [rbp-232], ax
  mov al, "."
  mov byte [rbp-230], al
  jmp .escape4
.elif85:
  mov eax, dword [rbp-4]
  mov edx, 89
  cmp eax, edx
  jne .elif86
  mov rax, "Destinat"
  mov qword [rbp-260], rax
  mov rax, "ion addr"
  mov qword [rbp-252], rax
  mov rax, "ess requ"
  mov qword [rbp-244], rax
  mov eax, "ired"
  mov dword [rbp-236], eax
  mov al, "."
  mov byte [rbp-232], al
  jmp .escape4
.elif86:
  mov eax, dword [rbp-4]
  mov edx, 90
  cmp eax, edx
  jne .elif87
  mov rax, "Message "
  mov qword [rbp-260], rax
  mov rax, "too long"
  mov qword [rbp-252], rax
  mov al, "."
  mov byte [rbp-244], al
  jmp .escape4
.elif87:
  mov eax, dword [rbp-4]
  mov edx, 91
  cmp eax, edx
  jne .elif88
  mov rax, "Protocol"
  mov qword [rbp-260], rax
  mov rax, " wrong t"
  mov qword [rbp-252], rax
  mov rax, "ype for "
  mov qword [rbp-244], rax
  mov eax, "sock"
  mov dword [rbp-236], eax
  mov ax, "et"
  mov word [rbp-232], ax
  mov al, "."
  mov byte [rbp-230], al
  jmp .escape4
.elif88:
  mov eax, dword [rbp-4]
  mov edx, 92
  cmp eax, edx
  jne .elif89
  mov rax, "Protocol"
  mov qword [rbp-260], rax
  mov rax, " not ava"
  mov qword [rbp-252], rax
  mov eax, "ilab"
  mov dword [rbp-244], eax
  mov ax, "le"
  mov word [rbp-240], ax
  mov al, "."
  mov byte [rbp-238], al
  jmp .escape4
.elif89:
  mov eax, dword [rbp-4]
  mov edx, 93
  cmp eax, edx
  jne .elif90
  mov rax, "Protocol"
  mov qword [rbp-260], rax
  mov rax, " not sup"
  mov qword [rbp-252], rax
  mov eax, "port"
  mov dword [rbp-244], eax
  mov ax, "ed"
  mov word [rbp-240], ax
  mov al, "."
  mov byte [rbp-238], al
  jmp .escape4
.elif90:
  mov eax, dword [rbp-4]
  mov edx, 94
  cmp eax, edx
  jne .elif91
  mov rax, "Socket t"
  mov qword [rbp-260], rax
  mov rax, "ype not "
  mov qword [rbp-252], rax
  mov rax, "supporte"
  mov qword [rbp-244], rax
  mov ax, "d."
  mov word [rbp-236], ax
  jmp .escape4
.elif91:
  mov eax, dword [rbp-4]
  mov edx, 95
  cmp eax, edx
  jne .elif92
  mov rax, "Operatio"
  mov qword [rbp-260], rax
  mov rax, "n not su"
  mov qword [rbp-252], rax
  mov rax, "pported "
  mov qword [rbp-244], rax
  mov rax, "on trans"
  mov qword [rbp-236], rax
  mov rax, "port end"
  mov qword [rbp-228], rax
  mov eax, "poin"
  mov dword [rbp-220], eax
  mov ax, "t."
  mov word [rbp-216], ax
  jmp .escape4
.elif92:
  mov eax, dword [rbp-4]
  mov edx, 96
  cmp eax, edx
  jne .elif93
  mov rax, "Protocol"
  mov qword [rbp-260], rax
  mov rax, " family "
  mov qword [rbp-252], rax
  mov rax, "not supp"
  mov qword [rbp-244], rax
  mov eax, "orte"
  mov dword [rbp-236], eax
  mov ax, "d."
  mov word [rbp-232], ax
  jmp .escape4
.elif93:
  mov eax, dword [rbp-4]
  mov edx, 97
  cmp eax, edx
  jne .elif94
  mov rax, "Address "
  mov qword [rbp-260], rax
  mov rax, "family n"
  mov qword [rbp-252], rax
  mov rax, "ot suppo"
  mov qword [rbp-244], rax
  mov rax, "rted by "
  mov qword [rbp-236], rax
  mov rax, "protocol"
  mov qword [rbp-228], rax
  mov al, "."
  mov byte [rbp-220], al
  jmp .escape4
.elif94:
  mov eax, dword [rbp-4]
  mov edx, 98
  cmp eax, edx
  jne .elif95
  mov rax, "Address "
  mov qword [rbp-260], rax
  mov rax, "already "
  mov qword [rbp-252], rax
  mov eax, "in u"
  mov dword [rbp-244], eax
  mov ax, "se"
  mov word [rbp-240], ax
  mov al, "."
  mov byte [rbp-238], al
  jmp .escape4
.elif95:
  mov eax, dword [rbp-4]
  mov edx, 99
  cmp eax, edx
  jne .elif96
  mov rax, "Cannot a"
  mov qword [rbp-260], rax
  mov rax, "ssign re"
  mov qword [rbp-252], rax
  mov rax, "quested "
  mov qword [rbp-244], rax
  mov rax, "address."
  mov qword [rbp-236], rax
  jmp .escape4
.elif96:
  mov eax, dword [rbp-4]
  mov edx, 100
  cmp eax, edx
  jne .elif97
  mov rax, "Network "
  mov qword [rbp-260], rax
  mov rax, "is down."
  mov qword [rbp-252], rax
  jmp .escape4
.elif97:
  mov eax, dword [rbp-4]
  mov edx, 101
  cmp eax, edx
  jne .elif98
  mov rax, "Network "
  mov qword [rbp-260], rax
  mov rax, "is unrea"
  mov qword [rbp-252], rax
  mov eax, "chab"
  mov dword [rbp-244], eax
  mov ax, "le"
  mov word [rbp-240], ax
  mov al, "."
  mov byte [rbp-238], al
  jmp .escape4
.elif98:
  mov eax, dword [rbp-4]
  mov edx, 102
  cmp eax, edx
  jne .elif99
  mov rax, "Network "
  mov qword [rbp-260], rax
  mov rax, "dropped "
  mov qword [rbp-252], rax
  mov rax, "connecti"
  mov qword [rbp-244], rax
  mov rax, "on becau"
  mov qword [rbp-236], rax
  mov rax, "se of re"
  mov qword [rbp-228], rax
  mov eax, "set."
  mov dword [rbp-220], eax
  jmp .escape4
.elif99:
  mov eax, dword [rbp-4]
  mov edx, 103
  cmp eax, edx
  jne .elif100
  mov rax, "Software"
  mov qword [rbp-260], rax
  mov rax, " caused "
  mov qword [rbp-252], rax
  mov rax, "connecti"
  mov qword [rbp-244], rax
  mov rax, "on abort"
  mov qword [rbp-236], rax
  mov al, "."
  mov byte [rbp-228], al
  jmp .escape4
.elif100:
  mov eax, dword [rbp-4]
  mov edx, 104
  cmp eax, edx
  jne .elif101
  mov rax, "Connecti"
  mov qword [rbp-260], rax
  mov rax, "on reset"
  mov qword [rbp-252], rax
  mov rax, " by peer"
  mov qword [rbp-244], rax
  mov al, "."
  mov byte [rbp-236], al
  jmp .escape4
.elif101:
  mov eax, dword [rbp-4]
  mov edx, 105
  cmp eax, edx
  jne .elif102
  mov rax, "No buffe"
  mov qword [rbp-260], rax
  mov rax, "r space "
  mov qword [rbp-252], rax
  mov rax, "availabl"
  mov qword [rbp-244], rax
  mov ax, "e."
  mov word [rbp-236], ax
  jmp .escape4
.elif102:
  mov eax, dword [rbp-4]
  mov edx, 106
  cmp eax, edx
  jne .elif103
  mov rax, "Transpor"
  mov qword [rbp-260], rax
  mov rax, "t endpoi"
  mov qword [rbp-252], rax
  mov rax, "nt is al"
  mov qword [rbp-244], rax
  mov rax, "ready co"
  mov qword [rbp-236], rax
  mov rax, "nnected."
  mov qword [rbp-228], rax
  jmp .escape4
.elif103:
  mov eax, dword [rbp-4]
  mov edx, 107
  cmp eax, edx
  jne .elif104
  mov rax, "Transpor"
  mov qword [rbp-260], rax
  mov rax, "t endpoi"
  mov qword [rbp-252], rax
  mov rax, "nt is no"
  mov qword [rbp-244], rax
  mov rax, "t connec"
  mov qword [rbp-236], rax
  mov eax, "ted."
  mov dword [rbp-228], eax
  jmp .escape4
.elif104:
  mov eax, dword [rbp-4]
  mov edx, 108
  cmp eax, edx
  jne .elif105
  mov rax, "Cannot s"
  mov qword [rbp-260], rax
  mov rax, "end afte"
  mov qword [rbp-252], rax
  mov rax, "r transp"
  mov qword [rbp-244], rax
  mov rax, "ort endp"
  mov qword [rbp-236], rax
  mov rax, "oint shu"
  mov qword [rbp-228], rax
  mov eax, "tdow"
  mov dword [rbp-220], eax
  mov ax, "n."
  mov word [rbp-216], ax
  jmp .escape4
.elif105:
  mov eax, dword [rbp-4]
  mov edx, 109
  cmp eax, edx
  jne .elif106
  mov rax, "Too many"
  mov qword [rbp-260], rax
  mov rax, " referen"
  mov qword [rbp-252], rax
  mov rax, "ces: can"
  mov qword [rbp-244], rax
  mov rax, "not spli"
  mov qword [rbp-236], rax
  mov ax, "ce"
  mov word [rbp-228], ax
  mov al, "."
  mov byte [rbp-226], al
  jmp .escape4
.elif106:
  mov eax, dword [rbp-4]
  mov edx, 110
  cmp eax, edx
  jne .elif107
  mov rax, "Connecti"
  mov qword [rbp-260], rax
  mov rax, "on timed"
  mov qword [rbp-252], rax
  mov eax, " out"
  mov dword [rbp-244], eax
  mov al, "."
  mov byte [rbp-240], al
  jmp .escape4
.elif107:
  mov eax, dword [rbp-4]
  mov edx, 111
  cmp eax, edx
  jne .elif108
  mov rax, "Connecti"
  mov qword [rbp-260], rax
  mov rax, "on refus"
  mov qword [rbp-252], rax
  mov ax, "ed"
  mov word [rbp-244], ax
  mov al, "."
  mov byte [rbp-242], al
  jmp .escape4
.elif108:
  mov eax, dword [rbp-4]
  mov edx, 112
  cmp eax, edx
  jne .elif109
  mov rax, "Host is "
  mov qword [rbp-260], rax
  mov eax, "down"
  mov dword [rbp-252], eax
  mov al, "."
  mov byte [rbp-248], al
  jmp .escape4
.elif109:
  mov eax, dword [rbp-4]
  mov edx, 113
  cmp eax, edx
  jne .elif110
  mov rax, "No route"
  mov qword [rbp-260], rax
  mov rax, " to host"
  mov qword [rbp-252], rax
  mov al, "."
  mov byte [rbp-244], al
  jmp .escape4
.elif110:
  mov eax, dword [rbp-4]
  mov edx, 114
  cmp eax, edx
  jne .elif111
  mov rax, "Operatio"
  mov qword [rbp-260], rax
  mov rax, "n alread"
  mov qword [rbp-252], rax
  mov rax, "y in pro"
  mov qword [rbp-244], rax
  mov eax, "gres"
  mov dword [rbp-236], eax
  mov ax, "s."
  mov word [rbp-232], ax
  jmp .escape4
.elif111:
  mov eax, dword [rbp-4]
  mov edx, 115
  cmp eax, edx
  jne .elif112
  mov rax, "Operatio"
  mov qword [rbp-260], rax
  mov rax, "n now in"
  mov qword [rbp-252], rax
  mov rax, " progres"
  mov qword [rbp-244], rax
  mov ax, "s."
  mov word [rbp-236], ax
  jmp .escape4
.elif112:
  mov eax, dword [rbp-4]
  mov edx, 116
  cmp eax, edx
  jne .elif113
  mov rax, "Stale NF"
  mov qword [rbp-260], rax
  mov rax, "S file h"
  mov qword [rbp-252], rax
  mov eax, "andl"
  mov dword [rbp-244], eax
  mov ax, "e."
  mov word [rbp-240], ax
  jmp .escape4
.elif113:
  mov eax, dword [rbp-4]
  mov edx, 117
  cmp eax, edx
  jne .elif114
  mov rax, "Structur"
  mov qword [rbp-260], rax
  mov rax, "e needs "
  mov qword [rbp-252], rax
  mov rax, "cleaning"
  mov qword [rbp-244], rax
  mov al, "."
  mov byte [rbp-236], al
  jmp .escape4
.elif114:
  mov eax, dword [rbp-4]
  mov edx, 118
  cmp eax, edx
  jne .elif115
  mov rax, "Not a XE"
  mov qword [rbp-260], rax
  mov rax, "NIX name"
  mov qword [rbp-252], rax
  mov rax, "d type f"
  mov qword [rbp-244], rax
  mov eax, "ile."
  mov dword [rbp-236], eax
  jmp .escape4
.elif115:
  mov eax, dword [rbp-4]
  mov edx, 119
  cmp eax, edx
  jne .elif116
  mov rax, "No XENIX"
  mov qword [rbp-260], rax
  mov rax, " semapho"
  mov qword [rbp-252], rax
  mov rax, "res avai"
  mov qword [rbp-244], rax
  mov eax, "labl"
  mov dword [rbp-236], eax
  mov ax, "e."
  mov word [rbp-232], ax
  jmp .escape4
.elif116:
  mov eax, dword [rbp-4]
  mov edx, 120
  cmp eax, edx
  jne .elif117
  mov rax, "Is a nam"
  mov qword [rbp-260], rax
  mov rax, "ed type "
  mov qword [rbp-252], rax
  mov eax, "file"
  mov dword [rbp-244], eax
  mov al, "."
  mov byte [rbp-240], al
  jmp .escape4
.elif117:
  mov eax, dword [rbp-4]
  mov edx, 121
  cmp eax, edx
  jne .elif118
  mov rax, "Remote I"
  mov qword [rbp-260], rax
  mov rax, "/O error"
  mov qword [rbp-252], rax
  mov al, "."
  mov byte [rbp-244], al
  jmp .escape4
.elif118:
  mov eax, dword [rbp-4]
  mov edx, 122
  cmp eax, edx
  jne .elif119
  mov rax, "Quota ex"
  mov qword [rbp-260], rax
  mov eax, "ceed"
  mov dword [rbp-252], eax
  mov ax, "ed"
  mov word [rbp-248], ax
  mov al, "."
  mov byte [rbp-246], al
  jmp .escape4
.elif119:
  mov eax, dword [rbp-4]
  mov edx, 123
  cmp eax, edx
  jne .elif120
  mov rax, "No mediu"
  mov qword [rbp-260], rax
  mov rax, "m found."
  mov qword [rbp-252], rax
  jmp .escape4
.elif120:
  mov eax, dword [rbp-4]
  mov edx, 124
  cmp eax, edx
  jne .elif121
  mov rax, "Wrong me"
  mov qword [rbp-260], rax
  mov rax, "dium typ"
  mov qword [rbp-252], rax
  mov ax, "e."
  mov word [rbp-244], ax
  jmp .escape4
.elif121:
  mov eax, dword [rbp-4]
  mov edx, 125
  cmp eax, edx
  jne .elif122
  mov rax, "Operatio"
  mov qword [rbp-260], rax
  mov rax, "n Cancel"
  mov qword [rbp-252], rax
  mov ax, "ed"
  mov word [rbp-244], ax
  mov al, "."
  mov byte [rbp-242], al
  jmp .escape4
.elif122:
  mov eax, dword [rbp-4]
  mov edx, 126
  cmp eax, edx
  jne .elif123
  mov rax, "Required"
  mov qword [rbp-260], rax
  mov rax, " key not"
  mov qword [rbp-252], rax
  mov rax, " availab"
  mov qword [rbp-244], rax
  mov ax, "le"
  mov word [rbp-236], ax
  mov al, "."
  mov byte [rbp-234], al
  jmp .escape4
.elif123:
  mov eax, dword [rbp-4]
  mov edx, 127
  cmp eax, edx
  jne .elif124
  mov rax, "Key has "
  mov qword [rbp-260], rax
  mov rax, "expired."
  mov qword [rbp-252], rax
  jmp .escape4
.elif124:
  mov eax, dword [rbp-4]
  mov edx, 128
  cmp eax, edx
  jne .elif125
  mov rax, "Key has "
  mov qword [rbp-260], rax
  mov rax, "been rev"
  mov qword [rbp-252], rax
  mov eax, "oked"
  mov dword [rbp-244], eax
  mov al, "."
  mov byte [rbp-240], al
  jmp .escape4
.elif125:
  mov eax, dword [rbp-4]
  mov edx, 129
  cmp eax, edx
  jne .elif126
  mov rax, "Key was "
  mov qword [rbp-260], rax
  mov rax, "rejected"
  mov qword [rbp-252], rax
  mov rax, " by serv"
  mov qword [rbp-244], rax
  mov eax, "ice."
  mov dword [rbp-236], eax
  jmp .escape4
.elif126:
  mov eax, dword [rbp-4]
  mov edx, 130
  cmp eax, edx
  jne .elif127
  mov rax, "Owner di"
  mov qword [rbp-260], rax
  mov ax, "ed"
  mov word [rbp-252], ax
  mov al, "."
  mov byte [rbp-250], al
  jmp .escape4
.elif127:
  mov eax, dword [rbp-4]
  mov edx, 131
  cmp eax, edx
  jne .elif128
  mov rax, "State no"
  mov qword [rbp-260], rax
  mov rax, "t recove"
  mov qword [rbp-252], rax
  mov eax, "rabl"
  mov dword [rbp-244], eax
  mov ax, "e."
  mov word [rbp-240], ax
.escape4:
.elif128:
  lea rax, [rbp-260]
  mov qword [rbp-268], rax
  mov rax, 2
  mov rdi, qword [rbp-268]
  call prints
.exit:
  add rsp, 296
  pop rbp
  ret
readf:
  push rbp
  mov rbp, rsp
  sub rsp, 72
  mov dword [rbp-4], eax
  mov qword [rbp-12], rdi
  mov dword [rbp-16], esi
  movsx rax, dword [rbp-4]
  mov rdi, qword [rbp-12]
  movsx rsi, dword [rbp-16]
  call SYS_read
  mov dword [rbp-16], eax
  mov eax, dword [rbp-16]
  mov edx, 0
  cmp eax, edx
  jge .if4
  mov byte [rbp-21], 10
  mov rax, "Unable t"
  mov qword [rbp-41], rax
  mov rax, "o read f"
  mov qword [rbp-33], rax
  mov eax, "ile."
  mov dword [rbp-25], eax
  lea rax, [rbp-41]
  mov qword [rbp-49], rax
  mov rax, 2
  mov rdi, qword [rbp-49]
  mov rsi, 21
  call SYS_write
  mov eax, "Erro"
  mov dword [rbp-56], eax
  mov ax, "r:"
  mov word [rbp-52], ax
  mov al, " "
  mov byte [rbp-50], al
  lea rax, [rbp-56]
  mov qword [rbp-49], rax
  mov rax, 2
  mov rdi, qword [rbp-49]
  mov rsi, 7
  call SYS_write
  movsx rax, dword [rbp-16]
  call __print_error
  mov rax, 2
  movsx rdi, byte [rbp-21]
  call printc
  mov rax, 1
  call SYS_exit
.if4:
  mov eax, dword [rbp-16]
  mov dword [rbp-60], eax
.exit:
  mov eax, dword [rbp-60]
  add rsp, 72
  pop rbp
  ret
openf:
  push rbp
  mov rbp, rsp
  sub rsp, 64
  mov qword [rbp-8], rax
  mov rax, qword [rbp-8]
  mov rdi, 2
  mov rsi, 1792
  call SYS_openfd
  mov dword [rbp-52], eax
  mov eax, dword [rbp-52]
  mov edx, 0
  cmp eax, edx
  jge .if5
  mov byte [rbp-13], 10
  mov rax, "Unable t"
  mov qword [rbp-33], rax
  mov rax, "o open f"
  mov qword [rbp-25], rax
  mov eax, "ile."
  mov dword [rbp-17], eax
  lea rax, [rbp-33]
  mov qword [rbp-41], rax
  mov rax, 2
  mov rdi, qword [rbp-41]
  mov rsi, 21
  call SYS_write
  mov eax, "Erro"
  mov dword [rbp-48], eax
  mov ax, "r:"
  mov word [rbp-44], ax
  mov al, " "
  mov byte [rbp-42], al
  lea rax, [rbp-48]
  mov qword [rbp-41], rax
  mov rax, 2
  mov rdi, qword [rbp-41]
  mov rsi, 7
  call SYS_write
  movsx rax, dword [rbp-52]
  call __print_error
  mov rax, 2
  movsx rdi, byte [rbp-13]
  call printc
  mov rax, 1
  call SYS_exit
.if5:
.exit:
  mov eax, dword [rbp-52]
  add rsp, 64
  pop rbp
  ret
socket:
  push rbp
  mov rbp, rsp
  sub rsp, 63
  mov dword [rbp-4], eax
  mov dword [rbp-8], edi
  mov dword [rbp-12], esi
  mov rax, 41
  movsx rdi, dword [rbp-4]
  movsx rsi, dword [rbp-8]
  movsx rdx, dword [rbp-12]
  syscall
  mov dword [rbp-51], eax
  mov eax, dword [rbp-51]
  mov edx, 0
  cmp eax, edx
  jge .if6
  mov byte [rbp-17], 10
  mov rax, "Failed t"
  mov qword [rbp-39], rax
  mov rax, "o open s"
  mov qword [rbp-31], rax
  mov eax, "ocke"
  mov dword [rbp-23], eax
  mov ax, "t."
  mov word [rbp-19], ax
  lea rax, [rbp-39]
  mov qword [rbp-47], rax
  mov rax, 2
  mov rdi, qword [rbp-47]
  mov rsi, 23
  call SYS_write
  mov rax, 1
  call SYS_exit
.if6:
.exit:
  mov eax, dword [rbp-51]
  add rsp, 63
  pop rbp
  ret
setsockopt:
  push rbp
  mov rbp, rsp
  sub rsp, 81
  mov dword [rbp-4], eax
  mov dword [rbp-8], edi
  mov dword [rbp-12], esi
  mov qword [rbp-20], rdx
  mov dword [rbp-24], r10d
  mov rax, 54
  movsx rdi, dword [rbp-4]
  movsx rsi, dword [rbp-8]
  movsx rdx, dword [rbp-12]
  mov r10, qword [rbp-20]
  movsx r8, dword [rbp-24]
  syscall
  mov dword [rbp-28], eax
  mov eax, dword [rbp-28]
  mov edx, 0
  cmp eax, edx
  jge .if7
  mov byte [rbp-29], 10
  mov rax, "Failed t"
  mov qword [rbp-57], rax
  mov rax, "o set so"
  mov qword [rbp-49], rax
  mov rax, "cket opt"
  mov qword [rbp-41], rax
  mov eax, "ion."
  mov dword [rbp-33], eax
  lea rax, [rbp-57]
  mov qword [rbp-65], rax
  mov rax, 2
  mov rdi, qword [rbp-65]
  mov rsi, 29
  call SYS_write
  mov rax, 1
  call SYS_exit
.if7:
.exit:
  add rsp, 81
  pop rbp
  ret
bind:
  push rbp
  mov rbp, rsp
  sub rsp, 67
  mov dword [rbp-4], eax
  mov qword [rbp-12], rdi
  mov dword [rbp-16], esi
  mov rax, 49
  movsx rdi, dword [rbp-4]
  mov rsi, qword [rbp-12]
  movsx rdx, dword [rbp-16]
  syscall
  mov dword [rbp-20], eax
  mov eax, dword [rbp-20]
  mov edx, 0
  cmp eax, edx
  jge .if8
  mov byte [rbp-21], 10
  mov rax, "Failed t"
  mov qword [rbp-43], rax
  mov rax, "o bind s"
  mov qword [rbp-35], rax
  mov eax, "ocke"
  mov dword [rbp-27], eax
  mov ax, "t."
  mov word [rbp-23], ax
  lea rax, [rbp-43]
  mov qword [rbp-51], rax
  mov rax, 2
  mov rdi, qword [rbp-51]
  mov rsi, 23
  call SYS_write
  movsx rax, dword [rbp-20]
  call __print_error
  mov rax, 2
  mov rdi, 10
  call printc
  movsx rax, dword [rbp-4]
  call SYS_closefd
  mov rax, 1
  call SYS_exit
.if8:
.exit:
  add rsp, 67
  pop rbp
  ret
listen:
  push rbp
  mov rbp, rsp
  sub rsp, 64
  mov dword [rbp-4], eax
  mov dword [rbp-8], edi
  mov rax, 50
  movsx rdi, dword [rbp-4]
  movsx rsi, dword [rbp-8]
  syscall
  mov dword [rbp-12], eax
  mov eax, dword [rbp-12]
  mov edx, 0
  cmp eax, edx
  jge .if9
  mov byte [rbp-13], 10
  mov rax, "Failed t"
  mov qword [rbp-40], rax
  mov rax, "o listen"
  mov qword [rbp-32], rax
  mov rax, " to sock"
  mov qword [rbp-24], rax
  mov ax, "et"
  mov word [rbp-16], ax
  mov al, "."
  mov byte [rbp-14], al
  lea rax, [rbp-40]
  mov qword [rbp-48], rax
  mov rax, 2
  mov rdi, qword [rbp-48]
  mov rsi, 28
  call SYS_write
  mov rax, 1
  call SYS_exit
.if9:
.exit:
  add rsp, 64
  pop rbp
  ret
accept:
  push rbp
  mov rbp, rsp
  sub rsp, 63
  mov dword [rbp-4], eax
  mov qword [rbp-12], rdi
  mov dword [rbp-16], esi
  mov rax, 43
  movsx rdi, dword [rbp-4]
  mov rsi, qword [rbp-12]
  movsx rdx, dword [rbp-16]
  syscall
  mov dword [rbp-51], eax
  mov eax, dword [rbp-51]
  mov edx, 0
  cmp eax, edx
  jge .if10
  mov byte [rbp-21], 0
  mov byte [rbp-22], 10
  mov rax, "Failed t"
  mov qword [rbp-39], rax
  mov rax, "o accept"
  mov qword [rbp-31], rax
  mov al, "."
  mov byte [rbp-23], al
  lea rax, [rbp-39]
  mov qword [rbp-47], rax
  mov rax, 2
  mov rdi, qword [rbp-47]
  call prints
  movsx rax, dword [rbp-51]
  call __print_error
  mov rax, 2
  mov rdi, 10
  call printc
  mov rax, 1
  call SYS_exit
.if10:
.exit:
  mov eax, dword [rbp-51]
  add rsp, 63
  pop rbp
  ret
recvERRCHECK:
  push rbp
  mov rbp, rsp
  sub rsp, 72
  mov dword [rbp-4], eax
  mov qword [rbp-12], rdi
  mov dword [rbp-16], esi
  mov qword [rbp-28], 0
  mov rax, 45
  movsx rdi, dword [rbp-4]
  mov rsi, qword [rbp-12]
  movsx rdx, dword [rbp-16]
  mov r10, qword [rbp-28]
  mov r8, 0
  syscall
  mov dword [rbp-60], eax
  mov eax, dword [rbp-60]
  mov edx, 0
  cmp eax, edx
  jge .if11
  mov byte [rbp-29], 0
  mov byte [rbp-30], 10
  mov rax, "Failed t"
  mov qword [rbp-48], rax
  mov rax, "o reciev"
  mov qword [rbp-40], rax
  mov ax, "e."
  mov word [rbp-32], ax
  lea rax, [rbp-48]
  mov qword [rbp-56], rax
  mov rax, 2
  mov rdi, qword [rbp-56]
  call prints
  movsx rax, dword [rbp-60]
  call __print_error
  mov rax, 2
  mov rdi, 10
  call printc
  mov rax, 1
  call SYS_exit
.if11:
.exit:
  mov eax, dword [rbp-60]
  add rsp, 72
  pop rbp
  ret
bitswap16:
  push rbp
  mov rbp, rsp
  sub rsp, 20
  mov word [rbp-2], ax
  movzx eax, word [rbp-2]
  shr ax, 8
  mov edx, eax
  movzx eax, word [rbp-2]
  sal eax, 8
  or eax, edx
  mov word [rbp-6], ax
.exit:
  mov ax, word [rbp-6]
  add rsp, 20
  pop rbp
  ret
mmap:
  push rbp
  mov rbp, rsp
  sub rsp, 81
  mov qword [rbp-8], rax
  mov dword [rbp-12], edi
  mov dword [rbp-16], esi
  mov dword [rbp-20], edx
  mov dword [rbp-24], r10d
  mov dword [rbp-28], r8d
  mov rax, 9
  mov rdi, qword [rbp-8]
  movsx rsi, dword [rbp-12]
  movsx rdx, dword [rbp-16]
  movsx r10, dword [rbp-20]
  movsx r8, dword [rbp-24]
  movsx r9, dword [rbp-28]
  syscall
  mov qword [rbp-73], rax
  mov rax, qword [rbp-73]
  mov rdx, 0
  cmp rax, rdx
  jge .if12
  mov byte [rbp-37], 0
  mov rax, "Couldn't"
  mov qword [rbp-57], rax
  mov rax, " map mem"
  mov qword [rbp-49], rax
  mov eax, "ory."
  mov dword [rbp-41], eax
  lea rax, [rbp-57]
  mov qword [rbp-65], rax
  mov rax, 2
  mov rdi, qword [rbp-65]
  call prints
  mov rax, 2
  mov rdi, 10
  call printc
  mov rax, qword [rbp-73]
  call __print_error
  mov rax, 2
  mov rdi, 10
  call printc
  mov rax, 1
  call SYS_exit
.if12:
.exit:
  mov rax, qword [rbp-73]
  add rsp, 81
  pop rbp
  ret
strb_contains:
  push rbp
  mov rbp, rsp
  sub rsp, 61
  mov qword [rbp-8], rax
  mov qword [rbp-16], rdi
  mov rax, qword [rbp-8]
  mov edx, dword [rax+8]
  mov dword [rbp-21], edx
  mov rax, qword [rbp-16]
  mov edx, dword [rax+8]
  mov dword [rbp-25], edx
  mov rax, qword [rbp-16]
  mov rdx, qword [rax]
  mov qword [rbp-33], rdx
  mov eax, dword [rbp-21]
  mov edx, dword [rbp-25]
  sub eax, edx
  mov dword [rbp-37], eax
  mov rax, qword [rbp-8]
  mov rdx, qword [rax]
  mov qword [rbp-45], rdx
  jmp .whilecmp6
.while6:
  mov rax, qword [rbp-45]
  movsx rdi, dword [rbp-25]
  mov rsi, qword [rbp-33]
  movsx rdx, dword [rbp-25]
  call strcmp_speclens
  mov byte [rbp-46], al
  cmp byte [rbp-46], 0
  je .if13
  mov dword [rbp-37], 1
.if13:
  inc qword [rbp-45]
  dec dword [rbp-37]
.whilecmp6:
  cmp dword [rbp-37], 0
  jne .while6
.exit:
  mov al, byte [rbp-46]
  add rsp, 61
  pop rbp
  ret
str_contains:
  push rbp
  mov rbp, rsp
  sub rsp, 81
  mov qword [rbp-8], rax
  mov dword [rbp-12], edi
  mov qword [rbp-20], rsi
  mov dword [rbp-24], edx
  mov rax, qword [rbp-8]
  mov qword [rbp-37], rax
  mov eax, dword [rbp-12]
  mov dword [rbp-29], eax
  mov rax, qword [rbp-20]
  mov qword [rbp-49], rax
  mov eax, dword [rbp-24]
  mov dword [rbp-41], eax
  lea rax, [rbp-37]
  mov qword [rbp-57], rax
  lea rax, [rbp-49]
  mov qword [rbp-65], rax
  mov rax, qword [rbp-57]
  mov rdi, qword [rbp-65]
  call strb_contains
  mov byte [rbp-66], al
.exit:
  mov al, byte [rbp-66]
  add rsp, 81
  pop rbp
  ret
strb_find:
  push rbp
  mov rbp, rsp
  sub rsp, 69
  mov qword [rbp-8], rax
  mov qword [rbp-16], rdi
  mov dword [rbp-57], 4294967294
  mov rax, qword [rbp-8]
  mov edx, dword [rax+8]
  mov dword [rbp-24], edx
  mov rax, qword [rbp-16]
  mov edx, dword [rax+8]
  mov dword [rbp-28], edx
  mov rax, qword [rbp-16]
  mov rdx, qword [rax]
  mov qword [rbp-36], rdx
  mov dword [rbp-40], 0
  mov eax, dword [rbp-24]
  mov edx, dword [rbp-28]
  sub eax, edx
  mov dword [rbp-44], eax
  mov rax, qword [rbp-8]
  mov rdx, qword [rax]
  mov qword [rbp-52], rdx
  jmp .whilecmp7
.while7:
  mov rax, qword [rbp-52]
  movsx rdi, dword [rbp-28]
  mov rsi, qword [rbp-36]
  movsx rdx, dword [rbp-28]
  call strcmp_speclens
  mov byte [rbp-53], al
  cmp byte [rbp-53], 0
  je .if14
  mov eax, dword [rbp-40]
  mov dword [rbp-57], eax
  mov dword [rbp-44], 0
.if14:
  inc qword [rbp-52]
  inc dword [rbp-40]
.whilecmp7:
  mov eax, dword [rbp-40]
  mov edx, dword [rbp-44]
  cmp eax, edx
  jl .while7
.exit:
  mov eax, dword [rbp-57]
  add rsp, 69
  pop rbp
  ret
str_find:
  push rbp
  mov rbp, rsp
  sub rsp, 84
  mov qword [rbp-8], rax
  mov dword [rbp-12], edi
  mov qword [rbp-20], rsi
  mov dword [rbp-24], edx
  mov rax, qword [rbp-8]
  mov qword [rbp-40], rax
  mov eax, dword [rbp-12]
  mov dword [rbp-32], eax
  mov rax, qword [rbp-20]
  mov qword [rbp-52], rax
  mov eax, dword [rbp-24]
  mov dword [rbp-44], eax
  lea rax, [rbp-40]
  mov qword [rbp-60], rax
  lea rax, [rbp-52]
  mov qword [rbp-68], rax
  mov rax, qword [rbp-60]
  mov rdi, qword [rbp-68]
  call strb_find
  mov dword [rbp-72], eax
.exit:
  mov eax, dword [rbp-72]
  add rsp, 84
  pop rbp
  ret
strlen_specnull:
  push rbp
  mov rbp, rsp
  sub rsp, 22
  mov qword [rbp-8], rax
  mov byte [rbp-9], dil
  mov rax, qword [rbp-8]
  mov qword [rbp-21], rax
  mov rax, qword [rbp-8]
  mov dl, byte [rax]
  mov byte [rbp-22], dl
  jmp .whilecmp8
.while8:
  inc qword [rbp-8]
  mov rax, qword [rbp-8]
  mov dl, byte [rax]
  mov byte [rbp-22], dl
.whilecmp8:
  mov al, byte [rbp-22]
  mov dl, byte [rbp-9]
  cmp al, dl
  jne .while8
  mov rax, qword [rbp-8]
  mov rdx, qword [rbp-21]
  sub rax, rdx
  mov dword [rbp-26], eax
.exit:
  mov eax, dword [rbp-26]
  add rsp, 22
  pop rbp
  ret
__base64_ARR8_INDEX:
  push rbp
  mov rbp, rsp
  sub rsp, 13
  mov qword [rbp-8], rax
  mov dword [rbp-12], edi
  mov rax, qword [rbp-8]
  movsx rdx, dword [rbp-12]
  add rax, rdx
  mov qword [rbp-8], rax
  mov rax, qword [rbp-8]
  mov dl, byte [rax]
  mov byte [rbp-14], dl
.exit:
  mov al, byte [rbp-14]
  add rsp, 13
  pop rbp
  ret
base64:
  push rbp
  mov rbp, rsp
  sub rsp, 85
  mov qword [rbp-8], rax
  mov qword [rbp-16], rdi
  mov dword [rbp-20], esi
  mov dword [rbp-24], edx
  mov eax, dword [rbp-20]
  mov edx, 2
  add eax, edx
  mov dword [rbp-28], eax
  mov eax, dword [rbp-28]
  mov edx, 4
  mul edx
  mov dword [rbp-28], eax
  xor rdx, rdx
  mov eax, dword [rbp-28]
  mov ebx, 3
  div ebx
  mov dword [rbp-28], eax
  mov eax, dword [rbp-28]
  mov edx, dword [rbp-24]
  cmp eax, edx
  jle .if15
  mov rax, 2
  mov rdi, base64@?errmsg
  call prints
  mov rax, 2
  movsx rdi, dword [rbp-28]
  call printi
  mov rax, 2
  mov rdi, 10
  call printc
  mov rax, 4294967294
  call SYS_exit
.if15:
  mov rax, qword [rbp-8]
  movsx rdx, dword [rbp-20]
  add rax, rdx
  mov qword [rbp-36], rax
  mov rax, qword [rbp-8]
  mov qword [rbp-44], rax
  mov rax, qword [rbp-16]
  mov qword [rbp-52], rax
  mov rax, qword [rbp-36]
  mov rdx, qword [rbp-44]
  sub rax, rdx
  mov qword [rbp-60], rax
  mov rax, qword [rbp-60]
  mov rdx, 3
  cmp rax, rdx
  setge al
  mov byte [rbp-61], al
  jmp .whilecmp9
.while9:
  mov byte [rbp-62], 0
  mov byte [rbp-63], 0
  mov byte [rbp-64], 0
  mov byte [rbp-65], 0
  mov dword [rbp-69], 0
  mov rax, qword [rbp-44]
  mov dl, byte [rax]
  mov byte [rbp-62], dl
  mov al, byte [rbp-62]
  shr al, 2
  mov byte [rbp-62], al
  xor rax, rax
  mov al, byte [rbp-62]
  mov dword [rbp-69], eax
  mov rax, base64@?b64table
  movsx rdi, dword [rbp-69]
  call __base64_ARR8_INDEX
  mov byte [rbp-63], al
  mov rax, qword [rbp-52]
  mov dl, byte [rbp-63]
  mov byte [rax], dl
  inc qword [rbp-52]
  mov rax, qword [rbp-44]
  mov dl, byte [rax]
  mov byte [rbp-62], dl
  movsx eax, byte [rbp-62]
  mov edx, 0x03
  and eax, edx
  mov byte [rbp-62], al
  mov al, byte [rbp-62]
  shl al, 4
  mov byte [rbp-62], al
  mov rax, qword [rbp-44]
  mov rdi, 1
  call __base64_ARR8_INDEX
  mov byte [rbp-63], al
  mov al, byte [rbp-63]
  shr al, 4
  mov byte [rbp-63], al
  mov al, byte [rbp-62]
  mov dl, byte [rbp-63]
  or al, dl
  movsx eax, al
  mov dword [rbp-69], eax
  mov rax, base64@?b64table
  movsx rdi, dword [rbp-69]
  call __base64_ARR8_INDEX
  mov byte [rbp-63], al
  mov rax, qword [rbp-52]
  mov dl, byte [rbp-63]
  mov byte [rax], dl
  inc qword [rbp-52]
  mov rax, qword [rbp-44]
  mov rdi, 1
  call __base64_ARR8_INDEX
  mov byte [rbp-62], al
  movsx eax, byte [rbp-62]
  mov edx, 0x0f
  and eax, edx
  mov byte [rbp-62], al
  mov al, byte [rbp-62]
  shl al, 2
  mov byte [rbp-62], al
  mov rax, qword [rbp-44]
  mov rdi, 2
  call __base64_ARR8_INDEX
  mov byte [rbp-63], al
  mov al, byte [rbp-63]
  shr al, 6
  mov byte [rbp-63], al
  mov al, byte [rbp-62]
  mov dl, byte [rbp-63]
  or al, dl
  movsx eax, al
  mov dword [rbp-69], eax
  mov rax, base64@?b64table
  movsx rdi, dword [rbp-69]
  call __base64_ARR8_INDEX
  mov byte [rbp-63], al
  mov rax, qword [rbp-52]
  mov dl, byte [rbp-63]
  mov byte [rax], dl
  inc qword [rbp-52]
  mov rax, qword [rbp-44]
  mov rdi, 2
  call __base64_ARR8_INDEX
  mov byte [rbp-62], al
  movsx eax, byte [rbp-62]
  mov edx, 0x3f
  and eax, edx
  mov byte [rbp-62], al
  xor rax, rax
  mov al, byte [rbp-62]
  mov dword [rbp-69], eax
  mov rax, base64@?b64table
  movsx rdi, dword [rbp-69]
  call __base64_ARR8_INDEX
  mov byte [rbp-63], al
  mov rax, qword [rbp-52]
  mov dl, byte [rbp-63]
  mov byte [rax], dl
  inc qword [rbp-52]
  mov rax, qword [rbp-44]
  mov rdx, 3
  add rax, rdx
  mov qword [rbp-44], rax
  mov rax, qword [rbp-36]
  mov rdx, qword [rbp-44]
  sub rax, rdx
  mov qword [rbp-60], rax
  mov rax, qword [rbp-60]
  mov rdx, 3
  cmp rax, rdx
  setge al
  mov byte [rbp-61], al
.whilecmp9:
  movsx eax, byte [rbp-61]
  mov edx, 0
  cmp eax, edx
  jg .while9
  mov rax, qword [rbp-36]
  mov rdx, qword [rbp-44]
  sub rax, rdx
  mov byte [rbp-61], al
  cmp byte [rbp-61], 0
  je .if16
  mov rax, qword [rbp-44]
  mov dl, byte [rax]
  mov byte [rbp-62], dl
  mov al, byte [rbp-62]
  shr al, 2
  mov byte [rbp-62], al
  xor rax, rax
  mov al, byte [rbp-62]
  mov dword [rbp-69], eax
  mov rax, base64@?b64table
  movsx rdi, dword [rbp-69]
  call __base64_ARR8_INDEX
  mov byte [rbp-63], al
  mov rax, qword [rbp-52]
  mov dl, byte [rbp-63]
  mov byte [rax], dl
  inc qword [rbp-52]
  movsx eax, byte [rbp-61]
  mov edx, 1
  cmp eax, edx
  jne .if17
  mov rax, qword [rbp-44]
  mov dl, byte [rax]
  mov byte [rbp-62], dl
  movsx eax, byte [rbp-62]
  mov edx, 0x03
  and eax, edx
  mov byte [rbp-62], al
  mov al, byte [rbp-62]
  shl al, 4
  mov byte [rbp-62], al
  xor rax, rax
  mov al, byte [rbp-62]
  mov dword [rbp-69], eax
  mov rax, base64@?b64table
  movsx rdi, dword [rbp-69]
  call __base64_ARR8_INDEX
  mov byte [rbp-63], al
  mov rax, qword [rbp-52]
  mov dl, byte [rbp-63]
  mov byte [rax], dl
  inc qword [rbp-52]
  mov rax, qword [rbp-52]
  mov dl, "="
  mov byte [rax], dl
  inc qword [rbp-52]
  jmp .escape25
.if17:
  mov rax, qword [rbp-44]
  mov dl, byte [rax]
  mov byte [rbp-62], dl
  movsx eax, byte [rbp-62]
  mov edx, 0x03
  and eax, edx
  mov byte [rbp-62], al
  mov al, byte [rbp-62]
  shl al, 4
  mov byte [rbp-62], al
  mov rax, qword [rbp-44]
  mov rdi, 1
  call __base64_ARR8_INDEX
  mov byte [rbp-63], al
  mov al, byte [rbp-63]
  shr al, 4
  mov byte [rbp-63], al
  mov al, byte [rbp-62]
  mov dl, byte [rbp-63]
  or al, dl
  movsx eax, al
  mov dword [rbp-69], eax
  mov rax, base64@?b64table
  movsx rdi, dword [rbp-69]
  call __base64_ARR8_INDEX
  mov byte [rbp-63], al
  mov rax, qword [rbp-52]
  mov dl, byte [rbp-63]
  mov byte [rax], dl
  inc qword [rbp-52]
  mov rax, qword [rbp-44]
  mov rdi, 1
  call __base64_ARR8_INDEX
  mov byte [rbp-62], al
  movsx eax, byte [rbp-62]
  mov edx, 0x0f
  and eax, edx
  mov byte [rbp-62], al
  mov al, byte [rbp-62]
  shl al, 2
  mov byte [rbp-62], al
  xor rax, rax
  mov al, byte [rbp-62]
  mov dword [rbp-69], eax
  mov rax, base64@?b64table
  movsx rdi, dword [rbp-69]
  call __base64_ARR8_INDEX
  mov byte [rbp-63], al
  mov rax, qword [rbp-52]
  mov dl, byte [rbp-63]
  mov byte [rax], dl
  inc qword [rbp-52]
.escape25:
  mov rax, qword [rbp-52]
  mov dl, "="
  mov byte [rax], dl
.if16:
.exit:
  add rsp, 85
  pop rbp
  ret
__hexToBin4:
  push rbp
  mov rbp, rsp
  sub rsp, 2
  mov byte [rbp-1], al
  movsx eax, byte [rbp-1]
  mov edx, 57
  cmp eax, edx
  jg .if18
  movsx eax, byte [rbp-1]
  mov edx, 48
  sub eax, edx
  mov byte [rbp-3], al
  jmp .escape29
.if18:
  movsx eax, byte [rbp-1]
  mov edx, 70
  cmp eax, edx
  jg .elif129
  movsx eax, byte [rbp-1]
  mov edx, 55
  sub eax, edx
  mov byte [rbp-3], al
  jmp .escape29
.elif129:
  movsx eax, byte [rbp-1]
  mov edx, 87
  sub eax, edx
  mov byte [rbp-3], al
.escape29:
.exit:
  mov al, byte [rbp-3]
  add rsp, 2
  pop rbp
  ret
hexToBin:
  push rbp
  mov rbp, rsp
  sub rsp, 48
  mov qword [rbp-8], rax
  mov qword [rbp-16], rdi
  mov dword [rbp-20], esi
  mov dword [rbp-24], edx
  mov eax, dword [rbp-20]
  mov dword [rbp-28], eax
  jmp .whilecmp10
.while10:
  mov byte [rbp-29], 0
  mov byte [rbp-30], 0
  mov rax, qword [rbp-8]
  mov dl, byte [rax]
  mov byte [rbp-31], dl
  mov byte [rbp-32], 0
  movsx rax, byte [rbp-31]
  call __hexToBin4
  mov byte [rbp-30], al
  inc qword [rbp-8]
  mov rax, qword [rbp-8]
  mov dl, byte [rax]
  mov byte [rbp-31], dl
  movsx rax, byte [rbp-31]
  call __hexToBin4
  mov byte [rbp-31], al
  inc qword [rbp-8]
  mov al, byte [rbp-30]
  shl al, 4
  mov byte [rbp-30], al
  mov al, byte [rbp-31]
  mov dl, byte [rbp-30]
  or al, dl
  mov byte [rbp-31], al
  mov rax, qword [rbp-16]
  mov dl, byte [rbp-31]
  mov byte [rax], dl
  inc qword [rbp-16]
  mov eax, dword [rbp-28]
  mov edx, 2
  sub eax, edx
  mov dword [rbp-28], eax
.whilecmp10:
  cmp dword [rbp-28], 0
  jne .while10
.exit:
  add rsp, 48
  pop rbp
  ret
hexToBase64:
  push rbp
  mov rbp, rsp
  sub rsp, 44
  mov qword [rbp-8], rax
  mov qword [rbp-16], rdi
  mov dword [rbp-20], esi
  mov dword [rbp-24], edx
  mov rax, qword [rbp-8]
  mov rdi, hexToBase64@?binaryOut
  movsx rsi, dword [rbp-20]
  mov rdx, 200
  call hexToBin
  xor rdx, rdx
  mov eax, dword [rbp-20]
  mov ebx, 2
  div ebx
  mov dword [rbp-28], eax
  mov rax, 1
  movsx rdi, dword [rbp-24]
  call printi
  mov rax, 1
  mov rdi, 10
  call printc
  mov rax, hexToBase64@?binaryOut
  mov rdi, qword [rbp-16]
  movsx rsi, dword [rbp-28]
  movsx rdx, dword [rbp-24]
  call base64
.exit:
  add rsp, 44
  pop rbp
  ret
getfile:
  push rbp
  mov rbp, rsp
  sub rsp, 72
  mov qword [rbp-8], rax
  mov rax, qword [rbp-8]
  call openf
  mov dword [rbp-20], eax
  movsx rax, dword [rbp-20]
  call sizeof_file
  mov dword [rbp-24], eax
  mov eax, 1
  mov edx, 2
  or eax, edx
  mov dword [rbp-28], eax
  mov eax, 2
  mov edx, 32
  or eax, edx
  mov dword [rbp-32], eax
  mov rax, 0
  movsx rdi, dword [rbp-24]
  movsx rsi, dword [rbp-28]
  movsx rdx, dword [rbp-32]
  mov r10, 0
  mov r8, 0
  call mmap
  mov qword [rbp-40], rax
  movsx rax, dword [rbp-20]
  mov rdi, qword [rbp-40]
  movsx rsi, dword [rbp-24]
  call readf
  mov dword [rbp-44], eax
  movsx rax, dword [rbp-20]
  call SYS_closefd
  mov rax, qword [rbp-40]
  mov qword [rbp-56], rax
  mov eax, dword [rbp-24]
  mov dword [rbp-48], eax
  lea rax, [rbp-56]
  mov qword [rbp-64], rax
.exit:
  mov rax, qword [rbp-64]
  add rsp, 72
  pop rbp
  ret
readHandShake:
  push rbp
  mov rbp, rsp
  sub rsp, 43
  mov dword [rbp-4], eax
  mov qword [rbp-12], rdi
  movsx rax, dword [rbp-4]
  mov rdi, readHandShake@?read_buffer
  mov rsi, 1000
  call recvERRCHECK
  mov dword [rbp-17], eax
  mov rax, qword [rbp-12]
  mov edx, dword [rbp-17]
  mov dword [rax], edx
  mov eax, dword [rbp-17]
  mov edx, 0
  cmp eax, edx
  jle .if19
  mov rax, readHandShake@?HTMLRQ
  call strlen
  mov dword [rbp-21], eax
  dec dword [rbp-21]
  mov rax, readHandShake@?read_buffer
  movsx rdi, dword [rbp-17]
  mov rsi, readHandShake@?HTMLRQ
  movsx rdx, dword [rbp-21]
  call str_contains
  mov byte [rbp-22], al
  cmp byte [rbp-22], 0
  je .if20
  mov byte [rbp-28], 1
  jmp .escape31
.if20:
  mov rax, readHandShake@?webSocketRQ
  call strlen
  mov dword [rbp-26], eax
  dec dword [rbp-26]
  mov rax, readHandShake@?read_buffer
  movsx rdi, dword [rbp-17]
  mov rsi, readHandShake@?webSocketRQ
  movsx rdx, dword [rbp-26]
  call str_contains
  mov byte [rbp-27], al
  cmp byte [rbp-27], 0
  je .if21
  mov byte [rbp-28], 2
  jmp .escape32
.if21:
  mov byte [rbp-28], 0
.escape32:
.escape31:
  jmp .escape30
.if19:
  mov eax, 0
  mov edx, 1
  sub eax, edx
  mov byte [rbp-28], al
.escape30:
.exit:
  mov al, byte [rbp-28]
  add rsp, 43
  pop rbp
  ret
handleWebSocketConnection:
  push rbp
  mov rbp, rsp
  sub rsp, 77
  mov dword [rbp-4], eax
  mov dword [rbp-8], edi
  mov rax, handleWebSocketConnection@?keyField
  call strlen
  mov dword [rbp-12], eax
  dec dword [rbp-12]
  mov rax, readHandShake@?read_buffer
  movsx rdi, dword [rbp-8]
  mov rsi, handleWebSocketConnection@?keyField
  movsx rdx, dword [rbp-12]
  call str_find
  mov dword [rbp-16], eax
  mov rax, 1
  movsx rdi, dword [rbp-16]
  call printi
  mov rax, 1
  mov rdi, 10
  call printc
  mov eax, dword [rbp-16]
  mov edx, 0
  cmp eax, edx
  jge .if22
  mov rax, 2
  mov rdi, handleWebSocketConnection@?errmsgNoKey
  call prints
  mov rax, 4294967294
  call SYS_exit
.if22:
  mov eax, dword [rbp-16]
  mov edx, dword [rbp-12]
  add eax, edx
  mov dword [rbp-16], eax
  mov eax, dword [rbp-16]
  mov edx, 2
  add eax, edx
  mov dword [rbp-16], eax
  lea rax, [readHandShake@?read_buffer]
  mov qword [rbp-24], rax
  lea rax, [handleWebSocketConnection@?keybuffer]
  mov qword [rbp-32], rax
  mov rax, qword [rbp-24]
  movsx rdx, dword [rbp-16]
  add rax, rdx
  mov qword [rbp-40], rax
  mov rax, qword [rbp-40]
  mov rdi, 13
  call strlen_specnull
  mov dword [rbp-44], eax
  mov rax, qword [rbp-40]
  mov rdi, handleWebSocketConnection@?keybuffer
  movsx rsi, dword [rbp-44]
  call strcpylen
  mov rax, qword [rbp-32]
  movsx rdx, dword [rbp-44]
  add rax, rdx
  mov qword [rbp-32], rax
  mov rax, handleWebSocketConnection@?magic_string
  mov rdi, qword [rbp-32]
  call strcpy
  mov rax, 1
  mov rdi, handleWebSocketConnection@?keybuffer
  mov rsi, 61
  call SYS_write
  mov rax, 1
  mov rdi, 10
  call printc
  mov rax, handleWebSocketConnection@?keybuffer
  call strlen
  mov dword [rbp-44], eax
  mov rax, 1
  mov rdi, handleWebSocketConnection@?prompt
  call prints
  mov rax, 0
  mov rdi, handleWebSocketConnection@?promptBuffer
  mov rsi, 128
  call SYS_read
  mov dword [rbp-48], eax
  dec dword [rbp-48]
  mov dword [rbp-52], 28
  mov rax, handleWebSocketConnection@?promptBuffer
  mov rdi, handleWebSocketConnection@?base64String
  movsx rsi, dword [rbp-48]
  mov rdx, 128
  call hexToBase64
  mov rax, handleWebSocketConnection@?handshakeBegining
  mov rdi, handleWebSocketConnection@?handshake
  call strcpy
  lea rax, [handleWebSocketConnection@?handshake]
  mov qword [rbp-60], rax
  mov rax, handleWebSocketConnection@?handshakeBegining
  call strlen
  mov dword [rbp-44], eax
  mov rax, qword [rbp-60]
  movsx rdx, dword [rbp-44]
  add rax, rdx
  mov qword [rbp-60], rax
  mov rax, handleWebSocketConnection@?base64String
  mov rdi, qword [rbp-60]
  movsx rsi, dword [rbp-52]
  call strcpylen
  mov rax, qword [rbp-60]
  movsx rdx, dword [rbp-52]
  add rax, rdx
  mov qword [rbp-60], rax
  mov byte [rbp-61], 13
  mov rax, qword [rbp-60]
  mov dl, byte [rbp-61]
  mov byte [rax], dl
  inc qword [rbp-60]
  mov byte [rbp-61], 10
  mov rax, qword [rbp-60]
  mov dl, byte [rbp-61]
  mov byte [rax], dl
  inc qword [rbp-60]
  mov byte [rbp-61], 13
  mov rax, qword [rbp-60]
  mov dl, byte [rbp-61]
  mov byte [rax], dl
  inc qword [rbp-60]
  mov byte [rbp-61], 10
  mov rax, qword [rbp-60]
  mov dl, byte [rbp-61]
  mov byte [rax], dl
  inc qword [rbp-60]
  mov byte [rbp-61], 0
  mov rax, qword [rbp-60]
  mov dl, byte [rbp-61]
  mov byte [rax], dl
  inc qword [rbp-60]
  mov rax, 1
  mov rdi, handleWebSocketConnection@?handshake
  call prints
  movsx rax, dword [rbp-4]
  mov rdi, handleWebSocketConnection@?handshake
  call prints
.exit:
  add rsp, 77
  pop rbp
  ret
main:
  push rbp
  mov rbp, rsp
  sub rsp, 100
  mov rax, main@?filename
  call getfile
  mov qword [rbp-8], rax
  mov rax, qword [rbp-8]
  mov rdx, qword [rax+4]
  mov qword [rbp-16], rdx
  mov edx, dword [rax+0]
  mov dword [rbp-20], edx
  mov dword [rbp-24], 1
  lea rax, [rbp-24]
  mov qword [rbp-32], rax
  mov rax, 2
  mov rdi, 1
  mov rsi, 0
  call socket
  mov dword [rbp-36], eax
  movsx rax, dword [rbp-36]
  mov rdi, 1
  mov rsi, 2
  mov rdx, qword [rbp-32]
  mov r10, 4
  call setsockopt
  mov word [rbp-38], 8080
  mov word [rbp-58], 2
  mov qword [rbp-54], 0
  movsx rax, word [rbp-38]
  call bitswap16
  mov word [rbp-56], ax
  lea rax, [rbp-58]
  mov qword [rbp-66], rax
  movsx rax, dword [rbp-36]
  mov rdi, qword [rbp-66]
  mov rsi, 16
  call bind
  mov byte [rbp-67], 1
  jmp .whilecmp11
.while11:
  movsx rax, dword [rbp-36]
  mov rdi, 5
  call listen
  movsx rax, dword [rbp-36]
  mov rdi, 0
  mov rsi, 0
  call accept
  mov dword [rbp-71], eax
  mov rax, 1
  mov rdi, main@?msgConnect
  call prints
  mov rax, 1
  mov rdi, 10
  call printc
  mov dword [rbp-75], 0
  lea rax, [rbp-75]
  mov qword [rbp-83], rax
  movsx rax, dword [rbp-71]
  mov rdi, qword [rbp-83]
  call readHandShake
  mov byte [rbp-84], al
  movsx eax, byte [rbp-84]
  mov edx, 0
  cmp eax, edx
  jge .if23
  mov rax, 1
  mov rdi, main@?msgNoRead
  call prints
  jmp .escape34
.if23:
  movsx eax, byte [rbp-84]
  mov edx, 1
  cmp eax, edx
  jne .elif130
  mov rax, 1
  mov rdi, main@?msgHTMLRQ
  call prints
  movsx rax, dword [rbp-71]
  mov rdi, qword [rbp-20]
  movsx rsi, dword [rbp-12]
  call SYS_write
  jmp .escape34
.elif130:
  movsx eax, byte [rbp-84]
  mov edx, 2
  cmp eax, edx
  jne .elif131
  mov rax, 1
  mov rdi, main@?msgWebSocketRQ
  call prints
  movsx rax, dword [rbp-71]
  movsx rdi, dword [rbp-75]
  call handleWebSocketConnection
.escape34:
.elif131:
  movsx rax, dword [rbp-71]
  call SYS_closefd
.whilecmp11:
  cmp byte [rbp-67], 0
  jne .while11
  movsx rax, dword [rbp-36]
  call SYS_closefd
.exit:
  add rsp, 100
  pop rbp
  mov rax, 60
  mov rdi, 0
  syscall
SECTION .data:
  base64@?errmsg: db "(base64) Not enough space in write buffer :(", 10, "You need this many bytes: ", 0
  base64@?b64table: db "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/", 0
  readHandShake@?HTMLRQ: db "text/html", 0
  readHandShake@?webSocketRQ: db "Sec-WebSocket-Version: 13", 0
  handleWebSocketConnection@?keyField: db "Sec-WebSocket-Key:", 0
  handleWebSocketConnection@?errmsgNoKey: db "The socket did not have a key or the server could not find it.", 10, "", 0
  handleWebSocketConnection@?magic_string: db "258EAFA5-E914-47DA-95CA-C5AB0DC85B11", 0
  handleWebSocketConnection@?prompt: db "Input SHA-1 in hex of given key: ", 0
  handleWebSocketConnection@?handshakeBegining: db "HTTP/1.1 101 Switching Protocols", 13, "", 10, "Upgrade: websocket", 13, "", 10, "Connection: Upgrade", 13, "", 10, "Sec-WebSocket-Accept: ", 0
  main@?filename: db "./webpage.html", 0
  main@?msgConnect: db "    got connection", 0
  main@?msgNoRead: db "no read", 10, "", 0
  main@?msgHTMLRQ: db "got an html request", 10, "", 0
  main@?msgWebSocketRQ: db "got a request from a websocket", 10, "", 0

SECTION .bss
  hexToBase64@?binaryOut: resb 200
  readHandShake@?read_buffer: resb 1000
  handleWebSocketConnection@?keybuffer: resb 61
  handleWebSocketConnection@?promptBuffer: resb 128
  handleWebSocketConnection@?base64String: resb 128
  handleWebSocketConnection@?handshake: resb 1000

