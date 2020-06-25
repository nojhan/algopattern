import code

def neighbors(op, pmin, pmax, grid_step, **kwargs):
    def f(p):
        return op(p, pmin, pmax, grid_step, **kwargs)
    return f

def quad_grid(p, pmin, pmax, grid_step):
    directions = ((1,0),(0,-1),(-1,0),(0,1))
    return code.neighbors_grid(p, grid_step, pmin, pmax, directions)

def octo_grid(p, pmin, pmax, grid_step):
    directions = ((1,0),(1,-1),(0,-1),(-1,-1),(-1,0),(-1,1),(0,1),(1,1))
    return code.neighbors_grid(p, grid_step, pmin, pmax, directions)


def transit(op, **kwargs):
    def f(p, neighbors, costs):
        return op(p, neighbors, costs, **kwargs)
    return f

def on_edge(p, neighbors, costs):
    return code.transit_on_edge(p, neighbors, costs)

def in_simplex(p, neighbors, costs, epsilon):
    return code.transit_in_simplex(p, neighbors, costs, epsilon)


def algo(neighbors, transit, seed, iterations):
    return code.algo_run( seed, iterations, neighbors, transit)


if __name__ == "__main__":

    seed = (0,0)
    pmin = (-5,-5)
    pmax = (15,15)
    step = 1
    maxit = 300
    eps = 1/100

    four  = neighbors(quad_grid, pmin, pmax, step)
    eight = neighbors(octo_grid, pmin, pmax, step)

    graph = transit(on_edge)
    mesh  = transit(in_simplex, epsilon=eps)

    print("Dijkstra, 4 neighbors")
    cd4 = algo(four, graph, seed, maxit)
    code.grid_print(cd4, pmin, pmax, step)

    print("Fast marching, 4 neighbors")
    cfm4 = algo(four, mesh, seed, maxit)
    code.grid_print(cfm4, pmin, pmax, step)

    print("Dijkstra, 8 neighbors")
    cd8 = algo(eight, graph, seed, maxit)
    code.grid_print(cd8, pmin, pmax, step)

    print("Fast marching, 8 neighbors")
    cfm8 = algo(eight, mesh, seed, maxit)
    code.grid_print(cfm8, pmin, pmax, step)


