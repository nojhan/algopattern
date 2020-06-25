package patterns.strategy;

import java.util.List;
import java.util.Map;

import patterns.Point;
/**
 * Find the transit of minimal cost among the given edges.

Edges are given as the considered point and the sequence of neighbors points.
 
 * @author pouyllau
 */


public class TransitOnEdge extends Transit {

	public double transit(Point p, List<Point> neighbors,  Map<Point, Double> costs) {
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
}
