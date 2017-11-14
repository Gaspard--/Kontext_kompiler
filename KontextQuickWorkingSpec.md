# Kontext Quick Spec

## 1) Basic syntaxe types

- value prefix // like 1, 2, 3
- unary prefix // like ! ~ + -
- unary postfix // like + - /

`unary prefix` or `postfix` can be applied to a value.
The result is a `value prefix` or a `unary prefix`.
Note that `postfix` and `unary postfix` is used interchangibly in this document.

## 2) Suffix or prefix rules:

An expresions has to start with a `unary prefix` or `value prefix`

## 3) Expression parsing

### a) Quick overview

Expression is processed from left to right.
The first symbol is looked up as a `value` or `unary prefix`.

###b) `unary prefix`:

When found or computed, is put on the `unary prefix stack`.

### c) Values:

When a value is found or computed, the next symbol is looked up as a suffix:
- If there is a `unary prefix` before  value AND no `unary suffix`  after the value.
  -> The `unary prefix` is applied on the value and the result is returned.
- Else if there is no `unary prefix` before the value and a `unary suffix` after the value:
  -> The `unary suffix` is applied on the value and the result is returned.
- Else if there is a `unary prefix` before the value and a `unary suffix` after the value:
  -> The prefix or the suffix is decided based on the best match through overloading. In case of same match cost, an error is raised. (See overloading)
- Otherwise if no symbol is left in the expression:
 -> The value is returned.
- Otherwise:
 -> A syntax error is raised.
If a `unary suffix` or `unary prefix` is applied, it is destroyed and no longer taken in consideration (as if it were erased from the expression).

Ex:
```c
1 + 2 * 3
(1+) 2 * 3 // no unary prefix present
(1+) (2*) 3 // overloading chooses * over +
(1+) 6 // no unary suffix present
7 // no unary suffix present
```

## 3) Types, Interfaces and Properties

### a) brief exposition

A type is the underlying data, the reality.
A property is an aditional tag that can be obtained by a variable either through promotion or explicit casting.

### b) Promotion

A type may be promoted to a property if it matches that properties criterias.
To a promotion is associated a specific cost. Multiple promotions can be chained, they can be considered as a single promotion of which the cost is the sum of the cost of all promotions. If mutliple "paths" to promotion are possible, the cheapest one is taken into account.

### c) Overload resolution

A unary can specify several types and property combinations it can accept.
Overload resolution occurs when a `unary` is applied. If several are competing (unary suffix and prefix) both of the `unary`'s possiblillities are taken into account.
If an overload fits except for a certain amount of properties, the cost of pormoting to these properties dertimins the cost of the overload, the cheapest overload is then chosen.

#// BELOW IS OLD STUFF, READ AT YOUR OWN RISK //
4) Operator overloading

When defining an operator, it's parameter type will determine which variables will be able go be passed to it.
In the case where multiple operators are defined, the last defined operator which can be applied to the value will be selected.

5) Reflecting variables

When using the preceding syntax, several problem arise:
- How does one handle variable declaration ?
- How does one access suffix and prefix operators ?

Solution: reflecting variables.
The '@' prefix operator (by default, it is of course redefinable, copiable etc.), will applied to a type constraint, create a reflecting variable, allowing operator inspection. An operator aplied to a reflecting will return its definition for that variable type.

The '@' suffix operator (see above comment) will copy a definition to an operator.
It returns a prefix which will copy the given operator definition to the operator which was given to the @ suffix.

Examples:

@Int +
Return the definition of suffix operator '+' for the type constraint 'Int'.
"-" @ @Value +
will copy the definition of '+' to '-' for any type.

5) Names

Prefixes that expect names (such as classes) take a special name type. When such a prefix is found, the next symbol is considered a name.
Names are constrained strings (All charcaters from a name must be alphanumeric without a starting number, or operators (all other none whitespace printable characters))
Names can be returned, and manipulated (this allows defining '+' from '+=', and encapsulating that process in a function to be able to generate '@' from '@=' for any suffix '@=').

