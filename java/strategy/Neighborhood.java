package patterns.strategy;

import java.util.List;

import patterns.Point;
/**
 * 
 * @author pouyllau
 *
 */
public abstract class Neighborhood  {
	protected double step;
	protected Point pMin;
	protected Point pMax;
	protected List<Point> directions;
	
	
	public Neighborhood(double step, Point pMin, Point pMax) {
		super();
		this.step = step;
		this.pMin = pMin;
		this.pMax = pMax;
	}

	public abstract List<Point> get(Point p);
	
}
