#include "Application.h"
#include "Graph.h"
#include "Utility.h"
#include "Timer.h"
#include "popl.hpp"

using namespace std;
using namespace popl;

void print_usage() {
	printf("Usage: ./ged -h -d database_file -q query_file -m running_mode -p search_paradigm -l lower_bound_method -t ged_threshold\n");
	printf("**** Note that for GED verification, if the returned value is not -1, then it is only an upper bound of (and may be larger than) the exact GED\n\n");
}

ui label2int(const char *str, map<string,ui> &M) {
	if(M.find(string(str)) == M.end()) M[string(str)] = M.size();
	return M[string(str)];
}

ui load_db(const char *file_name, vector<Graph*> &graphs, map<string,ui> &vM, map<string,ui> &eM) {
	FILE *fin = Utility::open_file(file_name, "r");

	const ui MAX_LINE = 1024;
	char line[MAX_LINE];
	if(fgets(line, MAX_LINE, fin) == NULL) {
		fclose(fin);
		return 0;
	}

	ui max_n = 0;
	while(line[0] == 't') {
		char buf[128], buf1[128];
		sscanf(line+2, "%s%s", buf1, buf);
		string id(buf);
		line[0] = 'x';

		vector<pair<int,ui> > vertices;
		vector<pair<pair<int,int>,ui> > edges;
		while(fgets(line, MAX_LINE, fin) != NULL&&line[0] != 't') {
			if(line[0] == 'v') {
				int a;
				sscanf(line+2, "%d%s", &a, buf);
				//buf[0] = '1';
				vertices.pb(mp(a, label2int(buf, vM)));
			}
			else if(line[0] == 'e') {
				int a, b;
				sscanf(line+2, "%d%d%s", &a, &b, buf);
				edges.pb(mp(mp(a,b), label2int(buf, eM)));
				edges.pb(mp(mp(b,a), label2int(buf, eM)));
			}
			else printf("!!! Unrecongnized first letter in a line when loading DB!\n");
			line[0] = 'x';
		}

		sort(vertices.begin(), vertices.end());
		for(ui i = 0;i < vertices.size();i ++) assert(vertices[i].first == i);
		if(vertices.size() > max_n) max_n = vertices.size();

		sort(edges.begin(), edges.end());
		for(ui i = 0;i < edges.size();i ++) {
			assert(edges[i].first.first >= 0&&edges[i].first.first < vertices.size());
			assert(edges[i].first.second >= 0&&edges[i].first.second < vertices.size());
			if(i > 0) assert(edges[i].first != edges[i-1].first);
			assert(edges[i].second < eM.size());
		}

		graphs.pb(new Graph(id, vertices, edges));
	}

	fclose(fin);
	return max_n;
}

void generate_queries(const vector<Graph *> &db, vector<ui> &queries, ui q_n) {
	assert(!db.empty());
	srand(time(NULL));
	for(ui i = 0;i < q_n;i ++) queries.pb(rand()%db.size());
}

void write_queries(const char *file_name, const vector<Graph *> &db, const vector<ui> &queries, const map<string,ui> &vM, const map<string,ui> &eM, bool bss) {
	vector<string> vlabels(vM.size());
	vector<string> elabels(eM.size());

	for(pair<string,ui> p: vM) vlabels[p.second] = p.first;
	for(pair<string,ui> p: eM) elabels[p.second] = p.first;

	FILE *fout = Utility::open_file(file_name, "w");

	for(ui i = 0;i < queries.size();i ++) db[queries[i]]->write_graph(fout, vlabels, elabels, bss);

	fclose(fout);
}