/-------------------------------------------\
| C-style operator precedence (total order) |
\-------------------------------------------/

1) The precedence partial order

'n@' denotes the prefix returned by applying the suffix '@'.
'@n' denotes the prefix returned by applying the prefix '@'.

prefix n., n->, @, n;
suffix ., -> [, (, --, ++
prefix + - ! ~ ++ -- * & sizeof (n)
prefix n*, n/, n%
suffix *, /, %
prefix n+, n-
suffix +, -
prefix n>>, n<<
suffix >>, <<
prefix n!=, n==
suffix !=, ==
prefix n<, n<=, n>, n>=
suffix <, <=, >, >=
prefix n&
suffix &
prefix n^
suffix ^
prefix n&&
suffix &&
prefix n||
suffix ||
prefix n:
suffix :
prefix n?
suffix ?
prefix n=, n+= ...
suffix =, += ...
suffix ","
prefix "n,"
suffix ), ]
prefix n(, n[, (,
suffix ;

2) Notes

'(' and 'n(' would expect the return type of ')'
same for 'n[' and ']'
';' being the lowest priority suffix assures that the prefix stack is empty.
'n;' would be a noop prefix.
The position of 'n;' in the precedence rules does not really matter. However having it on top creates minimal stack usage (otherwise it would linger). At the bottom it could even cause a memory leak.
In fact this is true for any prefix, the use case of having a lowest prefix (and not suffix) is questionable.

3) Examples

"a = b * c;"

a
-> returns value
a =
-> returns prefix (a=)
a = b
-> returns value (prefix a= is on stack)
a = b *
-> returns prefix
a = b * c
-> returns value (a=, and b* are on the stack)
a = b * c;
-> returns noop prefix: b* is applied on c, and a= is aplied on b*c. No prefix is then left. Therefore  the ';' suffix is applied, returning a noop prefix.

"(a;b;"

(
-> prefix
(a
-> value
(a;
-> prefix ('(' is on stack)
(a;b
-> value.
(a;b;
'(' will attempt to apply to 'b' (higher precedence than ';'), which will not work ('(' expects the return type of 'n)')

/---------------\
| Builtin types |
\---------------/

1) Non-undefinable symbols

These are special symbols that can't be redefined. (These are the only context independent symbols)

a) __default_builtin_definitions__

Upon creation, this hides all defined types, and defines the default compiler types.
Upon destruction, it removes the default compiler types it defined, and it redefins all symbols it has hidden (including the compiler symbols in the old context). Note that none compiler symbols aren't hidden.

These symbols are meant to facilitate writting seperate modules.

2) '{' prefix and '}' suffix

'{' places a marker on the stack
'}' removes and destroys all variables on the stack until the last mark

Note that '}' is overloaded (see below (WIP))

3) 'class', 'operator' prefix operators

a) 'class' prefix operator

'class' is a name prefix that expects a name and creates a type of that given name.
If name already exists it obeys overloading rules.
The returned value is a type definition, the (overloaded) suffix '=' accepts this type and returns a prefix operator that accepts a type definition. (This allows copying and assigning type defintions).

b) 'operator' prefix operator

This entry is BAD (to remake)

'operator' is a name prefix that expects a name and creates an operator of that given name.
An operator can be converted by the 'suffix' and 'prefix' keywords into a suffix or prefix operator.


4) The string type.

This entry is WIP.
Goals are:
- Extend '"' to an operator type.
- Allow easy redefinition of '"' (currently impossible)

This type is composed of an array of characters and a size.
'"' is a special prefix that will put the contents (including whitespace) between it and suffix '"' in a string.

/--------------------\
| Stuff (don't read) |
\--------------------/

????

define "symbolName" = {suffix, prefix}  ParamType "paramName" block ?

ex:

class Mod10Int
{
  Int value;
  value < 10;
};

prefix - Mod10Int n -> Mod10Int(10 - n.value);
prefix + Mod10Int n -> n;

implicit casts? -> yes
variable overloading? -> yes, use proxy type
