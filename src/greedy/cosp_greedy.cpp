/*
 *  Problem : cosp_tabu_search.cpp
 *  Description : 
 *  Created by ngocjr7 on [2020-05-24 10:41]	
*/
#include "../model/utils.h"
#include "../model/cosp_instance.h"

using namespace std;

struct Assignment {
	int i, j, k, val;

	Assignment() {}

	Assignment(int _i, int _j, int _k, int _value): 
		i(_i), j(_j), k(_k), val(_value) {}

	bool operator <(const Assignment& b) const {
		return this->val < b.val;
	}
};

typedef vector<Assignment> Move;

class COSPSolution {
public:
	const COSPInstance& prob;
	const int n, m, p;
	vector<vector<vector<int> > > load;
	int **tad;
	long long max_slack;
	long long total_cost;

	COSPSolution(const COSPInstance& _prob): prob(_prob), n(_prob.n), m(_prob.m), p(_prob.p) {
		max_slack = prob.obj1;
		total_cost = -1;

		load.resize(m);
		for (int i = 0; i < m; i++) {
			load[i].resize(p);
			for (int j = 0; j < p; j++) {
				load[i][j].resize(n);
				for (int k = 0; k < n; k++) 
					load[i][j][k] = 0;
			}
		}

		tad = new int*[n];
		for (int i = 0; i < n; i++) {
			tad[i] = new int[p];
			for (int j = 0; j < p; j++)
				tad[i][j] = prob.allocated_d[i][j];
		}
		generate_greedy_solution();
	}

	int id(int i, int j,int k) const {
		return i*p*n + j*n + k;
	}

	void generate_greedy_solution() {
		int **rs = new int*[m];
		for (int i = 0; i < m; i++) {
			rs[i] = new int[p];
			for (int j = 0; j < p; j++)
				rs[i][j] = prob.s[i][j];
		}

		vector<Assignment> flatten_c;
		flatten_c.resize(n*m*p);
		for (int i = 0; i < m; i++)
			for (int j = 0; j < p; j++)
				for (int k = 0; k < n; k++) {
					flatten_c[id(i,j,k)].i = i;
					flatten_c[id(i,j,k)].j = j;
					flatten_c[id(i,j,k)].k = k;
					flatten_c[id(i,j,k)].val = prob.c[i][j][k];
				}

		sort(flatten_c.begin(),flatten_c.end());
		for (auto e: flatten_c) {

			load[e.i][e.j][e.k] = min(tad[e.k][e.j], rs[e.i][e.j]);
			tad[e.k][e.j] -= load[e.i][e.j][e.k];
			rs[e.i][e.j] -= load[e.i][e.j][e.k];
		}

		total_cost = get_cost();
	}

	void print(ostream& os = cout) {
		os << max_slack << " " << total_cost << endl;
		for (int i = 0; i < m; i++) 
			for (int j = 0; j < p; j++) {
				for (int k = 0; k < n; k++)
					os << load[i][j][k] << " ";
				os << endl;
			}
	}

	long long get_cost() {
		if (total_cost != -1) 
			return total_cost;
		total_cost = 0;
		for (int i = 0; i < m; i++)
			for (int j = 0; j < p; j++)
				for (int k = 0; k < n; k++) 
					total_cost += prob.c[i][j][k] * load[i][j][k];
		return total_cost;
	}

};


int main(int argc, char* argv[]) 
{
	parseCommandFlags(argc, argv);

	auto start = high_resolution_clock::now(); 

	COSPInstance prob;
	if ( INPUT_FILE != "" ) {
		ifstream inp(INPUT_FILE);
		prob.parse_from_stream(inp);
	} else {
		cout << "No file passing, read from stdin\n"; 
		prob.parse_from_stream(cin);
	}

	COSPSolution sol(prob);

	auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(stop - start);

	if (OUTPUT_FILE != "") {
		ofstream fout(OUTPUT_FILE);
		sol.print(fout);
		fout.close();

		ofstream fout_stat(OUTPUT_FILE + "_stat");
		fout_stat << "\truntime: \t\t" << format_duration(duration) << " (" << float(duration.count())/1000 << "ms)" << endl;
		fout_stat.close();
	}

	sol.print(cout);
	cout << "\truntime: \t\t" << format_duration(duration) << " (" << float(duration.count())/1000 << "ms)" << endl;
	

	// int ret = 0; while ((ret = generator()) != -1) std::cout << "generator: " << ret << std::endl;
	return 0;
}