int main(int argc, char *argv[]) {
	string mode, paradigm, lower_bound;
	int threshold = -1;
	bool print_ged = false;

	OptionParser op("Allowed options");
	auto query_option = op.add<Value<string>>("q", "query", "\'query file name\'");
	auto mode_option = op.add<Value<string>>("m", "mode", "\'running mode\' (search | pair)", "search", &mode);
	auto paradigm_option = op.add<Value<string>>("p", "paradigm", "\'search paradigm\' (astar | dfs)", "astar", &paradigm);
	auto lower_bound_option = op.add<Value<string>>("l", "lower_bound", "\'lower bound method\' (LSa | BMao | BMa)", "BMao", &lower_bound);
	auto threshold_option = op.add<Value<int>>("t", "threshold", "\'threshold for GED verification; if not provided, then GED computation", -1, &threshold);
	op.add<Switch>("g", "ged", "\'print_ged\'", &print_ged);

	op.parse(argc, argv);


	string database = "../dataForReal/52373.txt";
	string query = query_option->value();

	vector<Graph *> db;
	map<string, ui> vM, eM;
	ui max_db_n = load_db(database.c_str(), db, vM, eM);

	// printf("*** %s %s %s %d: %s %s", mode.c_str(), paradigm.c_str(), lower_bound.c_str(), threshold, database.c_str(), query.c_str());
#ifdef _EXPAND_ALL_
	//printf(" Expand_all");
#else
	printf(" Expand_one");
#endif
#ifndef _EARLY_STOP_
	printf(" NoEarly_stop");
#endif
#ifndef _UPPER_BOUND_
	printf(" NoUpper_bound");
#endif
	// printf(" ***\n");

	vector<Graph *> queries;
	ui max_query_n = load_db(query.c_str(), queries, vM, eM);

	ui verify_upper_bound;
	if(threshold < 0) verify_upper_bound = INF;
	else verify_upper_bound = (ui)threshold;

	long long search_space = 0;
	long long results_cnt = 0, candidates_cnt = 0;

	ui pre = 1000;

	int *vlabel_cnt = new int[vM.size()];
	int *elabel_cnt = new int[eM.size()];
	memset(vlabel_cnt, 0, sizeof(int)*vM.size());
	memset(elabel_cnt, 0, sizeof(int)*eM.size());

	if(max_query_n > max_db_n) max_db_n = max_query_n;
	int *degree_q = new int[max_db_n];
	int *degree_g = new int[max_db_n];
	int *tmp = new int[max_db_n];

	if(strcmp(mode.c_str(), "pair") != 0&&strcmp(mode.c_str(), "search") != 0) {
		printf("!!! Wrong mode (pair | search) selection!\n");
		return 0;
	}
	if(strcmp(paradigm.c_str(), "astar") != 0&&strcmp(paradigm.c_str(), "dfs") != 0) {
		printf("!!! Wrong algorithm (astar | dfs) selection!\n");
		return 0;
	}

	Timer t;

	if(strcmp(mode.c_str(), "pair") == 0) {
		long long time1 = 0, cnt1 = 0, ss1 = 0;
		long long time2 = 0, cnt2 = 0, ss2 = 0;

		if(queries.size() != db.size()) {
			printf("Query size != db size in the pair mode\n");
			exit(0);
		}
		// if(print_ged) printf("*** GEDs ***\n");
		ui min_ged = 1000000000, max_ged = 0;
		for(ui i = 0;i < queries.size();i ++) {
			ui current = i*100/queries.size();
			if(current != pre) {
				// fprintf(stderr, "\r[%d%% finished]", current);
				fflush(stderr);
				//cout<<"\r["<<current<<"% finished]"<<flush;
				pre = current;
			}

			ui lb = queries[i]->ged_lower_bound_filter(db[i], verify_upper_bound, vlabel_cnt, elabel_cnt, degree_q, degree_g, tmp);
			if(lb > verify_upper_bound) continue;

			++ candidates_cnt;
			Timer t1;
			Application *app = new Application(verify_upper_bound, lower_bound.c_str());
			app->init(db[i], queries[i]);
			int res = INF;
			if(strcmp(paradigm.c_str(), "astar") == 0) res = app->AStar();
			else res = app->DFS(NULL); //Why I didn't see this thought it's all DFS

			search_space += app->get_search_space();
			if(res <= verify_upper_bound) ++ results_cnt;
			else res = -1;


			printf("%d", res);

			delete app;
		}
	

	}
	delete[] vlabel_cnt; vlabel_cnt = NULL;
	delete[] elabel_cnt; elabel_cnt = NULL;
	delete[] degree_q; degree_q = NULL;
	delete[] degree_g; degree_g = NULL;
	delete[] tmp; tmp = NULL;

	for(ui i = 0;i < db.size();i ++) {
		delete db[i];
		db[i] = nullptr;
	}
	for(ui i = 0;i < queries.size();i ++) {
		delete queries[i];
		queries[i] = nullptr;
	}
	return 0;
}
