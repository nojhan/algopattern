package patterns.functionnal;

import java.util.List;
import java.util.Map;
import java.util.function.Function;
import java.util.stream.Collectors;

import patterns.Point;
/**
 * 
 * @author pouyllau
 *
 */
public class Transit {

	public static boolean hasCost(Point p ,Map<Point, Double> costs) {
		return (costs.get(p) != null && costs.get(p) != Double.POSITIVE_INFINITY);
	}
	
	TriFunction<Point, List<Point>,  Map<Point, Double>, Double> f;

	
	public Transit(TriFunction<Point, List<Point>,  Map<Point, Double>, Double> f) {
		super();
		this.f = f;
	}

	public double transit(Point p, List<Point> neighbors, Map<Point, Double> costs) {
		return f.apply(p,neighbors,costs);
	}
	
	public static TriFunction<Point, List<Point>,  Map<Point, Double>, Double> simplex(double eps) {
		return (x, y, z) -> transitInSimplex(x,y,z, eps);
	}
	
	public static TriFunction<Point, List<Point>,  Map<Point, Double>, Double> edge() {
		return (x, y, z) -> transitOnEdge(x,y,z);
	}
	
	public static double transitOnEdge(Point p, List<Point> neighbors,  Map<Point, Double> costs) {
		double mincost = Double.POSITIVE_INFINITY;
		
		for (Point n : neighbors) {
			if (hasCost(n, costs)) {
				double c = costs.get(n) + Point.distance(p, n);
				if (c < mincost) {
					mincost = c;
				}
			}
		}
		assert (mincost != Double.POSITIVE_INFINITY);
		return mincost;
	}
	
	public static double transitInSimplex(Point p, List<Point> neighbors, Map<Point, Double> costs, double eps) {

		double mincost = Double.MAX_VALUE;
		List<Point> list = neighbors.stream().filter(n -> hasCost(n, costs)).collect(Collectors.toList());
		int k = list.size();
		if (k == 1) {
			mincost = costs.get(list.get(0))+ Point.distance(list.get(0), p);
		} else {
			for (int i = 0; i < neighbors.size(); i++) {
				Point pj = neighbors.get(i);
				Point pk = (i+1 < neighbors.size() ? neighbors.get(i+1) : neighbors.get(0));
				double c = 0;

				if (hasCost(pj, costs) && hasCost(pk, costs)) {
					// Cost of the transition from/to p from/to edge e.
					// This is the simplest way to minimize the transit, even if
					// not the most efficient.
					for (double z = 0; z <= 1; z += eps) {
						double zx = z * pj.x + (1 - z) * pk.x;
						double zy = z * pj.y + (1 - z) * pk.y;

						// Linear interpolation of costs.
						c = z *costs.get(pj) + (1 - z) * costs.get(pk)+ Point.distance(p, new Point(zx, zy));
						if (c < mincost) {
							mincost = c;
						}
					} // for z

					// If the front is reached on a single point.
				} else if (hasCost(pj, costs) && !hasCost(pk, costs)) {
					c = costs.get(pj)+ Point.distance(p, pj);
					if (c < mincost) {
						mincost = c;
					}
				} else if (!hasCost(pj, costs) && hasCost(pk, costs)) {
					c = costs.get(pk) + Point.distance(p, pk);
					if (c < mincost) {
						mincost = c;
					}
				}

				
			}

		}
		assert (mincost < Double.POSITIVE_INFINITY);
		return mincost;
	}
	
}
