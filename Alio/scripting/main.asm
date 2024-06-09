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
__base64_ARR8_INDEXOF:
  push rbp
  mov rbp, rsp
  sub rsp, 11
  mov qword [rbp-8], rax
  mov byte [rbp-9], dil
  mov byte [rbp-12], 0
  mov rax, qword [rbp-8]
  mov dl, byte [rax]
  mov byte [rbp-11], dl
  jmp .whilecmp2
.while2:
  inc qword [rbp-8]
  inc byte [rbp-12]
  mov rax, qword [rbp-8]
  mov dl, byte [rax]
  mov byte [rbp-11], dl
.whilecmp2:
  mov al, byte [rbp-11]
  mov dl, byte [rbp-9]
  cmp al, dl
  jne .while2
.exit:
  mov al, byte [rbp-12]
  add rsp, 11
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
  mov dword [rbp-28], eax
  mov eax, dword [rbp-28]
  mov edx, 3
  mul edx
  mov dword [rbp-28], eax
  xor rdx, rdx
  mov eax, dword [rbp-28]
  mov ebx, 4
  div ebx
  mov dword [rbp-28], eax
  mov eax, dword [rbp-28]
  mov edx, dword [rbp-24]
  cmp eax, edx
  jle .if2
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
.if2:
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
  jmp .whilecmp3
.while3:
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
.whilecmp3:
  movsx eax, byte [rbp-61]
  mov edx, 0
  cmp eax, edx
  jg .while3
  mov rax, qword [rbp-36]
  mov rdx, qword [rbp-44]
  sub rax, rdx
  mov byte [rbp-61], al
  cmp byte [rbp-61], 0
  je .if3
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
  jne .if4
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
  jmp .escape19
.if4:
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
.escape19:
  mov rax, qword [rbp-52]
  mov dl, "="
  mov byte [rax], dl
.if3:
.exit:
  add rsp, 85
  pop rbp
  ret
base64_decode:
  push rbp
  mov rbp, rsp
  sub rsp, 86
  mov qword [rbp-8], rax
  mov qword [rbp-16], rdi
  mov dword [rbp-20], esi
  mov dword [rbp-24], edx
  mov eax, dword [rbp-20]
  mov dword [rbp-28], eax
  mov eax, dword [rbp-28]
  mov edx, 3
  mul edx
  mov dword [rbp-28], eax
  xor rdx, rdx
  mov eax, dword [rbp-28]
  mov ebx, 4
  div ebx
  mov dword [rbp-28], eax
  mov eax, dword [rbp-28]
  mov edx, dword [rbp-24]
  cmp eax, edx
  jle .if5
  mov rax, 2
  mov rdi, base64_decode@?errmsg
  call prints
  mov rax, 2
  movsx rdi, dword [rbp-28]
  call printi
  mov rax, 2
  mov rdi, 10
  call printc
  mov rax, 4294967294
  call SYS_exit
.if5:
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
  mov rdx, 0
  cmp rax, rdx
  setg al
  mov byte [rbp-61], al
  mov rax, qword [rbp-44]
  mov rdi, 3
  call __base64_ARR8_INDEX
  mov byte [rbp-62], al
  mov al, byte [rbp-62]
  mov dl, "="
  cmp al, dl
  setne al
  mov byte [rbp-62], al
  mov al, byte [rbp-62]
  mov dl, byte [rbp-61]
  and al, dl
  mov byte [rbp-61], al
  jmp .whilecmp4
