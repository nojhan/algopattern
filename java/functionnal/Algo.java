package patterns.functionnal;

import java.text.DecimalFormat;
import java.text.SimpleDateFormat;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.PriorityQueue;

import patterns.Point;
/**
 * 
 * @author pouyllau
 *
 */
public class Algo {

	private static boolean DEBUG = false;

	public static Map<Point, Double> run(Point seed, int iterations, Neighborhood neighborhood, Transit transit) {
		Map<Point, Double> costs = new HashMap<Point, Double>();

		PriorityQueue<Point> front = new PriorityQueue<Point>(new Comparator<Point>() {
			public int compare(Point lhs, Point rhs) {
				if (costs.get(lhs) >= costs.get(rhs))
					return +1;
				if (costs.get(lhs) < costs.get(rhs))
					return -1;
				return 0;
			}
		});

		costs.put(seed, 0.);
		front.add(seed);

		for (int i = 0; i < iterations && !front.isEmpty(); i++) {
			if (DEBUG)
				System.out.println(i + "/" + iterations);

			// Accept the node with the min cost and update neighbors.
			// Accept the considered node with the min cost.
			Point accepted = front.poll();

			// Consider neighbors of the accepted node.
			List<Point> around = neighborhood.get(accepted);
			assert (around.size() > 0);
			
			for (Point n : around) {
				// If no cost has been computed (i.e. the node is "open").
				if (!Transit.hasCost(n, costs)) {

					double ncost = transit.transit(n, neighborhood.get(n), costs);
					if (DEBUG)
						System.out.println(n + " cost=" + ncost);
					costs.put(n, ncost);
					front.add(n);
				}
			}
		}
		return costs;
	}

	public static void printGrid(Map<Point, Double> grid, Point pMin, Point pMax, double step, String sep) {
		if (sep == null)
			sep = "\t";
		String width = "     ";
		System.out.print("   x:"+sep);
		
		for (double px = pMin.x; px <= pMax.x; px += step) {
			System.out.print(sep  + (int)px+ "   "+  width );
		}
		
		System.out.print("\n");
		System.out.print("  y\n");
		
		for (double py = pMax.y; py >= pMin.y; py -= step) {
			System.out.print(sep + (int)py + ":");
			for (double px = pMin.x; px <= pMax.x; px += step) {
				Point p = new Point(px, py);
				if (Transit.hasCost(p, grid)) {
					System.out.format(sep + "%.2f" + width, grid.get(p));
				} else {
					System.out.print(sep + " .  " + width);
				}
			}
			System.out.print("\n");
		}
		System.out.print("\n");
	}

	public static void performEvaluations(Point seed, int iterations, Neighborhood neighborhood, Transit transit, int T) {
		double mean = 0;
		double m2 = 0;
		for (int t = 0; t < T; t++) {
			long startTime = System.nanoTime();
			run(seed, iterations, neighborhood, transit);
			startTime = System.nanoTime() - startTime;
			double delta = (double)startTime - mean;
			mean += delta / (double) T;
			m2 += delta * delta;
		}
		m2 = m2 / (double)(T-1.);
		m2 = Math.sqrt(m2);
		System.out.println("Mean time : " + new DecimalFormat("#.##########").format((mean / 1000000000)) + " s"
				+ " sd : " + new DecimalFormat("#.##########").format((m2 / 1000000000)));
	}
	
	public static void main(String[] args) {
		Locale.setDefault(Locale.US);
		Point seed = new Point(0, 0);
		Point pMin = new Point(-5, -5);
		Point pMax = new Point(15, 15);
		double step = 1.;
		int maxit = 300;
		double eps = 1. / 100.0;
		int eval = 100;
		
		Neighborhood quad= new Neighborhood(Neighborhood.quad(), step, pMin, pMax);
		Neighborhood octo = new Neighborhood(Neighborhood.octo(), step, pMin, pMax);

		Transit graph = new Transit(Transit.edge());
		Transit mesh = new Transit(Transit.simplex(eps));

		    
		System.out.println("Dijkstra, 4 neighbors");
		performEvaluations(seed, maxit, quad, graph, eval);
		System.out.println("Dijkstra, 8 neighbors");
		performEvaluations(seed, maxit, octo, graph, eval);
		System.out.println("Fast marching, 4 neighbors");
		performEvaluations(seed, maxit, quad, mesh, eval);
		System.out.println("Fast marching, 8 neighbors");
		performEvaluations(seed, maxit, octo, mesh, eval);
		/*    
		System.out.println("Dijkstra, 4 neighbors");
		long startTime = System.nanoTime();
		Map<Point, Double> dijkstra4 = run(seed, maxit, quadCrtp, graph);
		startTime = System.nanoTime() - startTime;
		double duration = ((double)startTime / 1000000000);
		System.out.println("solution Time : " + new DecimalFormat("#.##########").format(duration) + " Seconds");
		printGrid(dijkstra4, pMin, pMax, step, null);
		
		System.out.println("Dijkstra, 8 neighbors");
		startTime = System.nanoTime();
		Map<Point, Double> dijkstra8 = run(seed, maxit, octoCrtp, graph);
		startTime = System.nanoTime() - startTime;
		duration = ((double)startTime / 1000000000);
		System.out.println("solution Time : " + new DecimalFormat("#.##########").format(duration) + " Seconds");
		printGrid(dijkstra8, pMin, pMax, step, null);
		
		System.out.println("Fast marching, 4 neighbors");
		startTime = System.nanoTime();
		Map<Point, Double> fast_marching4 = run(seed, maxit, quadCrtp, mesh);
		startTime = System.nanoTime() - startTime;
		duration = ((double)startTime / 1000000000);
		System.out.println("solution Time : " + new DecimalFormat("#.##########").format(duration) + " Seconds");
		printGrid(fast_marching4, pMin, pMax, step, null);
		
		
		System.out.println("Fast marching, 8 neighbors");
		startTime = System.nanoTime();
		Map<Point, Double> fast_marching8 = run(seed, maxit, octoCrtp, mesh);
		startTime = System.nanoTime() - startTime;
		duration = ((double)startTime / 1000000000);
		System.out.println("solution Time : " + new DecimalFormat("#.##########").format(duration) + " Seconds");
		printGrid(fast_marching8, pMin, pMax, step, null);*/


	}
}
