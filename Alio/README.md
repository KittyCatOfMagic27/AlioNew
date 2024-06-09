# Alio #
"Another" language written in C++
  
Originally Written for Debian Linux. 
Currently being developed in Ubuntu.

## Setup ##
Use the alio compiler through the shell script given. (Does not work in Ubuntu.)

```
usr@penguin:~/cwd$ alio --help
usr@penguin:~/cwd$ alio -f main.alio
usr@penguin:~/cwd$ ./main
```
Alternatively, make sure you have the following programs installed:
- nasm
- ld linker
- g++ 
And then simply run the following commands with ADDR being the location of this install.
```
usr@penguin:~/cwd$ g++ ADDR/alio.cpp -o ./alio -fpermissive
usr@penguin:~/cwd$ ./alio --help
usr@penguin:~/cwd$ ./alio -f main.alio
usr@penguin:~/cwd$ ./main
```

## Docs ##

### Procedures ###
   
Procedures are Alio's version of a funtion. They consist of two parts, the head and body. The head is the portion of code in between the label of the procedure and the begin keyword. In this part of the procedure we specify the in and out variables and other attributes of the procedure. After the begin keyword is the body of the procedure, this is the code that is executed when the procedure is called. The final thing about procedures is that they end (like all blocks in Alio) with the end keyword. For now the entry point is main, later this will have to be specified at the begining of the program.

```
proc main
in uint argc
in ptr argv
begin
  # Your code here :D
end
```

### Types ###
These are the current types in Alio and their sizes.
    
- uint      - 4
- ptr       - 8
- int       - 4 (currently treated as a uint, will maybe be fixed)
- char      - 1
- bool      - 1 
- long      - 8 (not available in 32bit mode)
- string    - variable 
- struct    - variable 
- short     - 2
- ptr(type) - 8
  
Something interesting about Alio is that most operators are procedually created at compile time. So the 5 basic oparators (+, -, *, /, %) can be used with any combination of the existing types except for strings and structs. A thing to note about this is that it does narrow the types while operating. Be mindful of that and note that operating on mismatching types in the way you can here would give you an -fpermissive warning in C++.

### Operators ###

The current list of operators in the order they appear in modules/lexer.h.
```
x + y         Adding two values
x - y         Subtracting two values
x * y         Multiplying two values
x / y         Dividing two values
x % y         Getting the modulo x of y / The remainder of x / y
var++         Increase the value in the variable by 1
var--         Increase the value in the variable by 2
x > y         Returns true if x is greater than y
x < y         Returns true if x is less than y
x <= y        Returns true if x is less than or equal to y
x >= y        Returns true if x is greater tha or equal to y
x = y         Returns true if x and y are equal
x != y        Returns true if x and y are not equal
x && y        Returns true if x and y are true
x -& y        Each bit is 1 only if it is 1 in both x and y
x | y         Each bit is 1 if it is 1 in x or y
!x            If x is true this returns false and vice versa    
^             Each bit is 1 if it is 1 in ONLY x or y
x << number   The bits in x are shifted to the left by number
x >> number   The bits in x are shifted to the right by number
@x            Get value at address of pointer/Assign to pointer address
```
Notice how = is a comparison operator and there is no assignment operator. In Alio and it is simply denoted as `int var x 0`, dropping the assignment operator. This decision was made hastily because I (kat/AlioDev) thought it would look cool, it is deeply regretted. The problems that arised by not having an assignment operator were/are not very fun problems.

