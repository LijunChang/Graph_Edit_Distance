#include "Application.h"
#include "Graph.h"
#include "Utility.h"
#include "Timer.h"
#include "popl.hpp"

#include <iostream>
#include <vector>
#include <climits>

using namespace std;
using namespace popl;

void print_usage() {
	printf("Usage: ./ged -h -d database_file -q query_file -m running_mode -p search_paradigm -l lower_bound_method -t ged_threshold\n");
	printf("**** Note that for GED verification, if the returned value is not -1, then it is only an upper bound of (and may be larger than) the exact GED\n\n");
}

ui label2int(const char *str, map<string,ui> &M) {
	if (M.find(string(str)) == M.end()) M[string(str)] = M.size();
	return M[string(str)];
}

ui load_db(const char *file_name, vector<Graph*> &graphs, map<string,ui> &vM, map<string,ui> &eM) {
	FILE *fin = Utility::open_file(file_name, "r");

	const ui MAX_LINE = 1024;
	char line[MAX_LINE];
	if (fgets(line, MAX_LINE, fin) == nullptr) {
		fclose(fin);
		return 0;
	}

	ui max_n = 0;
	while(line[0] == 't') {
		char buf[128], buf1[128];
		sscanf(line+2, "%s%s", buf1, buf);
		string id(buf);
		line[0] = 'x';

		vector<pair<int,ui>> vertices;
		vector<pair<pair<int,int>,ui>> edges;
		while(fgets(line, MAX_LINE, fin) != nullptr && line[0] != 't') {
			if (line[0] == 'v') {
				int a;
				sscanf(line+2, "%d%s", &a, buf);
				vertices.pb(mp(a, label2int(buf, vM)));
			}
			else if (line[0] == 'e') {
				int a, b;
				sscanf(line+2, "%d%d%s", &a, &b, buf);
				edges.pb(mp(mp(a,b), label2int(buf, eM)));
				edges.pb(mp(mp(b,a), label2int(buf, eM)));
			}
			else printf("!!! Unrecognized first letter in a line when loading DB !!!\n");
			line[0] = 'x';
		}

        int v_size = static_cast<int>(vertices.size());
        int e_size = static_cast<int>(edges.size());
		sort(vertices.begin(), vertices.end());
		for (ui i = 0; i < v_size; i++) assert(vertices[i].first == i);
		if (v_size > max_n) max_n = v_size;

		sort(edges.begin(), edges.end());
		for (ui i = 0; i < e_size; i++) {
			assert(edges[i].first.first >= 0 && edges[i].first.first < v_size);
			assert(edges[i].first.second >= 0 && edges[i].first.second < v_size);
			if (i > 0) assert(edges[i].first != edges[i-1].first);
			assert(edges[i].second < eM.size());
		}

		graphs.pb(new Graph(id, vertices, edges));
	}

	fclose(fin);
	return max_n;
}

void generate_queries(const vector<Graph *> &db, vector<ui> &queries, ui q_n) {
	assert(!db.empty());
	srand(time(nullptr));
	for (ui i = 0; i < q_n; i ++) queries.pb(rand() % db.size());
}

void write_queries(const char *file_name, const vector<Graph *> &db, const vector<ui> &queries, const map<string,ui> &vM, const map<string,ui> &eM, bool bss) {
	vector<string> vlabels(vM.size());
	vector<string> elabels(eM.size());

	for (pair<string,ui> p: vM) vlabels[p.second] = p.first;
	for (pair<string,ui> p: eM) elabels[p.second] = p.first;

	FILE *fout = Utility::open_file(file_name, "w");
	for (ui i = 0; i < queries.size(); i++) db[queries[i]]->write_graph(fout, vlabels, elabels, bss);
	fclose(fout);
}

