#include <iterator>
#include <utility>
#include <iomanip>
#include <functional>

//! x, y.
using point_t = std::pair<double,double>;
//! Time to go to a given point.
using costs_t = std::map<point_t,double>;
//! Concept to compare points.
using comp_t = std::function< bool( const point_t lhs, const point_t rhs) >;
//! The priority queue that permits an efficient single pass over the domain.
using front_t = std::priority_queue<point_t,std::vector<point_t>,comp_t>;
//! Set of points "around".
using neighbors_t = std::vector<point_t>;

point_t make_point(double x, double y) {return std::make_pair(x,y);}
double& x(      point_t& p) {return p.first ;}
double& y(      point_t& p) {return p.second;}
double  x(const point_t& p) {return p.first ;}
double  y(const point_t& p) {return p.second;}
//! Euclidean distance.
double distance(point_t u, point_t v) {return sqrt( (x(u)-x(v))*(x(u)-x(v)) + (y(u)-y(v))*(y(u)-y(v)) );}

/** Test if a cost has already been computed for a given point.

    \param p     The considered point.
    \param costs The costs map in which to look for the given point.
  */
bool has_cost(point_t p, const costs_t & costs)
{
    // The map does not have the point as key,
    // or it have an infinite cost (not use at the moment).
    return costs.find(p) != costs.end() and costs.at(p) < std::numeric_limits<double>::infinity();
}


/** \defgroup Tour Tools to easily build a sequence of consecutive pairs of iterators across a given container.
  @{
 */

/** State iterator of a tour.

  An iterator that maintain its next as a separate member and ends with next = begin.
 */
template<class FwdIt>
class tour_iterator
{
    public:
        using ref = typename std::iterator_traits<FwdIt>::reference;
        using pair = std::pair<ref,ref>;

        tour_iterator(FwdIt first, FwdIt last, bool end=false)
            : _first(first), _cur(first), _next(first == last ? last : std::next(first)), _last(last),
            _tour_ended(first == last ? true : end)
        {}

        bool operator!=(const tour_iterator & other) const
        {
            return /*_cur != other._cur and*/ _next != other._next;
        }

        tour_iterator & operator++()
        {
            _cur++;

            if(not _tour_ended) {
                _next++;
                if( _next == _last ) { // Back loop.
                    _next = _first;
                    _tour_ended = true;
                }
            } else {
                _cur  = _last;
                _next = _last;
            }

            return *this;
        }

        pair operator*() const
        {
            return pair(*_cur, *_next);
        }

    private:
        const FwdIt _first;
        FwdIt _cur;
        FwdIt _next;
        const FwdIt _last;
        bool _tour_ended;
};

/** Range of a tour. */
template<class FwdIt>
class tour
{
    public:
        tour(const FwdIt first, const FwdIt last) : _first(first), _last(last) { }

        tour_iterator<FwdIt> begin() const
        {
            return tour_iterator<FwdIt>(_first, _last, false);
        }

        tour_iterator<FwdIt> end() const
        {
            return tour_iterator<FwdIt>(_last, _last, true);
        }

    private:
        const FwdIt _first;
        const FwdIt _last;
};

/** Make a tour from a container. */
template<class C>
auto make_tour(C& c) -> tour<decltype(begin(c))>
{
    return tour<decltype(begin(c))>(begin(c), end(c));
}
/** @} Tour */


/** \defgroup Algorithm The actual code necessary for all Dijkstra/Fast Marching variants.

    The algorithms operates on an IMPLICIT square grid defined by a rectangle and an orthogonal step size.
    They propagate a front of computed isotropic costs (stored in a [sparse] map), from a single seed.

  @{
*/

/** Compute coordinates of the neighbors of the given point, according to grid parameters and directions.

  Directions are supposed to be given in a specific order
  (in this project, the transit_in_simplex functions will necessitate clockwise order).

  \param p The considered point.
  \param grid_step Length of an orthogonal edge of the grid.
  \param pmin Corner point of the grid, with minimal coordinates.
  \param pmax Corner point of the grid, with maximal coordinates.
  \return A sequence of neighbors points.
 */
inline neighbors_t neighbors_grid(const point_t & p, double grid_step, const point_t & pmin, const point_t & pmax, std::vector<point_t> directions)
{
    neighbors_t neighbors;
    for( point_t d : directions) {
        point_t n;
        x(n) = x(p) + x(d) * grid_step;
        y(n) = y(p) + y(d) * grid_step;
        if( x(pmin) <= x(n) and x(n) <= x(pmax) and y(pmin) <= y(n) and y(n) <= y(pmax) ) {
            neighbors.push_back( n );
        }
    }
    assert(neighbors.size() >= 2);
    return neighbors;
}

/** Find the transit of minimal cost among the given edges.

    Edges are given as the considered point and the sequence of neighbors points. */
