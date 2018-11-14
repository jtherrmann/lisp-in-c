# Lisp in C

TODO: address TODO/FIXME in file
TODO: toc

TODO: intro

## The language

### Objects

#### ints

An int is a signed integer and evaluates to itself.

    > 5
    5
    > -2
    -2
    > (+ 1 2)
    3

#### symbols

A symbol evaluates to the object to which it is bound.

    > (define x 3)
    > x
    3
    > (quote x)
    x

TODO: rename bool? to boolean? in code
#### boolean

A boolean is either `#t` (true) or `#f` (false) and evaluates to itself.

    > #t
    #t
    > #f
    #f
    > (null? ())
    #t

#### pairs and lists

#### functions

### Special forms

#### cond

special form: **cond** *clause clause ...*

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
    > (add 1 2)
    3

Functions are lexically scoped:

    > (define addx (lambda (x) (lambda (n) (+ n x))))
    > (define add2 (addx 2))
    > (add2 3)
    5

#### quote

special form: **quote** *object*

Evaluates to *object*.

    > (quote foo)
    foo
    > (quote (1 2 3))
    (1 2 3)
    > (quote (/ 10 2))
    (/ 10 2)