int main(int argc, char *argv[]) {

#ifndef NDEBUG
	printf("**** GED (Debug) build at %s %s ***\n", __TIME__, __DATE__);
#else
	printf("**** GED (Release) build at %s %s ***\n", __TIME__, __DATE__);
#endif

	print_usage();

	string mode, paradigm, lower_bound, save_filepath;
	int threshold = -1;
	bool print_ged = false;

	OptionParser op("Allowed options");
	auto help_option = op.add<Switch>("h", "help", "\'produce help message\'");
	auto database_option = op.add<Value<string>>("d", "database", "\'database file name\'");
	auto query_option = op.add<Value<string>>("q", "query", "\'query file name\'");
	auto save_option = op.add<Value<string>>("s", "save", "\'pairwise distance filename", "datasets/pairwise_ged.csv", &save_filepath);
	auto mode_option = op.add<Value<string>>("m", "mode", "\'running mode\' (search | pair | pairwise)", "search", &mode);
	auto paradigm_option = op.add<Value<string>>("p", "paradigm", "\'search paradigm\' (astar | dfs)", "astar", &paradigm);
	auto lower_bound_option = op.add<Value<string>>("l", "lower_bound", "\'lower bound method\' (LSa | BMao | BMa)", "BMao", &lower_bound);
	auto threshold_option = op.add<Value<int>>("t", "threshold", "\'threshold for GED verification; if not provided, then GED computation", -1, &threshold);
	op.add<Switch>("g", "ged", "\'print_ged\'", &print_ged);
	op.parse(argc, argv);

	if (help_option->is_set()||argc == 1) cout << op << endl;
	if (mode == "pairwise") {
	    if (!database_option->is_set()) {
	    	printf("!!! Database file name not provided! Exit !!!\n");
		    return 0;
	    } else {
	        query_option = database_option;
	    }
	    if (query_option->is_set()) {
	        printf("Warning: query file ignored. Pairwise distances are calculated for the compounds from database file\n");
	    }
	} else if (!database_option->is_set() || !query_option->is_set()) {
		printf("!!! Database file name or query file name is not provided! Exit !!!\n");
		return 0;
	}
	string database = database_option->value();
	string query = query_option->value();

	vector<Graph *> db;
	map<string, ui> vM, eM;
	ui max_db_n = load_db(database.c_str(), db, vM, eM);

#ifdef _EXPAND_ALL_
	// printf(" Expand_all");
#else
	printf(" Expand_one");
#endif
#ifndef _EARLY_STOP_
	printf(" NoEarly_stop");
#endif
#ifndef _UPPER_BOUND_
	printf(" NoUpper_bound");
#endif
	printf(" ***\n");

	vector<Graph *> queries;
	ui max_query_n = load_db(query.c_str(), queries, vM, eM);

	ui verify_upper_bound;
	if (threshold < 0) verify_upper_bound = INF;
	else verify_upper_bound = (ui)threshold;

	long long search_space = 0;
	long long results_cnt = 0, candidates_cnt = 0;

	ui pre = 1000;

	int *vlabel_cnt = new int[vM.size()];
	int *elabel_cnt = new int[eM.size()];
	memset(vlabel_cnt, 0, sizeof(int)*vM.size());
	memset(elabel_cnt, 0, sizeof(int)*eM.size());

	if (max_query_n > max_db_n) max_db_n = max_query_n;
	int *degree_q = new int[max_db_n];
	int *degree_g = new int[max_db_n];
	int *tmp = new int[max_db_n];

	if (mode != "pair" && mode != "search" && mode != "pairwise") {
		printf("!!! Wrong mode (pair | search) selection !!!\n");
		return 0;
	}
	if (paradigm != "astar" && paradigm != "dfs") {
		printf("!!! Wrong algorithm (astar | dfs) selection !!!\n");
		return 0;
	}

	Timer t;

	if (mode == "pair") {
		long long time1 = 0, cnt1 = 0, ss1 = 0;
		long long time2 = 0, cnt2 = 0, ss2 = 0;

        int db_size = static_cast<int>(db.size());
        int q_size = static_cast<int>(queries.size());
		if (q_size != db_size) {
			printf("Query size != db size in the pair mode\n");
			exit(0);
		}
		if (print_ged) printf("*** GEDs ***\n");
		ui min_ged = INT_MAX, max_ged = 0;
		for (ui i = 0; i < q_size; i++) {
			ui current = i*100/q_size;
			if (current != pre) {
				fprintf(stderr, "\r[%d%% finished]", current);
				fflush(stderr);
				pre = current;
			}

			ui lb = queries[i]->ged_lower_bound_filter(db[i], verify_upper_bound, vlabel_cnt, elabel_cnt, degree_q, degree_g, tmp);
			if (lb > verify_upper_bound) continue;

			++candidates_cnt;
			Timer t1;
			Application *app = new Application(verify_upper_bound, lower_bound.c_str());
			app->init(db[i], queries[i]);
			int res = INF;
			if (paradigm == "astar") res = app->AStar();
			else res = app->DFS(nullptr);
#ifndef NDEBUG
			assert(res == app->compute_ged_of_BX());
#endif
			search_space += app->get_search_space();
			if (res <= verify_upper_bound)++results_cnt;
			else res = -1;

			if (print_ged) {
				printf("%d\n", res);
				if (res > max_ged) max_ged = res;
				if (res < min_ged) min_ged = res;
			}
			if (res == -1) {
				time2 += t1.elapsed();
				ss2 += app->get_search_space();
				++cnt2;
			} else {
				time1 += t1.elapsed();
				ss1 += app->get_search_space();
				++cnt1;
			}
			delete app;
		}
		fprintf(stderr, "\n");
		if (print_ged) {
			printf("*** GEDs ***\n");
			printf("min_ged: %u, max_ged: %u\n", min_ged, max_ged);
		}

		if (cnt1 + cnt2 != 0) printf("total average time: %s, total average_ss: %lld\n", Utility::integer_to_string((time1+time2)/(cnt1+cnt2)).c_str(), (ss1+ss2)/(cnt1+cnt2));
		if (verify_upper_bound < INF) {
			printf("Dissimilar (%lld pairs) average time: ", cnt2);
			if (cnt2 == 0) printf("0, ");
			else printf("%s, ", Utility::integer_to_string(time2/cnt2).c_str());

			printf("Dissimilar average space: ");
			if (cnt2 == 0) printf("0\n");
			else printf("%lld\n", ss2/cnt2);

			printf("Similar (%lld pairs) average time: ", cnt1);
			if (cnt1 == 0) printf("0, ");
			else printf("%s, ", Utility::integer_to_string(time1/cnt1).c_str());
			printf("Similar average space: ");
			if (cnt1 == 0) printf("0\n");
			else printf("%lld\n", ss1/cnt1);
		}
		printf("Total time: %s (microseconds), total search space: %lld\n #candidates: %lld, #matches: %lld\n", Utility::integer_to_string(t.elapsed()).c_str(), search_space, candidates_cnt, results_cnt);
	} else if (mode == "pairwise") {
		long long total_res = 0;
		int db_size = static_cast<int>(db.size());
		vector<vector<int>> ged_matrix(db_size, vector<int>(db_size, 0));
		ui min_ged = INT_MAX, max_ged = 0;

		for (ui i = 0; i < db_size; i++) {
			for (ui j = i+1; j < db_size; j++) {
				Application *app = new Application(verify_upper_bound, lower_bound.c_str());
				app->init(db[i], db[j]);
				int res = INF;
				if (paradigm == "astar") res = app->AStar();
				else res = app->DFS(nullptr);
				ged_matrix[i][j] = ged_matrix[j][i] = res;
				if (res > max_ged) max_ged = res;
				if (res < min_ged) min_ged = res;
				total_res += res;

				delete app;
			}
		}
		std::ofstream out(save_filepath);
		for (auto& row : ged_matrix) {
			for (int k = 0; k < db_size-1; k++) {
				out << row[k] <<',';
			}
			out << row[db_size-1] << '\n';
		}
		printf("*** GEDs ***\n");
		printf("Min ged: %u, max ged: %u avg ged: %.3lf\n", min_ged, max_ged, double(total_res) / (db_size*(db_size-1)/2));
		printf("Total time: %s (microseconds)\n", Utility::integer_to_string(t.elapsed()).c_str());
		printf("Results saved in %s\n", save_filepath.c_str());
	} else {
		long long total_res = 0;
		int db_size = static_cast<int>(db.size());
        int q_size = static_cast<int>(queries.size());

		if (print_ged) printf("*** GEDs ***\n");
		ui min_ged = INT_MAX, max_ged = 0;
		for (ui i = 0; i < q_size; i++) {
			for (ui j = 0; j < db_size; j++) {
				ui current = (i*(long long)(db_size)+j+1)*100/(q_size*(long long)(db_size));
				if (current != pre) {
					fprintf(stderr, "\r[%d%% finished]", current);
					fflush(stderr);
					pre = current;
				}

				ui lb = queries[i]->ged_lower_bound_filter(db[j], verify_upper_bound, vlabel_cnt, elabel_cnt, degree_q, degree_g, tmp);
				if (lb > verify_upper_bound) continue;

				++candidates_cnt;
				Application *app = new Application(verify_upper_bound, lower_bound.c_str());
				// app->init(db_v[i], db_e[i], query_v[i], query_e[i]);
				app->init(db[j], queries[i]);
				int res = INF;
				if (paradigm == "astar") res = app->AStar();
				else res = app->DFS(nullptr);
#ifndef NDEBUG
				assert(res == app->compute_ged_of_BX());
#endif

				if (print_ged) {
					if (j) printf(" ");
					printf("%u", res);
					if (res > max_ged) max_ged = res;
					if (res < min_ged) min_ged = res;
				}
				total_res += res;
				// printf("pair %lu (%s, %s): %d\n", i*db.size()+j, queries[i]->id.c_str(), db[j]->id.c_str(), res);

				search_space += app->get_search_space();
				if (res <= verify_upper_bound) ++results_cnt;
				delete app;
			}
			if (print_ged) printf("\n");
		}
		fprintf(stderr, "\n");
		if (print_ged) {
			printf("*** GEDs ***\n");
			printf("Min ged: %u, max ged: %u avg ged: %.3lf\n", min_ged, max_ged, double(total_res) / (q_size*db_size));
		}
		printf("Total time: %s (microseconds), total search space: %lld\n #candidates: %lld, #matches: %lld\n", Utility::integer_to_string(t.elapsed()).c_str(), search_space, candidates_cnt, results_cnt);
	}

	delete[] vlabel_cnt; vlabel_cnt = nullptr;
	delete[] elabel_cnt; elabel_cnt = nullptr;
	delete[] degree_q; degree_q = nullptr;
	delete[] degree_g; degree_g = nullptr;
	delete[] tmp; tmp = nullptr;

	for (ui i = 0; i < db.size(); i++) {
		delete db[i];
		db[i] = nullptr;
	}
	for (ui i = 0; i < queries.size(); i++) {
		delete queries[i];
		queries[i] = nullptr;
	}
	return 0;
}
