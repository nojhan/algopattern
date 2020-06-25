package patterns.crtp;

import java.util.List;
import java.util.Map;

import patterns.Point;
/**
 * 
 * @author pouyllau
 *
 * @param <T>
 */
public class Transit<T extends Transit<T>> {

	T t;
	
	
	public double transit(Point p, List<Point> neighbors, Map<Point, Double> costs) {
		return  t.transit(p, neighbors, costs);
	}
	
	public static boolean hasCost(Point p ,Map<Point, Double> costs) {
		return (costs.get(p) != null && costs.get(p) != Double.POSITIVE_INFINITY);
	}
}
