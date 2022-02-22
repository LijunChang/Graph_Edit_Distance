#ifndef _GRAPH_H_
#define _GRAPH_H_

#include "Utility.h"

class Graph {
public:
	std::string id;
	ui n, m;
	ui *pstarts, *edges;
	ui *vlabels, *elabels;

public:
	Graph(const std::string &_id, const std::vector<std::pair<int,ui> > &_vertices, const std::vector<std::pair<std::pair<int,int>,ui> > &_edges) {
		id = _id;
		n = _vertices.size();
		m = _edges.size();

		pstarts = new ui[n+1]; vlabels = new ui[n];
		edges = new ui[m]; elabels = new ui[m];

		for(ui i = 0;i < n;i ++) vlabels[i] = _vertices[i].second;

		for(ui i = 0;i < m;i ++) {
			edges[i] = _edges[i].first.second;
			elabels[i] = _edges[i].second;
			//assert(elabels[i] >= 0&&elabels[i] < 3);
		}

		ui idx = 0;
		pstarts[0] = idx;
		for(ui i = 0;i < n;i ++) {
			while(idx < m&&_edges[idx].first.first == i) ++ idx;
			pstarts[i+1] = idx;
		}
		assert(pstarts[n] == m);
	}

	~Graph() {
		if(pstarts != nullptr) {
			delete[] pstarts;
			pstarts = nullptr;
		}
		if(edges != nullptr) {
			delete[] edges;
			edges = nullptr;
		}
		if(vlabels != nullptr) {
			delete[] vlabels;
			vlabels = nullptr;
		}
		if(elabels != nullptr) {
			delete[] elabels;
			elabels = nullptr;
		}
	}

	void write_graph(FILE *fout, const std::vector<std::string> &_vlabels, const std::vector<std::string> &_elabels, bool bss) {
		assert(fout != NULL);
		if(bss) {
			for(ui i = 0;i <  id.length();i ++) if(id[i] < '0'||id[i] > '9') printf("!!! Wrong graph id for bss\n");
			fprintf(fout, "%s\n", id.c_str());
			fprintf(fout, "%d %d\n", n, m/2);
			for(ui i = 0;i < n;i ++) fprintf(fout, "%d\n", vlabels[i]);
			for(ui i = 0;i < n;i ++) for(ui j = pstarts[i];j < pstarts[i+1];j ++) if(edges[j] > i) {
				fprintf(fout, "%u %u %d\n", i, edges[j], elabels[j]);
			}
		}
		else{
			fprintf(fout, "t # %s\n", id.c_str());
			for(ui i = 0;i < n;i ++) fprintf(fout, "v %u %s\n", i, _vlabels[vlabels[i]].c_str());
			for(ui i = 0;i < n;i ++) for(ui j = pstarts[i];j < pstarts[i+1];j ++) if(edges[j] > i) {
				fprintf(fout, "e %u %u %s\n", i, edges[j], _elabels[elabels[j]].c_str());
			}
		}
	}

	bool is_connected() {
		std::vector<ui> Q;
		char *vis = new char[n];
		memset(vis, 0, sizeof(char)*n);
		vis[0] = 1;
		Q.push_back(0);
		for(ui i = 0;i < Q.size();i ++) {
			for(ui j = pstarts[Q[i]];j < pstarts[Q[i]+1];j ++) if(!vis[edges[j]]) {
				vis[edges[j]] = 1;
				Q.push_back(edges[j]);
			}
		}

		if(Q.size() == n) return 1;
		return 0;
	}

	ui size_based_bound(Graph *g) {
		ui r1 = n>g->n ? n-g->n : g->n-n;
		ui r2 = m>g->m ? m-g->m : g->m-m;
		assert(r2%1 == 0);
		return r1 + r2/2;
	}

