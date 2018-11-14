# Lisp in C

TODO: address TODO/FIXME in file
TODO: toc

TODO: intro

## Objects

### Ints

An int is a signed integer and evaluates to itself.

    > 5
    5
    > -2
    -2
    > (+ 1 2)
    3

### Symbols

A symbol evaluates to the object to which it is bound.

    > (define x 3)
    > x
    3
    > (quote x)
    x

### Bools

A bool is either true, `#t`, or false, `#f`, and evaluates to itself.

    > #t
    #t
    > #f
    #f
    > (null? ())
    #t

### Pairs and lists

A pair is an object with two data members, car and cdr.

    > (define p (cons 1 2))
    > p
    (1 . 2)
    > (car p)
    1
    > (cdr p)
    2

A list is the empty list, `()`, or any pair whose cdr is a list:

    > (define l (cons 1 (cons 2 (cons 3 ()))))
    > l
    (1 2 3)
    > (cdr l)
    (2 3)
    > (cdr (cdr (cdr l)))
    ()
    > (pair? l)
    #t
    > (list? l)
    #t

The empty list evaluates to itself, while a non-empty list evaluates as a
function application:

    > ()
    ()
    > (+ 1 2)
    3
    > (define sum3 (lambda (x y z) (+ x (+ y z))))
    > (sum3 1 2 3)
    6

A non-list pair cannot be evaluated.

### Functions

A function returns a result when applied to zero or more arguments. A function
may be built-in or created with `lambda` and evaluates to itself.

    > cons
    #<builtin function: cons>
    > (eval cons)
    #<builtin function: cons>
    > (cons 1 2)
    (1 . 2)

Functions created with `lambda` are lexically scoped:

    > (define addx (lambda (x) (lambda (n) (+ n x))))
    > (define add2 (addx 2))
    > (add2 3)
    5

## Special forms

### cond

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

### define

special form: **define** *name* *definition*

Binds the symbol *name* to the result of evaluating *definition*.

    > (define x 1)
    > (define y 2)
    > (cons x y)
    (1 . 2)

### lambda

special form: **lambda** *args* *body*

Evaluates to a function whose parameter names are given by *args* and whose
body is given by *body*, where *args* is a list of symbols and *body* is a
single expression.

    > (define add (lambda (x y) (+ x y)))
    > (add 1 2)
    3

### quote

special form: **quote** *object*

Evaluates to *object*.

    > (quote foo)
    foo
    > (quote (1 2 3))
    (1 2 3)
    > (quote (/ 10 2))
    (/ 10 2)

## Builtin functions

### eval

`eval` evaluates an object as an expression.

    > (quote (+ 1 2))
    (+ 1 2)
    > (eval (quote (+ 1 2)))
    3
    > (define x 1)
    > (quote x)
    x
    > (eval (quote x))
    1

### cons, car, and cdr

`cons` constructs a pair, while `car` and `cdr` return the car and cdr of a
pair, respectively.

    > (cons 1 2)
    (1 . 2)
    > (cons 1 (cons 2 (cons 3 ())))
    (1 2 3)
    > (car (cons 1 2))
    1
    > (cdr (cons 1 2))
    2

### length

`length` returns the number of pairs in a list.

    > (length ())
    0
    > (length (cons 1 ()))
    1
    > (length (cons 1 (cons 2 (cons 3 ()))))
    3
    > (length (quote (1 2 3 4 5)))
    5

### Arithmetic operators

`+`, `-`, `*`, `/`

Each takes two arguments.

    > (+ 1 2)
    3
    > (- 7 3)
    4
    > (* 5 2)
    10
    > (/ 10 2)
    5
    > (/ 15 2)
    7
    > (/ 1 2)
    0

### Logical operators

`and`, `or`, `not`

    > (and #t #f)
    #f
    > (or #t #f)
    #t
    > (not #t)
    #f

### Comparison functions

`equal?`, `<`, `<=`, `>`, `>=`

    > (equal? 1 2)
    #f
    > (equal? (quote (1 2 3)) (quote (1 2 3)))
    #t
    > (< 1 2)
    #t
    > (< 5 1)
    #f
    > (>= 3 3)
    #t

### Type predicates

`int?`, `symbol?`, `bool?`, `pair?`, `list?`, `null?`, `function?`

Each takes an argument *object* and returns true if *object* is of the
corresponding type and false otherwise.

    > (int? 5)
    #t
    > (symbol? (quote foo))
    #t
    > (pair? (cons 1 2))
    #t
    > (list? (cons 1 2))
    #f
    > (pair? (cons 1 ()))
    #t
    > (list? (cons 1 ()))
    #t
    > (pair? ())
    #f
    > (list? ())
    #t
    > (null? ())
    #t
