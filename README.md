Educational demonstration of several design patterns which may be useful in algorithmics, in several languages.

The algorithms are simple motion planning methods
and the patterns are centered on composition.
The code demonstrates how to *compose* either a Dijkstra algorithm,
either a fast-marching algorithm, with different neighborhoods for each.


Avalaible languages
===================

So far, the demo contains:
- Python:
    - a purely functional pattern (functions which returns parametrized closures),
    - the strategy pattern (composition of abstract interface),
- C++:
    - purely functional (functions which returns parametrized lambdas),
    - strategy (composition of abstract classes),
    - policies (function parameters as templated variables),
    - CRTP (Curiously Recurring Template Pattern).
- Java:
    - functional,
    - strategy,
    - CRTP.

The algorithm machinery itself is located within the `code.*` files
and is not of major interest,
look for the other files for the architecture.


Algorithmics
============

The Dijkstra algorithm is a well-known way to compute shortest paths on a graph.
Here, we use a simple regular discretization of a bounded plan as the
navigation domain (i.e. a grid).
The Dijkstra algorithm proceed by going on the edge from one node of the graph
to another *neighbor* node (in a specific order, which does not matter here).
As such, it needs to know what is the neighborhood of a node.
Here, we define two choices: either a set of four nodes, or a set of eight
nodes.

The Fast-Marching algorithm is a generalization of the Dijkstra's algorithm
for continuous space instead of a graph.
Instead of going from one node to another, it will traverse a triangle
(i.e. one of the 2D *simplex* of a partition of the domain space),
from one node to the opposite edge.
The difference between the two thus rely on how they define a *transit* on the domain.
This traversal is called the *Hopf-Lax operator* in the litterature.

Just like Dijkstra needs to know how many nodes are in the neighborhood,
Fast-Marching needs to know how many triangles are in the neighborhood.


Architecture
============

Hence, we have two *operator slots* for each algorithms:
- neighborhood (where to go?),
- Hopf-Lax operator (how to go there?).

For each slot, we have two options:
- neighborhood:
    - four neighbors,
    - eight neighbors;
- Hopf-Lax:
    - on the edge of a grpah,
    - across a triangle.

![A diagram showing plugs going into slots](https://raw.githubusercontent.com/nojhan/algopattern/master/algopattern_operators.svg)

At the end, by combining those options, we can make up for four different
algorithms:
- four neighbors + on edges (4-Dijkstra),
- eight neighbors + on edges (8-Dijkstra),
- four neighbors + in triangles (4-Fast-Marching),
- eight neighbors + in triangles (8-Fast-Marching).

Each of those algorithms may have different behavior/performances and be useful
in different cases.
It can however be handy to easily come up with the one you want,
especially if you have many more operators to manage.

