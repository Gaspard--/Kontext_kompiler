# Kontext Quick Spec

## 1) Expression parsing

### a) Basic syntaxe types

- value prefix // like 1, 2, 3
- unary prefix // like ! ~ + -
- unary postfix // like + - /

`unary prefix` or `postfix` can be applied to a value.

The result is a `value prefix` or a `unary prefix`.

Note that `postfix` and `unary postfix` is used interchangibly in this document.

### b) Suffix or prefix rules:

An expresions has to start with a `unary prefix` or `value prefix`

### c) The Parsing explained

Expression is processed from left to right.

The first symbol is looked up as a `value` or `unary prefix`.

(a suffix would cause a syntaxe error)

#### I: The current symbol is a `unary prefix`

We advance to the next symbol.

(This continues as long as we only encounter `unary prefix`es)

#### II: the current symbol is a `value`

When a value is found or computed, the next symbol is looked up as a suffix:

- If there is a `unary prefix` before  value AND no `unary suffix`  after the value.

  The `unary prefix` is applied on the value and the result is returned.

- Else if there is no `unary prefix` before the value and a `unary suffix` after the value:

  The `unary suffix` is applied on the value and the result is returned.

- Else if there is a `unary prefix` before the value and a `unary suffix` after the value:
  
  The prefix or the suffix is decided based on the best match through overloading. In case of same match cost, an error is raised. (See [Overloading](#c-overload-resolution))

- Otherwise if no symbol is left in the expression:
  
  The value is returned.

- Otherwise:

  A syntax error is raised.

If a `unary suffix` or `unary prefix` is applied, it is destroyed and no longer taken in consideration (as if it were erased from the expression).

The returned type replaces the value, and is now the current symbol.

Ex:
```c
1 + 2 * 3
(1+) 2 * 3 // no unary prefix present
(1+) (2*) 3 // overloading chooses * over +
(1+) 6 // no unary suffix present
7 // no unary suffix present
```

## 3) Types, Interfaces and Properties

### a) Brief exposition

A prmitive is a builtin compiler type.

A struct is the underlying data, it's an agregation of primitives andbo structs.

A property is an aditional tag that can be obtained by a variable through [promotion](#b-promotion).

A type is a struct or primitive with a list of properties.

### b) Promotion

A type may be promoted to a property if it matches that properties criterias.

To a promotion is associated a specific cost.
Multiple promotions can be chained, they can be considered as a single promotion of which the cost is the sum of the cost of all promotions.
If mutliple "paths" to promotion are possible, the cheapest one is taken into account.

### c) Overload resolution

A unary can specify several types and property combinations it can accept.

Overload resolution occurs when a `unary` is applied. If several are competing (unary prefix and postfix) both of the `unary`'s possiblillities are taken into account.

If an overload fits except for a certain amount of properties, the cost of pormoting to these properties dertimins the cost of the overload, the cheapest overload is then chosen.