.while4:
  mov byte [rbp-63], 0
  mov byte [rbp-64], 0
  mov byte [rbp-65], 0
  mov byte [rbp-66], 0
  mov dword [rbp-70], 0
  mov rax, qword [rbp-44]
  mov dl, byte [rax]
  mov byte [rbp-63], dl
  mov rax, base64_decode@?b64table
  movsx rdi, byte [rbp-63]
  call __base64_ARR8_INDEXOF
  mov byte [rbp-63], al
  mov al, byte [rbp-63]
  shl al, 2
  mov byte [rbp-63], al
  mov rax, qword [rbp-44]
  mov rdi, 1
  call __base64_ARR8_INDEX
  mov byte [rbp-64], al
  mov rax, base64_decode@?b64table
  movsx rdi, byte [rbp-64]
  call __base64_ARR8_INDEXOF
  mov byte [rbp-64], al
  mov al, byte [rbp-64]
  shr al, 4
  mov byte [rbp-64], al
  movsx eax, byte [rbp-64]
  mov edx, 0x03
  and eax, edx
  mov byte [rbp-64], al
  mov al, byte [rbp-64]
  mov dl, byte [rbp-63]
  or al, dl
  mov byte [rbp-64], al
  mov rax, qword [rbp-52]
  mov dl, byte [rbp-64]
  mov byte [rax], dl
  inc qword [rbp-52]
  mov rax, qword [rbp-44]
  mov rdi, 1
  call __base64_ARR8_INDEX
  mov byte [rbp-63], al
  mov rax, base64_decode@?b64table
  movsx rdi, byte [rbp-63]
  call __base64_ARR8_INDEXOF
  mov byte [rbp-63], al
  mov al, byte [rbp-63]
  shl al, 4
  mov byte [rbp-63], al
  mov rax, qword [rbp-44]
  mov rdi, 2
  call __base64_ARR8_INDEX
  mov byte [rbp-64], al
  mov rax, base64_decode@?b64table
  movsx rdi, byte [rbp-64]
  call __base64_ARR8_INDEXOF
  mov byte [rbp-64], al
  mov al, byte [rbp-64]
  shr al, 2
  mov byte [rbp-64], al
  mov al, byte [rbp-64]
  mov dl, byte [rbp-63]
  or al, dl
  mov byte [rbp-64], al
  mov rax, qword [rbp-52]
  mov dl, byte [rbp-64]
  mov byte [rax], dl
  inc qword [rbp-52]
  mov rax, qword [rbp-44]
  mov rdi, 2
  call __base64_ARR8_INDEX
  mov byte [rbp-63], al
  mov rax, base64_decode@?b64table
  movsx rdi, byte [rbp-63]
  call __base64_ARR8_INDEXOF
  mov byte [rbp-63], al
  mov al, byte [rbp-63]
  shl al, 6
  mov byte [rbp-63], al
  mov rax, qword [rbp-44]
  mov rdi, 3
  call __base64_ARR8_INDEX
  mov byte [rbp-64], al
  mov rax, base64_decode@?b64table
  movsx rdi, byte [rbp-64]
  call __base64_ARR8_INDEXOF
  mov byte [rbp-64], al
  mov al, byte [rbp-64]
  mov dl, byte [rbp-63]
  or al, dl
  mov byte [rbp-64], al
  mov rax, qword [rbp-52]
  mov dl, byte [rbp-64]
  mov byte [rax], dl
  inc qword [rbp-52]
  mov rax, qword [rbp-44]
  mov rdx, 4
  add rax, rdx
  mov qword [rbp-44], rax
  mov rax, qword [rbp-36]
  mov rdx, qword [rbp-44]
  sub rax, rdx
  mov qword [rbp-60], rax
  mov rax, qword [rbp-60]
  mov rdx, 0
  cmp rax, rdx
  setg al
  mov byte [rbp-61], al
  mov rax, qword [rbp-44]
  mov rdi, 3
  call __base64_ARR8_INDEX
  mov byte [rbp-62], al
  mov al, byte [rbp-62]
  mov dl, "="
  cmp al, dl
  setne al
  mov byte [rbp-62], al
  mov al, byte [rbp-62]
  mov dl, byte [rbp-61]
  and al, dl
  mov byte [rbp-61], al
