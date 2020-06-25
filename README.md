Educational demonstration of several design patterns which may be useful in algorithmics, in several languages.

The algorithms are simple motion planning methods
and the patterns are centered on composition.
The code demonstrates how to *compose* either a Dijkstra algorithm,
either a fast-marching algorithm, with different neighborhoods for each.

So far, the demo contains:
- Python:
    - a purely functional pattern (functions which returns parametrized closures),
    - the strategy pattern (composition of abstract interface),
- C++:
    - purely functional (functions which returns parametrized lambdas),
    - strategy (composition of abstract classes),
    - policies (function parameters as templated variables),
    - CRTP (Curiously Recurring Template Pattern)

The algorithm machinery itself is located within the `code.*` files
and is not of major interest,
look for the other files for the architecture.
