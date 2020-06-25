import sys
import math
from queue import PriorityQueue


def x(p):
    return p[0]


def y(p):
    return p[1]


def distance(u,v):
    return math.sqrt( (x(u)-x(v))*(x(u)-x(v)) + (y(u)-y(v))*(y(u)-y(v)) )


def tour(lst):
    """Yield a sequence of consecutive pairs of items across a given container."""
    assert(len(lst)>=2)
    # consecutive pairs in lst  + last-to-first element
    for a,b in zip(lst, lst[1:] + [lst[0]]):
        yield (a,b)


# This is a very bad way to implement frange,
# you should use nympy.linspace in real code…
def frange(x, y, s):
  while x < y:
    yield x
    x += s


def neighbors_grid(p, grid_step, pmin, pmax, directions):
    """
    Compute coordinates of the neighbors of the given point, according to grid parameters and directions.

    Directions are supposed to be given in a specific order
    (in this project, the transit_in_simplex functions will necessitate clockwise order).

    Args:
        point:     The considered point.
        grid_step: Length of an orthogonal edge of the grid.
        pmin:      Corner point of the grid, with minimal coordinates.
        pmax:      Corner point of the grid, with maximal coordinates.

    Returns:
        A sequence of neighbors points.
    """
    neighbors = []
    for d in directions:
        nx = x(p)+x(d)*grid_step
        ny = y(p)+y(d)*grid_step
        n = (nx, ny)
        if x(pmin) <= x(n) <= x(pmax) and y(pmin) <= y(n) <= y(pmax):
            neighbors.append(n)
    assert(len(neighbors) >= 2)
    return neighbors


def transit_on_edge(point, neighbors, costs):
    """
    Find the transit of minimal cost among the given edges.

    Edges are given as the considered point and the sequence of neighbors points. 
    """
    mincost = float("inf")
    for n in neighbors:
        # Do not compute transition toward points without a cost
        # (i.e. Supposedly, only toward the front).
        if n in costs:
            # Cost of the transition from/to p from/to n.
            c = costs[n] + distance(point,n)
            if c < mincost:
                mincost = c
    # Should be near the front (and thus have found a transit).
    assert(mincost != float("inf"))
    return mincost


def transit_in_simplex(p, neighbors, costs, eps):
    mincost = float("inf")

    # Special case: having a single point with a cost in the neighborhood.
    # e.g. the seed.
    # This would make an empty tour and thus needs a special case.
    with_costs = [n for n in neighbors if n in costs]
    if len(with_costs) == 1:
        # There is only one possible transition.
        i = with_costs[0]
        mincost = costs[i] + distance(p,i)

    else:
        for edge in tour(neighbors):
            pj, pk = edge

            # Do not compute transition toward points without a cost
            # (i.e. only toward the front).
            if pj in costs and pk in costs:
                # Cost of the transition from/to p from/to edge e.
                # This is the simplest way to minimize the transit, even if not the most efficient.
                for z in frange(0,1,eps):
                    xj,yj = pj
                    xk,yk = pk
                    zx = z*xj + (1-z)*xk
                    zy = z*yj + (1-z)*yk
                    n = (zx,zy)

                    # Linear interpolation of costs.
                    c = z*costs[pj] + (1-z)*costs[pk] + distance(p,n)
                    if c < mincost:
                        mincost = c

            # If the front is reached on a single point.
            elif pj     in costs and pk not in costs:
                c = costs[pj] + distance(p,pj)
                if c < mincost:
                    mincost = c

            elif pj not in costs and pk     in costs:
                c = costs[pk] + distance(p,pk)
                if c < mincost:
                    mincost = c

    # Should be near the front (and thus have found a transit).
    assert(mincost != float("inf"))
    return mincost


def algo_run(seed, iterations, neighbors, transit_func):
    """
    Propagate the front from the given seed, during the given number of iterations.

    Iteratively accept points of minimal costs (see the transit function) in a neighborhood (see the neighbors function).

    Args:
        seed:       The point with NULL cost.
        iterations: The maximum number of iterations. If ommitted, the costs of all the points of the grid will be computed.

    Returns:
        The costs map: <points coordinates> => <cost>
    """
    costs = {}

    # Make a priority queue of considered nodes.
    class Prioritize(PriorityQueue):
        def __init__(self, keyf):
            super().__init__()
            self.keyf = keyf

        def push(self, i):
            # Put (priority,item)
            super().put( (self.keyf(i), i) )

        def pop(self):
            # Return and remove
            return super().get()[1]

    on_cost = lambda n: costs[n]
    front = Prioritize(on_cost)

    # Start the front from the seed
    costs[seed] = 0
    front.push(seed)
    i=0
    while i < iterations and not front.empty():
        print("{} iterations".format(i), end='\r')
        # Accept the node with the min cost and update neighbors.
        # Accept the considered node with the min cost.
        accepted = front.pop()
        # Consider neighbors of the accepted node.
        around = neighbors(accepted)
        assert(len(around)>0)
        for n in around:
            # If no cost has been computed (i.e. the node is "open").
            if n not in costs:
                # Compute costs.
                costs[n] = transit_func(n, neighbors(n), costs)
                front.push(n)

        i += 1

    return costs


def grid_print(grid, pmin, pmax, step,
    out = sys.stdout, sep = " ", end = "\n",
    width = 5, fill = ' ', prec = 3):
    """Pretty print a cost map"""

    print("   x:", end="", file=out)
    for px in range(x(pmin),x(pmax),step):
        print("{sep}{:{fill}>{width}}".format(px,sep=sep,fill=fill,width=width,prec=prec), end="", file=out)

    print(end=end)
    print("  y",end=end)
    for py in range(y(pmax),y(pmin),-step):
        print("{:{fill}>{width}}:".format(py,fill=fill,width=width,prec=prec), end="", file=out)
        for px in range(x(pmin),x(pmax),step):
            p=(px,py)
            if p in grid:
                print("{sep}{:{fill}>{width}.{prec}}".format(float(grid[p]),sep=sep,fill=fill,width=width,prec=prec), end="", file=out)
            else:
                print("{sep}{:{fill}>{width}}".format(".",sep=sep,fill=fill,width=width,prec=prec), end="", file=out)
        print(end=end)
    print(end=end)
