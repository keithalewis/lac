Don't special case things!

read a token from input stream, look it up, execute

eval fp 
    read token 
    lookup
    if cif exec
    else value

eval_type fp type
    read token
    convert to type

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
Use file descriptors.

Use backtick to execute shell command in child and open pipe to read.

`token_read` fd -> token

`scan_token` token -> type

`eval_token`


