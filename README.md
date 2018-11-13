# Lisp in C

TODO: address TODO/FIXME in file
TODO: toc

TODO: intro

## The language

### Special forms

#### cond

special form: **cond** *clause* *clause* ...

Each *clause* has the form *predicate expression*.

Evaluates to the result of evaluating the first *expression* whose *predicate*
evaluates to true.

    > (cond (#t 1) (#t 2) (#f 3))
    1
    > (cond ((int? ()) ()) ((int? 5) 5))
    5
    > (cond ((null? 1) 0) ((null? 2) 1) (#t 2))
    2

#### define

special form: **define** *name* *definition*

Binds the symbol *name* to the result of evaluating *definition*.

    > (define x 1)
    > (define y 2)
    > (cons x y)
    (1 . 2)

#### lambda

special form: **lambda** *args* *body*

Evaluates to a function whose parameter names are given by *args* and whose
body is given by *body*, where *args* is a list of symbols and *body* is a
single expression.

    > (define add (lambda (x y) (+ x y)))
    #<function>[()](x y)->(+ x y)
    > (add 1 2)
    3

TODO: lexical scope

#### quote

special form: **quote** *object*

Evaluates to *object*.

    > (quote foo)
    foo
    > (quote (1 2 3))
    (1 2 3)
    > (quote (/ 10 2))
    (/ 10 2)
