/* This file is a detailed set of abuses of the
   C preprocessor (cpp) that I have written down
   in hopes of having a general file to test them
   out :). Have fun with these, and don't just
   look at them, try them! Use the command:
     cpp -P cpp_tutorial.h
   to see what text is generated. I have to give
   thanks to the following tutorials:

   http://jhnet.co.uk/articles/cpp_magic
   by Jonathan Heathcote

   https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms
   by Paul Fultz II
*/

#define FIRST(a, ...) a
#define SECOND(a, b, ...) b

/* Allows for macro expansion, the ## operator
   prohibits macro expansion when concatenating
   two tokens, so we expand the macro then
   concatenate.
*/
#define CAT(a,b) a ## b

/* If the case is any non-zero token like 'a':
     IF_ELSE(a)(true-case)(false-case)  =>
     _IF_1 (true-case)(false-case) =>
     true-case _IF_1_ELSE (false-case) =>
     true-case
   If the case is zero:
     IF_ELSE(0)(true-case)(false-case)  =>
     _IF_0 (true-case)(false-case) =>
     _IF_1_ELSE (false-case) =>
     false-case
*/
#define IF_ELSE(condition) _IF_ELSE(BOOL(condition))
#define _IF_ELSE(condition) CAT(_IF_, condition)

#define _IF_1(...) __VA_ARGS__ _IF_1_ELSE
#define _IF_0(...)             _IF_0_ELSE

#define _IF_1_ELSE(...)
#define _IF_0_ELSE(...) __VA_ARGS__

/* Returns a list with 1 as the second element */
#define PROBE() ~, 1

/* Takes any single token and returns 0.
   Takes a list of tokens and returns the second
   one, if it is on PROBE() this will be 1.
*/
#define IS_PROBE(...) SECOND(__VA_ARGS__, 0)

/* If x is zero, this should evaluate to calling
   IS_PROBE on PROBE() which will return 1. If
   x is a non-zero token (but not a list) this
   should return 0, see IS_PROBE for why. */
#define NOT(x) IS_PROBE(CAT(_NOT_, x))
#define _NOT_0 PROBE()

/* If the case is any non-zero token:
     Not turns any non-zero token into
     zero. Not then turns zero into one.
   If the case is zero:
     Not turns zero into one.
     Not turns one into zero.
*/
#define BOOL(x) NOT(NOT(x))

#define EMPTY()

/* Expands a lot of macros, good enough. */
#define EVAL(...) EVAL1024(__VA_ARGS__)
#define EVAL1024(...) EVAL512(EVAL512(__VA_ARGS__))
#define EVAL512(...) EVAL256(EVAL256(__VA_ARGS__))
#define EVAL256(...) EVAL128(EVAL128(__VA_ARGS__))
#define EVAL128(...) EVAL64(EVAL64(__VA_ARGS__))
#define EVAL64(...) EVAL32(EVAL32(__VA_ARGS__))
#define EVAL32(...) EVAL16(EVAL16(__VA_ARGS__))
#define EVAL16(...) EVAL8(EVAL8(__VA_ARGS__))
#define EVAL8(...) EVAL4(EVAL4(__VA_ARGS__))
#define EVAL4(...) EVAL2(EVAL2(__VA_ARGS__))
#define EVAL2(...) EVAL1(EVAL1(__VA_ARGS__))
#define EVAL1(...) __VA_ARGS__

/* Delays a macro expansion, if A() is a function like
   macro such that
     A(123) => 1
   then DEFER1 will expand making the next evaluation call it,
   but not the current one
     DEFER1(A)(123) => A (123)
   If we use EVAL1 this should be the same as A(123)
     EVAL1(DEFER1(A)(123)) => EVAL1(A (123)) => 1
*/
#define DEFER1(m) m EMPTY()
#define DEFER2(m) m EMPTY EMPTY()()
#define DEFER3(m) m EMPTY EMPTY EMPTY()()()
#define DEFER4(m) m EMPTY EMPTY EMPTY EMPTY()()()()

/* First parent at the end turns _RECURSE in RECURSE and the
   last paren cause it to be evaluated. No magic here. Or it
   might be, but you can figure out how it works. We must use
   _RECURSE instead of RECURSE to avoid the token being "painted
   blue" meaning the preprocesser will make it impossible to be
   expanded since it is the same name as the original macro.
*/
#define RECURSE() I am recursive, look: DEFER1(_RECURSE) () ()
#define _RECURSE() RECURSE
/* Example since this may be difficult to get:
   EVAL1(RECURSE()) =>
   EVAL1(I am recursive, look: DEFER1(_RECURSE) () ()) =>
   EVAL1(I am recursive, look: _RECURSE () ()) =>
     -- First evaluation ends here, eval runs the next --
   I am recursive, look: RECURSE () =>
   I am recursive, look: I am recursive, look: DEFER1(_RECURSE) () () =>
   I am recursive, look: I am recursive, look: _RECURSE () ()
*/

#define HAS_ARGS(...) BOOL(FIRST(_END_OF_ARGUMENTS_ __VA_ARGS__)())
#define _END_OF_ARGUMENTS_() 0

/* When HAS_ARGS is given no arguments:
     HAS_ARGS() =>
     BOOL(FIRST(_END_OF_ARGUMENTS_)()) =>
     BOOL(_END_OF_ARGUMENTS_ ()) =>
     BOOL(0) =>
     0
   When HAS_ARGS is given arguments:
     HAS_ARGS(some, arguments) =>
     BOOL(FIRST(_END_OF_ARGUMENTS_ some, arguments)()) =>
     BOOL(_END_OF_ARGUMENTS_ some ()) =>
     1
*/

/* Must use DEFER2 because DEFER1 is an arguments to
   a macro so it gets evaluated while still in scope
   causing the token to become painted blue. DEFER2
   puts this off until after it is no longer in map
   so it can be evaluated by another go of EVAL.
*/
#define MAP(m, first, ...)			\
	m(first)				\
	IF_ELSE(HAS_ARGS(__VA_ARGS__))		\
	  (DEFER2(_MAP) () (m, __VA_ARGS__))	\
	  ()
#define _MAP() MAP

#define GREET(x) Hello, x!
EVAL8(MAP(GREET, world, the ilk, macro abusers, and Happy Halloween))
