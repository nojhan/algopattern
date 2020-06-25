package patterns.functionnal;

import java.util.Arrays;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.function.Function;

import patterns.Point;
/**
 * 
 * @author pouyllau
 *
 */
public class Neighborhood {
	
	protected static double step;
	protected static Point pMin;
	protected static Point pMax;
	Function<Point, List<Point>> f;
	
	public Neighborhood(Function<Point, List<Point>> f,double step, Point pMin, Point pMax) {
		super();
		this.f = f;
		this.step = step;
		this.pMin = pMin;
		this.pMax = pMax;
	}

	public List<Point> get( Point p) {
		return f.apply(p);
	}

	public static Function<Point, List<Point>> quad() {
		return x -> Point.neighbors_grid(x, step, pMin, pMax,
				Arrays.asList(new Point(1, 0), new Point(0, -1), new Point(-1, 0), new Point(0, 1)));
	}
	
	public static Function<Point, List<Point>> octo() {
		return x -> Point.neighbors_grid(x, step, pMin, pMax,Arrays.asList(new Point(1, 0), new Point(1, -1), new Point(0, -1), new Point(-1, -1),
				new Point(-1, 0), new Point(-1, 1), new Point(0, 1), new Point(1, 1)));
	}
	
}
