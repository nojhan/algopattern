#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <cmath>
#include <map>
#include <queue>
#include <cassert>
#include <functional>

#include "code.h"

namespace neighbors {

    using Neighborhood = std::function< neighbors_t(const point_t&) >;

    template<typename F, typename...Fargs>
    Neighborhood make(F f, const point_t& pmin, const point_t& pmax, double grid_step, Fargs...args)
    {
        return [f,pmin,pmax,grid_step,args...]
            (const point_t& p)
            ->neighbors_t
            { return f(p, pmin, pmax, grid_step, args...); };
    }

    neighbors_t quad_grid(const point_t& p, const point_t& pmin, const point_t& pmax, double grid_step)
    {
        // neighborhood (list) of relative locations (vector)
        // Should be given in [clockwise] order.
        std::vector<point_t> directions{{1,0},{0,-1},{-1,0},{0,1}};
        return neighbors_grid(p,grid_step,pmin,pmax,directions);
    }

    neighbors_t octo_grid(const point_t& p, const point_t& pmin, const point_t& pmax, double grid_step)
    {
        std::vector<point_t> directions{{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1},{1,1}};
        return neighbors_grid(p,grid_step,pmin,pmax,directions);
    }
}

namespace transit {

    using HopfLax = std::function< double(const point_t &, const neighbors_t  , const costs_t &) >;

    template<typename F, typename...Fargs>
    HopfLax make(F f, Fargs...args)
    {
        return [f,args...]
            (const point_t & p, const neighbors_t neighbors, const costs_t & costs)
            ->double
            { return f(p, neighbors, costs, args...); };
    }

    double on_edge(const point_t & p, const neighbors_t & neighbors, const costs_t & costs)
    {
        return transit_on_edge(p,neighbors,costs);
    }

    double in_simplex(const point_t & p, const neighbors_t & neighbors, const costs_t & costs, double epsilon)
    {
        return transit_in_simplex(p,neighbors,costs,epsilon);
    }

}

costs_t algo(neighbors::Neighborhood neighbors, transit::HopfLax transit, point_t seed, unsigned int iterations)
{
    return algo_run(seed, iterations, neighbors, transit);
}


int main()
{
    point_t seed; x(seed)= 0;y(seed)= 0;
    point_t pmin; x(pmin)=-5;y(pmin)=-5;
    point_t pmax; x(pmax)=15;y(pmax)=15;
        assert(x(seed) <= x(pmax));
        assert(x(seed) >= x(pmin));
        assert(y(seed) <= y(pmax));
        assert(y(seed) >= y(pmin));
    double step = 1;
    unsigned int maxit=300;
    double eps = 1/100.0;

    auto four  = neighbors::make(neighbors::quad_grid, pmin, pmax, step);
    auto eight = neighbors::make(neighbors::octo_grid, pmin, pmax, step);

    auto graph = transit::make(transit::on_edge);
    auto mesh  = transit::make(transit::in_simplex, eps);

    std::cout << "Dijkstra, 4 neighbors" << std::endl;
    costs_t cd4 = algo(four, graph, seed, maxit);
    std::cout << std::endl;
    grid_print(cd4, pmin, pmax, step);

    std::cout << "Fast marching, 4 neighbors" << std::endl;
    costs_t cfm4 = algo(four, mesh, seed, maxit);
    std::cout << std::endl;
    grid_print(cfm4, pmin, pmax, step);

    std::cout << "Dijkstra, 8 neighbors" << std::endl;
    costs_t cd8 = algo(eight, graph, seed, maxit);
    std::cout << std::endl;
    grid_print(cd8, pmin, pmax, step);

    std::cout << "Fast marching, 8 neighbors" << std::endl;
    costs_t cfm8 = algo(eight, mesh, seed, maxit);
    std::cout << std::endl;
    grid_print(cfm8, pmin, pmax, step);
}
