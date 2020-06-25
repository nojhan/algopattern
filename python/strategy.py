import abc
import code


class neighbors:
    class Neighborhood(object, metaclass=abc.ABCMeta):
        def __init__(self, pmin, pmax):
            self.pmin = pmin
            self.pmax = pmax

        @abc.abstractmethod
        def __call__(self, p):
            raise NotImplementedError


    class quad_grid(Neighborhood):
        def __init__(self, grid_step, pmin, pmax):
            super().__init__(pmin, pmax)
            self.grid_step = grid_step

        def __call__(self, p):
            directions = ((1,0),(0,-1),(-1,0),(0,1))
            return code.neighbors_grid(p, self.grid_step, pmin, pmax, directions)


    class octo_grid(Neighborhood):
        def __init__(self, grid_step, pmin, pmax):
            super().__init__(pmin, pmax)
            self.grid_step = grid_step

        def __call__(self, p):
            directions = ((1,0),(1,-1),(0,-1),(-1,-1),(-1,0),(-1,1),(0,1),(1,1))
            return code.neighbors_grid(p, self.grid_step, pmin, pmax, directions)


class transit:
    class HopfLax(object, metaclass=abc.ABCMeta):

        @abc.abstractmethod
        def __call__(self, p, neighbors, costs):
            raise NotImplementedError


    class on_edge(HopfLax):
        def __call__(self, p, neighbors, costs):
            return code.transit_on_edge(p, neighbors, costs)


    class in_simplex(HopfLax):
        def __init__(self, epsilon):
            self.eps = epsilon

        def __call__(self, p, neighbors, costs):
            return code.transit_in_simplex(p, neighbors, costs, self.eps)


class algo(object):
    def __init__(self, neighbors, transit):
        self.neighbors = neighbors
        self.transit = transit

    def __call__(self, seed, iterations):
        return code.algo_run( seed, iterations, self.neighbors, self.transit)


if __name__ == "__main__":

    seed = (0,0)
    pmin = (-5,-5)
    pmax = (15,15)
    step = 1
    maxit = 300
    eps = 1/100

    four  = neighbors.quad_grid(step, pmin, pmax)
    eight = neighbors.octo_grid(step, pmin, pmax)

    graph = transit.on_edge()
    mesh  = transit.in_simplex(epsilon=eps)

    dijkstra4 = algo(four, graph)
    print("Dijkstra, 4 neighbors")
    cd4 = dijkstra4(seed, maxit)
    code.grid_print(cd4, pmin, pmax, step)

    fast_marching4 = algo(four, mesh)
    print("Fast marching, 4 neighbors")
    cfm4 = fast_marching4(seed, maxit)
    code.grid_print(cfm4, pmin, pmax, step)

    dijkstra8 = algo(eight, graph)
    print("Dijkstra, 8 neighbors")
    cd8 = dijkstra8(seed, maxit)
    code.grid_print(cd8, pmin, pmax, step)

    fast_marching8 = algo(eight, mesh)
    print("Fast marching, 8 neighbors")
    cfm8 = fast_marching8(seed, maxit)
    code.grid_print(cfm8, pmin, pmax, step)


