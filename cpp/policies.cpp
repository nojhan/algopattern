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
#include <ratio>

#include "code.h"

namespace neighbors {

    template<int STEP, int PMIN_X, int PMIN_Y, int PMAX_X, int PMAX_Y>
    class quad_grid
    {
        protected:
            const double grid_step = STEP;
            const point_t pmin = {PMIN_X, PMIN_Y};
            const point_t pmax = {PMAX_X, PMAX_Y};

            neighbors_t neighbors(const point_t & p)
            {
                // neighborhood (list) of relative locations (vector)
                // Should be given in [clockwise] order.
                std::vector<point_t> directions{{1,0},{0,-1},{-1,0},{0,1}};
                return neighbors_grid(p,grid_step,pmin,pmax,directions);
            }
    };

    template<int STEP, int PMIN_X, int PMIN_Y, int PMAX_X, int PMAX_Y>
    class octo_grid
    {
        protected:
            const double grid_step = STEP;
            const point_t pmin = {PMIN_X, PMIN_Y};
            const point_t pmax = {PMAX_X, PMAX_Y};

            neighbors_t neighbors(const point_t & p)
            {
                std::vector<point_t> directions{{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1},{1,1}};
                return neighbors_grid(p,grid_step,pmin,pmax,directions);
            }
    };
}

namespace transit {

    class on_edge
    {
        protected:
            double transit(const point_t & p, const neighbors_t & neighbors, const costs_t & costs)
            {
                return transit_on_edge(p,neighbors,costs);
            }
    };

    template<typename EPS>
    class in_simplex
    {
        protected:
            const double eps;
            in_simplex() : eps(static_cast<double>(EPS::num)/static_cast<double>(EPS::den)) {}

            double transit(const point_t & p, const neighbors_t & neighbors, const costs_t & costs)
            {
                return transit_in_simplex(p,neighbors,costs, this->eps);
            }
    };
}


template<typename NeighborsPolicy, typename TransitPolicy>
class algo : private NeighborsPolicy, private TransitPolicy
{

    public:
        using NeighborsPolicy::neighbors;
        using TransitPolicy::transit;

        costs_t operator()(point_t seed, unsigned int iterations)
        {
            costs_t costs;

            // Make a priority queue of considered nodes.
            auto compare = [&costs](const point_t& lhs, const point_t& rhs) { return costs[lhs] > costs[rhs]; };
            front_t front(compare);

            // Start the front from the seed
            costs[seed] = 0;
            front.push(seed);

            unsigned int i=0;
            while(i++ < iterations and not front.empty()) {
                std::cout << "\r" << i << "/" << iterations;

                // Accept the node with the min cost and update neighbors.
                // Accept the considered node with the min cost.
                point_t accepted = front.top(); front.pop();
                // Consider neighbors of the accepted node.
                neighbors_t around = neighbors(accepted);
                assert(around.size()>0);
                for( auto n : around ) {
                    // If no cost has been computed (i.e. the node is "open").
                    if( not has_cost(n, costs)) {
                        // Compute costs.
                        costs[n] = transit(n, neighbors(n), costs);
                        front.push(n);
                    }
                }
            }
            return costs;
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

    using four  = neighbors::quad_grid</*seed*/1, /*x(pmin)*/-5, /*y(pmin)*/-5, /*x(pmax)*/15, /*y(pmax)*/15>;
    using eight = neighbors::octo_grid<1, -5, -5, 15, 15>;

    using graph = transit::on_edge;
    using mesh  = transit::in_simplex</*epsilon=*/std::ratio<1,100>>;

    algo<four,graph> dijkstra4;
    std::cout << "Dijkstra, 4 neighbors" << std::endl;
    costs_t cd4 = dijkstra4(seed, maxit);
    std::cout << std::endl;
    grid_print(cd4, pmin, pmax, step);

    algo<four,mesh> fast_marching4;
    std::cout << "Fast marching, 4 neighbors" << std::endl;
    costs_t cfm4 = fast_marching4(seed, maxit);
    std::cout << std::endl;
    grid_print(cfm4, pmin, pmax, step);

    algo<eight,graph> dijkstra8;
    std::cout << "Dijkstra, 8 neighbors" << std::endl;
    costs_t cd8 = dijkstra8(seed, maxit);
    std::cout << std::endl;
    grid_print(cd8, pmin, pmax, step);

    algo<eight,mesh> fast_marching8;
    std::cout << "Fast marching, 8 neighbors" << std::endl;
    costs_t cfm8 = fast_marching8(seed, maxit);
    std::cout << std::endl;
    grid_print(cfm8, pmin, pmax, step);
}
