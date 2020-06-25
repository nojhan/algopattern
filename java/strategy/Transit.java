package patterns.strategy;

import java.util.List;
import java.util.Map;

import patterns.Point;
/**
 * 
 * @author pouyllau
 *
 */
public abstract class Transit {

	public abstract double transit(Point p, List<Point> neighbors, Map<Point, Double> costs);
	
	public static boolean hasCost(Point p ,Map<Point, Double> costs) {
		return (costs.get(p) != null && costs.get(p) != Double.POSITIVE_INFINITY);
	}
}