.whilecmp4:
  cmp byte [rbp-61], 0
  jne .while4
  mov rax, qword [rbp-44]
  mov rdi, 3
  call __base64_ARR8_INDEX
  mov byte [rbp-61], al
  mov al, byte [rbp-61]
  mov dl, "="
  cmp al, dl
  jne .if6
  mov rax, qword [rbp-44]
  mov dl, byte [rax]
  mov byte [rbp-63], dl
  mov rax, base64_decode@?b64table
  movsx rdi, byte [rbp-63]
  call __base64_ARR8_INDEXOF
  mov byte [rbp-63], al
  mov al, byte [rbp-63]
  shl al, 2
  mov byte [rbp-63], al
  mov rax, qword [rbp-44]
  mov rdi, 1
  call __base64_ARR8_INDEX
  mov byte [rbp-64], al
  mov rax, base64_decode@?b64table
  movsx rdi, byte [rbp-64]
  call __base64_ARR8_INDEXOF
  mov byte [rbp-64], al
  mov al, byte [rbp-64]
  shr al, 4
  mov byte [rbp-64], al
  movsx eax, byte [rbp-64]
  mov edx, 0x03
  and eax, edx
  mov byte [rbp-64], al
  mov al, byte [rbp-64]
  mov dl, byte [rbp-63]
  or al, dl
  mov byte [rbp-64], al
  mov rax, qword [rbp-52]
  mov dl, byte [rbp-64]
  mov byte [rax], dl
  inc qword [rbp-52]
  mov rax, qword [rbp-44]
  mov rdi, 2
  call __base64_ARR8_INDEX
  mov byte [rbp-61], al
  mov al, byte [rbp-61]
  mov dl, "="
  cmp al, dl
  je .if7
  mov rax, qword [rbp-44]
  mov rdi, 1
  call __base64_ARR8_INDEX
  mov byte [rbp-63], al
  mov rax, base64_decode@?b64table
  movsx rdi, byte [rbp-63]
  call __base64_ARR8_INDEXOF
  mov byte [rbp-63], al
  mov al, byte [rbp-63]
  shl al, 4
  mov byte [rbp-63], al
  mov rax, qword [rbp-44]
  mov rdi, 2
  call __base64_ARR8_INDEX
  mov byte [rbp-64], al
  mov rax, base64_decode@?b64table
  movsx rdi, byte [rbp-64]
  call __base64_ARR8_INDEXOF
  mov byte [rbp-64], al
  mov al, byte [rbp-64]
  shr al, 2
  mov byte [rbp-64], al
  mov al, byte [rbp-64]
  mov dl, byte [rbp-63]
  or al, dl
  mov byte [rbp-64], al
  mov rax, qword [rbp-52]
  mov dl, byte [rbp-64]
  mov byte [rax], dl
  inc qword [rbp-52]
.if7:
.if6:
.exit:
  add rsp, 86
  pop rbp
  ret
main:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov rax, main@?str
  mov rdi, main@?buffer
  mov rsi, 7
  mov rdx, 20
  call base64
  mov rax, main@?buffer
  mov rdi, main@?finalResult
  mov rsi, 12
  mov rdx, 20
  call base64_decode
  mov rax, 1
  mov rdi, main@?finalResult
  mov rsi, 20
  call SYS_write
  mov rax, 1
  mov rdi, 10
  call printc
.exit:
  add rsp, 16
  pop rbp
  mov rax, 60
  mov rdi, 0
  syscall
SECTION .data:
  base64@?errmsg: db "(base64) Not enough space in write buffer :(", 10, "You need this many bytes: ", 0
  base64@?b64table: db "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/", 0
  base64_decode@?errmsg: db "(base64_decode) Not enough space in write buffer :(", 10, "You need this many bytes: ", 0
  base64_decode@?b64table: db "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/", 0
  main@?str: db "ABCDEFG", 0

SECTION .bss
  main@?buffer: resb 20
  main@?finalResult: resb 20

