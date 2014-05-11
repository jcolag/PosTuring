PosTuring
=========

Interpreter for a programming language based on the Post-Turing Machine

###Post-Turing###

The [Post-Turing Machine](https://en.wikipedia.org/wiki/Post-Turing_machine) is a model developed by [Emil Post](https://en.wikipedia.org/wiki/Emil_Post) as a _program formulation_ variation of a [Turing Machine](https://en.wikipedia.org/wiki/Turing_machine).

A Post–Turing machine operates on an infinite sequence of storage locations, each containing one of a pair of symbols (a binary alphabet), by moving through the sequence in either direction and altering contents one element at a time.  Post described the model in 1936 as directions given to a [Human Computer](https://en.wikipedia.org/wiki/Human_computer).

The worker (comparable to a Turing Machine's "tape head") can move to the left or right, write to the current box, or make a decision based on the contents of the current box.

###The Davis Model###

PosTuring is based primarily on a variation of the 1978 model developed by [Martin Davis](https://en.wikipedia.org/wiki/Martin_Davis).  As such, it strongly resembles the syntax of simple imperative programming languages like BASIC, while still retaining its own minimalist flavor.

####Syntax####

PosTuring has four kinds of statements:

 - __`Right`__:  Move the {worker, tape head, phantom pterosaur} one box to the right from its current position.
 - __`Left`__:  Move the {worker, tape head, phantom pterosaur} one box to the left from its current position.
 - __`Print`__ _x_:  Replace the value of the current box with _x_.  Obviously, due to the strict binary nature of the Post-Turing machine architecture, _x_ may only be the values `0` or `1`.
 - __`If`__ _x_ __`Goto`__ _y_:  Test the contents of the current box against _x_.  If they match, transfer control to the first statement after label _y_.

Labels are represented single ASCII characters.  Define them in the program by placing them in square-brackets, such as `[A]` or `[{]` or `[]]`, though implementations are not required to support the last, for obvious reasons.  Use the label in targets by name, such as:

    If 0 Goto A
    If 1 Goto {
    If 0 Goto ]

Technically, the above is a lie.  The parser is written to be case-insensitive and only check the first character of each statement and make decisions assuming that any syntax that doesn't represent data can be ignored.  Therefore, `Right` can also be `right` or `R` or `roundaboutnineoclock`, if you prefer.  `If 0 Goto A` could very well be `indescribable 0 !@#blah$% X`.  Too many programming languages worry that the keywords are _just right_, when the result is just a loss of poetry...

###The PosTuring Interpreter###

Run the intepreter, giving the program name as a command-line parameters.  The interpreter will read the program and request:

    Enter initial conditions:

This is the initial data in the boxes (or on the tape), encoded as `0`s and `1`s, terminated with a carriage return (the Enter key).  Note that the implementation currently places a maximum length of 1,024 boxes on the state.  Anything more will result in nuclear holocaust, because it's a safe bet that I didn't bother to check the bounds.  This is _old_ code.

The interpreter will then execute the program, showing the contents of the boxes after each instruction until the program is complete.

###Programming in PosTuring###

The syntax, above, are presumably clear enough, and the semantics obvious from it.

It is also useful to know that the program terminates when there are no more statements.  That is, after the interpreter executes the last statement in the program (assuming it doesn't transfer control elsewhere), the program exits and the interpreter ends.

It is also useful to consider the format of data.  Because each box contains only a single on/off state (a `0` or `1`) and there is no information dividing one "data structure" from another, most values will need to be represented in _unary_ form, a string of bits that represents a value by its size.

At this time, there is no input or output facility other than direct, batch tape manipulation.  Future versions may concoct something appropriate to the data model and which doesn't impact the syntax too badly.

###Examples###

Four examples are provided with this distribution:

 - _null.pt_:  An empty program.  It, naturally, does nothing.
 - _add1.pt_:  Takes an initial number, represented in unary notation, and increments the number by 1.
 - _twice1.pt_:  Doubles a number, represented in unary.
 - _twice2.pt_:  Not strict PosTuring, doubles a number represented in _binary_ notation, assuming a third symbol (`X`) can be used as an alphabet.  `0`s are used as delimiters, and the `X`s are used for binary zeroes.

###Notes###

This is _really_ old code.  I don't actually remember writing it.  I don't remember if I ever released it anywhere else.  It's also fairly ugly and does semi-strange things that a good programmer really wouldn't appreciate.  So, if you fall into a bottomless pit poking around in the source, don't come running to me...
