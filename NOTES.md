Don't special case things!

read a token from input stream
look it up
    if cif execute
    else value
    free token

cif execute
    foreach arg
        read token from stream
            argi = eval_type
    call function
    foreach arg
        free arg

## Parsing

https://web.archive.org/web/20090824073244/http://pplab.snu.ac.kr/courses/adv_pl05/papers/p261-knuth.pdf

Ole-Johan Dahl syntax.

`loop; S; while ~B: T; repeat;`

```
                   {P}S{Q}
               {Q and ~B}T{P}
-------------------------------------------
{P} loop: S; while B: T; repeat; {Q and ~B} 
```
loop {S} while {!B} {T} repeat
loop while {!B} {T} repeat
loop {S} while {!B} repeat
loop {S} repeat // break and continue?

if {B} then {T} else {F}
if {B} then {T}

for {C} {S} // for {var i iota} { print i }

Use file descriptors.

Use backtick to execute shell command in child and open pipe to read.

`token_read` fd -> token

`scan_token` token -> type

`eval_token`

## Token

A _token_ is a string of characters. String. Block.
