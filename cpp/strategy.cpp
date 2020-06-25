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
    /** A neighborhood returns a sequence of points "around" a given point. */
    class Neighborhood
    {
        public:
            Neighborhood( const point_t & pmin_, const point_t & pmax_) : pmin(pmin_), pmax(pmax_) {}
            neighbors_t operator()(const point_t & p) {return call(p);}
        protected:
            const point_t & pmin, pmax;
            virtual neighbors_t call(const point_t & p) = 0;
    };

    /** The four closest orthogonal neighbors on a square grid. */
    class quad_grid : public Neighborhood
    {
        public:
            quad_grid(double grid_step_, const point_t & pmin, const point_t & pmax) : Neighborhood(pmin,pmax), grid_step(grid_step_) {}
        protected:
            double grid_step;
            virtual neighbors_t call(const point_t & p)
            {
                // neighborhood (list) of relative locations (vector)
                // Should be given in [clockwise] order.
                std::vector<point_t> directions{{1,0},{0,-1},{-1,0},{0,1}};
                return neighbors_grid(p,grid_step,pmin,pmax,directions);
            }
    };

    /** The eights closests neighbors on a square grid.

        The four orthogonal neighbors + the four diagonal ones. */
    class octo_grid : public Neighborhood
    {
        public:
            octo_grid(double grid_step_, const point_t & pmin, const point_t & pmax) : Neighborhood(pmin,pmax),  grid_step(grid_step_) {}
        protected:
            double grid_step;
            virtual neighbors_t call(const point_t & p)
            {
                std::vector<point_t> directions{{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1},{1,1}};
                return neighbors_grid(p,grid_step,pmin,pmax,directions);
            }
    };
}

namespace transit {
    /** The Hopf-Lax operator computes the minimal transition from/to a given point to/from its boundary.

        The boundary of the point is discretized and given a a sequence of points. */
    class HopfLax
    {
        public:
            double operator()(const point_t & p, const neighbors_t & neighbors, const costs_t & costs) {return call(p,neighbors,costs);}
        protected:
            virtual double call(const point_t & p, const neighbors_t & neighbors, const costs_t & costs) = 0;
    };

    /** Search the minimal transition on a boundary discretized as points. */
    class on_edge : public HopfLax
    {
        protected:
            virtual double call(const point_t & p, const neighbors_t & neighbors, const costs_t & costs)
            {
                return transit_on_edge(p,neighbors,costs);
            }
    };

    /** Search the minimal transition on a boundary discretized as segments. */
    class in_simplex : public HopfLax
    {
        public:
            in_simplex( double epsilon ) : eps(epsilon) {assert(0 < epsilon and epsilon < 1);}
        protected:
            double eps;
            virtual double call(const point_t & p, const neighbors_t & neighbors, const costs_t & costs)
            {
                return transit_in_simplex(p,neighbors,costs,eps);
            }
    };
}

/** An algorithm is a combination of a neighborhood and an Hopf-Lax operator. */
class algo
{
    protected:
        neighbors::Neighborhood & neighbors;
        transit::HopfLax & transit;
    public:
        algo(neighbors::Neighborhood & neighbors_, transit::HopfLax & hl) : neighbors(neighbors_), transit(hl) {}
        virtual costs_t operator()(point_t seed, unsigned int iterations)
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

    // Instanciate 2 different functors for each function of the algorithm.
    neighbors::quad_grid four (step, pmin, pmax);
    neighbors::octo_grid eight(step, pmin, pmax);

    transit::on_edge graph;
    transit::in_simplex mesh(eps);

    // Instanciate the 2*2 possible algo.

    // Instanciate an algo propagating the front across a 4-neighborhood, on edges.
    // That is, a Dijkstra's algorithm.
    algo dijkstra4(four,graph);

    // Run and print.
    std::cout << "Dijkstra, 4 neighbors" << std::endl;
    costs_t cd4 = dijkstra4(seed, maxit);
    std::cout << std::endl;
    grid_print(cd4, pmin, pmax, step);

    algo fast_marching4(four,mesh);
    std::cout << "Fast marching, 4 neighbors" << std::endl;
    costs_t cfm4 = fast_marching4(seed, maxit);
    std::cout << std::endl;
    grid_print(cfm4, pmin, pmax, step);

    algo dijkstra8(eight,graph);
    std::cout << "Dijkstra, 8 neighbors" << std::endl;
    costs_t cd8 = dijkstra8(seed, maxit);
    std::cout << std::endl;
    grid_print(cd8, pmin, pmax, step);

    algo fast_marching8(eight,mesh);
    std::cout << "Fast marching, 8 neighbors" << std::endl;
    costs_t cfm8 = fast_marching8(seed, maxit);
    std::cout << std::endl;
    grid_print(cfm8, pmin, pmax, step);
}
