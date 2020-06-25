package patterns.crtp;

import java.util.Arrays;
import java.util.List;

import patterns.Point;
/**
 * 
 * @author pouyllau
 *
 */
public class OctoGrid extends Neighborhood<OctoGrid> {

	public OctoGrid(double step, Point pMin, Point pMax) {
		super(step, pMin, pMax);
		this.t = this;
	}

	public List<Point> get(Point p) {
		return Point.neighbors_grid(p, step, pMin, pMax,
				Arrays.asList(new Point(1, 0), new Point(1, -1), new Point(0, -1), new Point(-1, -1), new Point(-1, 0),
						new Point(-1, 1), new Point(0, 1), new Point(1, 1)));
	}

}