inline double transit_on_edge(const point_t & p, const neighbors_t & neighbors, const costs_t & costs)
{
    double mincost = std::numeric_limits<double>::infinity();
    for( auto n : neighbors ) {
        // Do not compute transition toward points without a cost
        // (i.e. Supposedly, only toward the front).
        if( has_cost(n, costs) ) {
            // Cost of the transition from/to p from/to n.
            double c = costs.at(n) + distance(p, n);
            if( c < mincost ) {
                mincost = c;
            }
        }
    }
    // Should be near the front (and thus have found a transit).
    assert(mincost != std::numeric_limits<double>::infinity());
    return mincost;
}

/** Find the transit in minimal cost within the given simplexes.

    That is, find the minimal distance between the given point
    and one of the edges formed by the sequence of pairs of neighbors.
    Neighbors should thus be given in clockwise order.
    The minimal transit is searched across 1/eps distances,
    regularly spaced on each edge. */
inline double transit_in_simplex(const point_t & p, const neighbors_t & neighbors, const costs_t & costs, double eps)
{
    double mincost = std::numeric_limits<double>::infinity();

    // Special case: having a single point with a cost in the neighborhood.
    // e.g. the seed.
    // This would make an empty tour and thus needs a special case.
    auto with_cost = [&costs] (const point_t& i) {return has_cost(i,costs);};
    if( std::count_if(begin(neighbors), end(neighbors), with_cost) == 1 ) {
        // There is only one possible transition.
        auto in = std::find_if( begin(neighbors), end(neighbors), with_cost);
        mincost = costs.at(*in) + distance(p,*in);

    } else {
        for( auto edge : make_tour(neighbors) ) {
            point_t pj = edge.first;
            point_t pk = edge.second;

            // Do not compute transition toward points without a cost
            // (i.e. only toward the front).
            if( has_cost(pj, costs) and has_cost(pk, costs) ) {
                // Cost of the transition from/to p from/to edge e.
                // This is the simplest way to minimize the transit, even if not the most efficient.
                for( double z=0; z<=1; z+=eps) {
                    double xj = x(pj); double yj = y(pj);
                    double xk = x(pk); double yk = y(pk);
                    double zx = ( z*xj + (1-z)*xk );
                    double zy = ( z*yj + (1-z)*yk );
                    point_t n; x(n) = zx, y(n)=zy;

                    // Linear interpolation of costs.
                    double c = z*costs.at(pj) + (1-z)*costs.at(pk) + distance(p, n);
                    if( c < mincost ) {
                        mincost = c;
                    }
                } // for z

                // If the front is reached on a single point.
            } else if(has_cost(pj, costs) and not has_cost(pk, costs)) {
                double c = costs.at(pj) + distance(p,pj);
                if( c < mincost ) {
                    mincost = c;
                }
            } else if(not has_cost(pj, costs) and has_cost(pk, costs)) {
                double c = costs.at(pk) + distance(p,pk);
                if( c < mincost ) {
                    mincost = c;
                }
            }
        } // for edge
    }

    // Should be near the front (and thus have found a transit).
    assert(mincost < std::numeric_limits<double>::infinity());
    return mincost;
}

/** Propagate the front from the given seed, during the given number of iterations.

  Iteratively accept points of minimal costs (see the transit function) in a neighborhood (see the neighbors function).

  \param seed The point with NULL cost.
  \param iterations The maximum number of iterations. If ommitted, the costs of all the points of the grid will be computed.
  \return The costs map: <points coordinates> => <cost>
*/
inline costs_t algo_run(
        point_t seed,
        unsigned int iterations,
        std::function< neighbors_t(const point_t&) > neighbors,
        std::function< double(const point_t &, const neighbors_t &, const costs_t &) > transit
    )
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

/** @} Algorithm */



/** Pretty print a costs map. */
void grid_print( const costs_t & grid, point_t pmin, point_t pmax, double step,
        std::ostream& out = std::cout, std::string sep = "  ", std::string end = "\n",
        unsigned int width = 5, char fill = ' ', unsigned int prec = 3)
{
    out << std::setw(width) << std::setfill(fill) << std::setprecision(prec);

    out << "   x:";
    for( double px=x(pmin); px<=x(pmax); px+=step) {
        out << sep << std::setw(width) << std::setfill(fill) << px;
    }
    out << end;
    out << "  y" << end;;
    for( double py=y(pmax); py>=y(pmin); py-=step) {
        out << std::setw(width) << std::setfill(fill) << py << ":";
        for( double px=x(pmin); px<=x(pmax); px+=step) {
            point_t p; x(p)=px; y(p)=py;
            if( has_cost(p,grid) ) {
                out << sep << std::setw(width) << std::setfill(fill) << std::setprecision(prec) << grid.at(p);
            } else {
                out << sep << std::setw(width) << std::setfill(fill) <<  ".";
            }
        }
        out << end;
    }
    out << end;
}