### Compier Directives ###
These are writen as ;;DIRECTIVE. There are a couple of them and many are not used, don't work, or are currently useless as they hinge on features to be added in the future like linking assembly code.
```
;;include INCLUDE_METHOD FILE
    INCLUDE_METHOD: static (dynamic is also planned to be added)
    FILE: "./file.alio", <includefile>
;;define LABEL LITERAL_VALUE
    LABEL: a name that gets replaced by LITERAL_VALUE
    LITERAL_VALUE: a value such as a number, character, ect.
;;def LIBRARY_NAME
    LIBRARY_NAME: a name that is checked in ifdef and ifndef
;;ifdef LIBRARY_NAME
    LIBRARY_NAME: a name that is checked in ifdef and ifndef
    The condition is true if the LIBRARY_NAME is defined
    Includes all code after if condition is true all the way up to the associated ;;fi
;;ifndef LIBRARY_NAME
    LIBRARY_NAME: a name that is checked in ifdef and ifndef
    The condition is true if the LIBRARY_NAME is not defined
    Includes all code after if condition is true all the way up to the associated ;;fi
;;fi
    Ends an ;;if___ directive.
;;setentry ENTRYPOINT
    ENTRYPOINT: the procedure which is ran first when the program is ran, by default this value is main
    This sets a new ENTRYPOINT.

Both of the following are currently useless and meant to refernce external procedures with the inclusion of sys/win32 and assembly code respectively.
;;win32kernproc PROC
;;externproc PROC
    PROC: A procedure from another program not written in Alio that is linked to the Alio program
```

### Pointers ###
Pointers store an address to a place in memory. Most of the time this is to a variable. They have two unique operators. The first is the '&' operator used to get the address of a variable. The second is the '@' operartor, this is used to get the value at the address.
```
proc main
begin
  int x 5
  ptr y &x
  int z @x
end
```

### Strings ###
Strings in Alio aren't traditional strings; they are simply used as sectors of memory. For example, you can have a string that is `string x[16]` which is 16 bytes of memory. You *could* use this as a string of 16 characters (traditional use) or as an array of 4 integers. By that logic it could be used as an array of 2 pointers ect. Note that strings that are not static ARE NOT null-terminated; not ending in a byte of value zero. Static strings are automatically null-terminated. Something else that is noteworthy is that strings can be assigned from string literals and only static strings support escape codes for now. Static strings declared with a value cannot be written to.
```
proc main
begin
  static string hi "Hello Word!\n"
end
```

### Static ###
Static variables are stored in the BSS and DATA sections. This means they have some different properties from normal variables and are slightly slower to access. One of these differences is that if you give the static variable an initial value it CANNOT be written to as it is acting as a const in the DATA section.
```
;;include static <stdlib>

proc main
begin
  static string hello "Hello World!"
  ptr z &hello
  uint length strlen(z)
  SYS_write(fd z length)
end
```

### Syscalls ###
System Calls are used to interface with the linux kernel. The first argument is the syscall ID and the rest are the arguments to the call. Syscalls can also return values. You don't have to worry about these too much if you don't want to since some (and soon a lot more) are given convienent wrappers in the stdlib header file.
```
proc SYS_write
in uint fd
in ptr buffer
in uint size
begin
  syscall(1 fd buffer size)
end
```

### While ###
The keyword while instantiates a loop, currently it runs while the checked value or expression is not 0.
```
;;include static <stdlib>

proc main
begin
  static string hi "Hello World"
  ptr str &hi
  # strlen
  ptr beginning str
  char c @str
  while(c)
    str++
    c @str
  end
  uint len str-beginning
  # Print
  str &hi
  SYS_write(STDOUT str length)
end
```

### If ###
If activates the code within the block if and only if the input or output of the expression is not 0. 
```
proc main
begin
  uint x 69
  bool eq x = 420
  if(eq)
    static string y "Dank"
    ptr str &y
    uint length strlen(str)
    SYS_write(STDOUT str length)
  end
end
```

### Else and Else If ##
The keywords 'else' and 'else if' are put to after if blocks and terminate the if blocks instead of having to use the 'end' keyword. Else is put as the last in a chain and activates if none of the other previous expressions were true. Else if is put after an if or else if, it only actiavtes if everything before it was false and its expression is true. 
Fun fact, I barely even implemented else if. It was originally going to be elif, but what i had written with if and else somehow just worked with else if. It only needed a little tweaking :p
```
proc main
begin
  uint x 69
  if(x = 420)
    static string msg0 "Dank\n"
    ptr str &msg0
    uint length strlen(str)
    SYS_write(STDOUT str length)
  else if(x = 69)
    static string ms1 "Nice\n"
    str &msg1
    length strlen(str)
    SYS_write(STDOUT str length)
  else
    static string msg2 "Lame\n"
    str &msg2
    length strlen(str)
    SYS_write(STDOUT str length)
  end
end
```

