package patterns.crtp;

import java.util.Arrays;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.function.Function;

import patterns.Point;
/**
 * 
 * @author pouyllau
 *
 * @param <T>
 */
public class Neighborhood<T extends Neighborhood<T>> {
	
	protected static double step;
	protected static Point pMin;
	protected static Point pMax;
	T t;
	
	public Neighborhood(double step, Point pMin, Point pMax) {
		super();
		this.step = step;
		this.pMin = pMin;
		this.pMax = pMax;
	}

	public List<Point> get(Point p) {
		return t.get(p);
	}

	
	
}
