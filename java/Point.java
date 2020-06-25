package patterns;

import java.util.ArrayList;
import java.util.List;

/**
 * 
 * @author pouyllau
 *
 */
public class Point {

	public double x;
	public double y;

	public Point(double x, double y) {
		super();
		this.x = x;
		this.y = y;
	}

	public static double distance(Point u, Point v) {
		return Math.sqrt( (u.x - v.x)*(u.x - v.x) + (u.y - v.y)*(u.y - v.y));
	}



	/**
	 * Compute and set the coordinates of the neighbors of the given point,
	 * according to grid parameters and directions. Directions are supposed to
	 * be given in a specific order (in this project, the transit_in_simplex
	 * functions will necessitate clockwise order).
	 * 
	 * @param step
	 *            Length of an orthogonal edge of the grid.
	 * @param pMin
	 *            Corner point of the grid, with minimal coordinates.
	 * @param pMax
	 *            Corner point of the grid, with maximal coordinates.
	 * @param directions
	 *            The given direction
	 */
	public static List<Point> neighbors_grid(Point p, double step, Point pMin, Point pMax, List<Point> directions) {

		List<Point> neighbors = new ArrayList<Point>(directions.size());
		for (Point d : directions) {
			double nx = p.x + d.x * step;
			double ny = p.y + d.y * step;
			if ( pMin.x <= nx && nx <= pMax.x && pMin.y <= ny && ny <= pMax.y) {
				neighbors.add(new Point(nx, ny));
			}
		}
		assert (neighbors.size() >= 2);
		
		return neighbors;

	}

	@Override
	public int hashCode() {
		final int prime = 7919;
		int result = 1;
		long temp;
		temp = Double.doubleToLongBits(x);
		result = prime * result + (int) (temp ^ (temp >>> 7920));
		temp = Double.doubleToLongBits(y);
		result = prime * result + (int) (temp ^ (temp >>> 7920));
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		Point other = (Point) obj;
		if (Double.doubleToLongBits(x) != Double.doubleToLongBits(other.x))
			return false;
		if (Double.doubleToLongBits(y) != Double.doubleToLongBits(other.y))
			return false;
		return true;
	}

	@Override
	public String toString() {
		return "Point [x=" + x + ", y=" + y+ "]";
	}
	
	

}
