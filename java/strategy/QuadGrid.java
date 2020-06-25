package patterns.strategy;

import java.util.Arrays;
import java.util.List;

import patterns.Point;
/**
 * 
 * @author pouyllau
 *
 */
public class QuadGrid extends Neighborhood {

	public QuadGrid(double step, Point pMin, Point pMax) {
		super(step, pMin, pMax);
		this.directions =  Arrays.asList(new Point(1,0), new Point (0,-1), new Point(-1,0), new Point(0,1));
	}

	public List<Point> get(Point p) {
		return Point.neighbors_grid(p, step,pMin, pMax, directions);
	}
	
	
}