	ui ged_lower_bound_filter(Graph *g, ui verify_upper_bound, int *vlabel_cnt, int *elabel_cnt, int *degree_q, int *degree_g, int *tmp) {
		ui lb = size_based_bound(g);
		if(lb > verify_upper_bound) return lb;

		lb = (n > g->n ? n : g->n);
		for(ui i = 0;i < n;i ++) ++ vlabel_cnt[vlabels[i]];
		for(ui i = 0;i < g->n;i ++) {
			ui vl = g->vlabels[i];
			if(vlabel_cnt[vl] > 0) {
				-- vlabel_cnt[vl];
				-- lb;
			}
		}
		for(ui i = 0;i < n;i ++) vlabel_cnt[vlabels[i]] = 0;
		if(lb > verify_upper_bound) return lb;

		assert(m == pstarts[n]&&g->m == g->pstarts[g->n]);
		for(ui i = 0;i < n;i ++) degree_q[i] = pstarts[i+1] - pstarts[i];
		for(ui i = 0;i < g->n;i ++) degree_g[i] = g->pstarts[i+1] - g->pstarts[i];
		int *degrees_cnt_q = tmp;
		int max_degree_q = 0, max_degree_g = 0;
		memset(degrees_cnt_q, 0, sizeof(int)*n);
		for(ui i = 0;i < n;i ++) {
			int td = degree_q[i];
			++ degrees_cnt_q[td];
			if(td > max_degree_q) max_degree_q = td;
		}
		int *degrees_cnt_g = degree_q;
		memset(degrees_cnt_g, 0, sizeof(int)*g->n);
		for(ui i = 0;i < g->n;i ++) {
			ui td = degree_g[i];
			++ degrees_cnt_g[td];
			if(td > max_degree_g) max_degree_g = td;
		}
		ui de = 0, ie = 0;
		while(max_degree_q > 0&& max_degree_g > 0) {
			if(degrees_cnt_q[max_degree_q] == 0) {
				-- max_degree_q;
				continue;
			}
			if(degrees_cnt_g[max_degree_g] == 0) {
				-- max_degree_g;
				continue;
			}

			ui td = mmin(degrees_cnt_q[max_degree_q], degrees_cnt_g[max_degree_g]);
			if(max_degree_q > max_degree_g) de += td*(max_degree_q - max_degree_g);
			else ie += td*(max_degree_g - max_degree_q);
			degrees_cnt_q[max_degree_q] -= td;
			degrees_cnt_g[max_degree_g] -= td;
		}
		while(max_degree_q > 0) {
			de += max_degree_q*degrees_cnt_q[max_degree_q];
			-- max_degree_q;
		}
		while(max_degree_g > 0) {
			ie += max_degree_g*degrees_cnt_g[max_degree_g];
			-- max_degree_g;
		}
		de = (de+1)/2; ie = (ie+1)/2;

		ui edge_lb = de+ie;
		if(de*2 + g->m/2 > m/2&&de*2 + g->m/2 - m/2 > edge_lb) edge_lb = de*2 + g->m/2 - m/2;
		if(ie*2 + m/2 > g->m/2&&ie*2 + m/2 - g->m/2 > edge_lb) edge_lb = ie*2 + m/2 - g->m/2;
		if(lb + edge_lb > verify_upper_bound) return lb + edge_lb;

		ui common_elabel_cnt = 0;
		for(ui i = 0;i < pstarts[n];i ++) ++ elabel_cnt[elabels[i]];
		for(ui i = 0;i < g->pstarts[g->n];i ++) {
			ui el = g->elabels[i];
			if(elabel_cnt[el] > 0) {
				-- elabel_cnt[el];
				++ common_elabel_cnt;
			}
		}
		for(ui i = 0;i < pstarts[n];i ++) elabel_cnt[elabels[i]] = 0;
		common_elabel_cnt /= 2;
		if(de + g->m/2 - common_elabel_cnt > edge_lb) edge_lb = de + g->m/2 - common_elabel_cnt;
		if(ie + m/2 - common_elabel_cnt > edge_lb) edge_lb = de + m/2 - common_elabel_cnt;
		ui e_cnt = m;
		if(g->m > e_cnt) e_cnt = g->m;
		e_cnt /= 2;
		if(e_cnt - common_elabel_cnt > edge_lb) edge_lb = e_cnt - common_elabel_cnt;

		return lb + edge_lb;
	}
};

#endif