### Structs ###
A struct is a new user defined type! It consists of other types. In this case it constains x and y both of them being uints. It can be used just like a native type (in most cases). To access the variables within the struct, called fields, you can put the variable name, a dot, and then the field name. 
```
;;include static <stdlib>

struct A
begin
    uint x
    uint y
end

proc A::sum
out uint s
begin
    A var
    var @self
    s var.x+var.y
end

proc main
begin
    A var
    var.x 10
    var.y 20
    uint s var.sum()
    printu(STDOUT s)
    printc(STDOUT 10)
end
```

### Methods ###
You can add methods to types or structs, the value it is operated upon is located in the ptr self.
```
;;include static <stdlib>

struct A
begin
    uint x
    uint y
end

proc uint::add
in uint y
begin
    uint x @self
    x x+y
    @self x
end

proc main
begin
    uint x 420
    x.add(69)
    printu(STDOUT x)
    printc(STDOUT 10)
end
```

### Typed Pointers and -> ###
Typed Pointers allow you to get a field of an instance of a struct even if it is just a pointer with the operator '->'. Typed Pointers have yet to be fully integrated so use them sparingly to avoid a confusing error or crash (which if you have used this compiler you're most likely very familiar with). 

```
;;include static <stdlib>

struct Ab
begin 
    uint x
    uint y
    uint z
end 

proc readStruct
in ptr(Ab) thinger          # typed pointer of a struct 
begin
    int y thinger->z        # gets the value of the field in the struct pointed to
    printu ( STDOUT y )
    printc ( STDOUT 10 )
end
```

### Global ###
Takes static variable from another proc. Can be an anynomus or specified proc (advised to use specified). Specified `global procname::var`. Anynomus `global var`.
```
;;include static <stdlib>

proc foo
begin
  static string f[12]
end

proc main
begin
  foo()
  global foo::f
  static string hi "Hello World!\n"
  strcpy(hi f)
  prints(STDOUT f)
end
```

### Persist ###
The keyword persist keeps the procedure even if it is not called anywhere. It is a good way to make global variables in tandem with the global keyword.
```
persist proc __rand_scope
begin
    static long step 0
end

proc hash_seed
out long newseed
begin
    global __rand_scope::step
    step++
    . . .
end

```

### Dumb Conventions ###
There is a long list of them, you can read through the standard library and probably will be able to notice a lot of them. One that should be followed for best practice in alio is the ptr naming convention. This will save the user a lot of time in mixing up ptrs, ptr types, ect. The ptr convetion is if you are making a ptr to a variable it is declared as `ptr __x &x`. This way you know exactly what type is and where it points to. Another set of convention is naming by type since there are no extensions for Alio text highlighting and it may be difficult to remember the types of everything. Here is a list of common names for each type and use case:
- str for ptrs that point to a string of characters
- x, y, z for uints
- i, j, k for iterators (same as a for loop in C) 
- c for char
- byte for bool or char
- dstr for dynamic_string (in the library dynamic_string)
- fd for file directory numbers (uint)
- buffer a static string\[bytes\] or memory allocated ptr
- addr for memory allocated ptr
- SYS_procname for implemented system calls (the procname is the name of the system call, if it works without SYS_ that means it is in a wrapper)
- proc __procname for internal procedures that should not be called by a user

## Goals ##
- Document Standard Libraries 
- Syntax Highlighting for .alio files in Visual Studio Code
- resupport x86 mode (elf32/i386) (dead goal for now)
- Add %tmp1, %tmp2, %tmp3... for expression expansions in intermediate (working towards, search for the ENUM_TYPE::OPERATOR in modules/lexer.h)
- Write a basic alio compiler in alio
