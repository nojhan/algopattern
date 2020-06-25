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
    template <class T> 
    struct Neighborhood
    {
        public:
            Neighborhood( const point_t & pmin_, const point_t & pmax_) : pmin(pmin_), pmax(pmax_) {}
            neighbors_t operator()(const point_t & p)
            {
                return static_cast<T*>(this)->call(p);
            }
        protected:
            const point_t & pmin, pmax;
    };

    class quad_grid : public Neighborhood<quad_grid>
    {
        public:
            quad_grid(double grid_step_, const point_t & pmin, const point_t & pmax) : Neighborhood<quad_grid>(pmin,pmax), grid_step(grid_step_) {}
            neighbors_t call(const point_t & p)
            {
                // neighborhood (list) of relative locations (vector)
                // Should be given in [clockwise] order.
                std::vector<point_t> directions{{1,0},{0,-1},{-1,0},{0,1}};
                return neighbors_grid(p,grid_step,pmin,pmax,directions);
            }
        protected:
            double grid_step;
    };

    class octo_grid : public Neighborhood<octo_grid>
    {
        public:
            octo_grid(double grid_step_, const point_t & pmin, const point_t & pmax) : Neighborhood<octo_grid>(pmin,pmax),  grid_step(grid_step_) {}
            neighbors_t call(const point_t & p)
            {
                std::vector<point_t> directions{{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1},{1,1}};
                return neighbors_grid(p,grid_step,pmin,pmax,directions);
            }
        protected:
            double grid_step;
    };
}

namespace transit {
    template <class T> 
    class HopfLax
    {
        public:
            double operator()(const point_t & p, const neighbors_t & neighbors, const costs_t & costs)
            {
                return static_cast<T*>(this)->call(p,neighbors,costs);
            }
    };

    class on_edge : public HopfLax<on_edge>
    {
        public:
            double call(const point_t & p, const neighbors_t & neighbors, const costs_t & costs)
            {
                return transit_on_edge(p,neighbors,costs);
            }
    };

    class in_simplex : public HopfLax<in_simplex>
    {
        public:
            in_simplex( double epsilon ) : eps(epsilon) {assert(0 < epsilon and epsilon < 1);}
            double call(const point_t & p, const neighbors_t & neighbors, const costs_t & costs)
            {
                return transit_in_simplex(p,neighbors,costs,eps);
            }
        protected:
            double eps;
    };
}


template<typename N, typename T>
class algo
{
    protected:
        N & neighbors;
        T & transit;
    public:
        algo(N & neighbors_, T & hl) : neighbors(neighbors_), transit(hl) {}
        costs_t operator()(point_t seed, unsigned int iterations)
        {
            return algo_run(seed, iterations, std::ref(this->neighbors), std::ref(this->transit));
        }
};


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

    neighbors::quad_grid four (step, pmin, pmax);
    neighbors::octo_grid eight(step, pmin, pmax);

    transit::on_edge graph;
    transit::in_simplex mesh(eps);

    algo<neighbors::quad_grid,transit::on_edge> dijkstra4(four,graph);
    std::cout << "Dijkstra, 4 neighbors" << std::endl;
    costs_t cd4 = dijkstra4(seed, maxit);
    std::cout << std::endl;
    grid_print(cd4, pmin, pmax, step);

    algo<neighbors::quad_grid,transit::in_simplex> fast_marching4(four,mesh);
    std::cout << "Fast marching, 4 neighbors" << std::endl;
    costs_t cfm4 = fast_marching4(seed, maxit);
    std::cout << std::endl;
    grid_print(cfm4, pmin, pmax, step);

    algo<neighbors::octo_grid,transit::on_edge> dijkstra8(eight,graph);
    std::cout << "Dijkstra, 8 neighbors" << std::endl;
    costs_t cd8 = dijkstra8(seed, maxit);
    std::cout << std::endl;
    grid_print(cd8, pmin, pmax, step);

    algo<neighbors::octo_grid,transit::in_simplex> fast_marching8(eight,mesh);
    std::cout << "Fast marching, 8 neighbors" << std::endl;
    costs_t cfm8 = fast_marching8(seed, maxit);
    std::cout << std::endl;
    grid_print(cfm8, pmin, pmax, step);
}
