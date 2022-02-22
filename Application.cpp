#include "Application.h"
#include "Utility.h"

using namespace std;

Application::Application(ui _verify_upper_bound, const char *lower_bound) {
	q_n = g_n = 0;

	q_starts = q_edges = q_vlabels = q_elabels = NULL;
	g_starts = g_edges = g_vlabels = g_elabels = NULL;

	vlabels_n = elabels_n = 0;

	verify_upper_bound = _verify_upper_bound;
	upper_bound = verify_upper_bound+1;

	MO = NULL;
	search_n_for_IS = search_n = 0;

	search_space = 0;

	q_matrix = NULL;
	elabels_map = vlabels_map = NULL;

	visX = visY = NULL;
	mx = my = NULL;
	BX = NULL;
	queue = prev = NULL;
	candidates = NULL;

	q_g_swapped = false;

	states_pool_n = siblings_pool_n = 0;

	visited_siblings = NULL;
	visited_siblings_n = NULL;

	cost = NULL;
	lx = ly = slack = slackmy = NULL;

	elabels_matrix = NULL;

	children = NULL;

	if(strcmp(lower_bound, "LSa") == 0) lb_method = LSa;
	else if(strcmp(lower_bound, "BMao") == 0) lb_method = BMao;
	else if(strcmp(lower_bound, "BMa") == 0) lb_method = BMa;
	else {
		lb_method = LSa;
		printf("Lower bound %s is not available and LSa is used by default!\n", lower_bound);
	}
}

Application::~Application() {
	if(q_starts != NULL) {
		delete[] q_starts;
		q_starts = NULL;
	}
	if(q_edges != NULL) {
		delete[] q_edges;
		q_edges = NULL;
	}
	if(q_vlabels != NULL) {
		delete[] q_vlabels;
		q_vlabels = NULL;
	}
	if(q_elabels != NULL) {
		delete[] q_elabels;
		q_elabels = NULL;
	}

	if(g_starts != NULL) {
		delete[] g_starts;
		g_starts = NULL;
	}
	if(g_edges != NULL) {
		delete[] g_edges;
		g_edges = NULL;
	}
	if(g_vlabels != NULL) {
		delete[] g_vlabels;
		g_vlabels = NULL;
	}
	if(g_elabels != NULL) {
		delete[] g_elabels;
		g_elabels = NULL;
	}
	if(elabels_matrix != NULL) {
		delete[] elabels_matrix;
		elabels_matrix = NULL;
	}
	if(visited_siblings != NULL) {
		delete[] visited_siblings;
		visited_siblings = NULL;
	}
	if(visited_siblings_n != NULL) {
		delete[] visited_siblings_n;
		visited_siblings_n = NULL;
	}
	if(MO != NULL) {
		delete[] MO;
		MO = NULL;
	}
	if(visX != NULL) {
		delete[] visX;
		visX = NULL;
	}
	if(visY != NULL) {
		delete[] visY;
		visY = NULL;
	}
	if(mx != NULL) {
		delete[] mx;
		mx = NULL;
	}
	if(my != NULL) {
		delete[] my;
		my = NULL;
	}
	if(BX != NULL) {
		delete[] BX;
		BX = NULL;
	}
	if(queue != NULL) {
		delete[] queue;
		queue = NULL;
	}
	if(prev != NULL) {
		delete[] prev;
		prev = NULL;
	}
	if(candidates != NULL) {
		delete[] candidates;
		candidates = NULL;
	}
	if(cost != NULL) {
		delete[] cost;
		cost = NULL;
	}
	if(lx != NULL) {
		delete[] lx;
		lx = NULL;
	}
	if(ly != NULL) {
		delete[] ly;
		ly = NULL;
	}
	if(slack != NULL) {
		delete[] slack;
		slack = NULL;
	}
	if(slackmy != NULL) {
		delete[] slackmy;
		slackmy = NULL;
	}
	if(q_matrix != NULL) {
		delete[] q_matrix;
		q_matrix = NULL;
	}
	if(elabels_map != NULL) {
		delete[] elabels_map;
		elabels_map = NULL;
	}
	if(vlabels_map != NULL) {
		delete[] vlabels_map;
		vlabels_map = NULL;
	}
	if(children != NULL) {
		delete[] children;
		children = NULL;
	}

	for(ui i = 0;i < states_memory.size();i ++) {
		delete[] states_memory[i];
		states_memory[i] = NULL;
	}
	states_memory.clear();
	for(ui i = 0;i < siblings_memory.size();i ++) {
		delete[] siblings_memory[i];
		siblings_memory[i] = NULL;
	}
	siblings_memory.clear();
}

void Application::init(const Graph *g, const Graph *q) {
	q_n = q->n;
	q_starts = new ui[q_n+1]; q_vlabels = new ui[q_n];
	q_edges = new ui[q->m]; q_elabels = new ui[q->m];
	memcpy(q_starts, q->pstarts, sizeof(ui)*(q_n+1));
	memcpy(q_vlabels, q->vlabels, sizeof(ui)*q_n);
	memcpy(q_edges, q->edges, sizeof(ui)*q->m);
	memcpy(q_elabels, q->elabels, sizeof(ui)*q->m);

	g_n = g->n;
	g_starts = new ui[g_n+1]; g_vlabels = new ui[g_n];
	g_edges = new ui[g->m]; g_elabels = new ui[g->m];
	memcpy(g_starts, g->pstarts, sizeof(ui)*(g_n+1));
	memcpy(g_vlabels, g->vlabels, sizeof(ui)*g_n);
	memcpy(g_edges, g->edges, sizeof(ui)*g->m);
	memcpy(g_elabels, g->elabels, sizeof(ui)*g->m);

	preprocess();
}

void Application::init(const std::vector<std::pair<int,int> > &g_v, const std::vector<std::pair<std::pair<int,int>,int> > &g_e, const std::vector<std::pair<int,int> > &q_v, const std::vector<std::pair<std::pair<int,int>,int> > &q_e) {
	q_n = q_v.size();
	q_starts = new ui[q_n+1]; q_vlabels = new ui[q_n];
	for(ui i = 0;i < q_n;i ++) q_vlabels[i] = q_v[i].second;

	q_edges = new ui[q_e.size()]; q_elabels = new ui[q_e.size()];
	ui idx = 0;
	for(ui i = 0;i < q_e.size();i ++) {
		q_edges[i] = q_e[i].first.second;
		q_elabels[i] = q_e[i].second;
		if(i == 0||q_e[i].first.first != q_e[i-1].first.first) {
			while(idx <= q_e[i].first.first) q_starts[idx ++] = i;
		}
	}
	while(idx <= q_n) q_starts[idx ++] = q_e.size();

	g_n = g_v.size();
	g_starts = new ui[g_n+1]; g_vlabels = new ui[g_n];
	for(ui i = 0;i < g_n;i ++) g_vlabels[i] = g_v[i].second;

	g_edges = new ui[g_e.size()]; g_elabels = new ui[g_e.size()];
	idx = 0;
	for(ui i = 0;i < g_e.size();i ++) {
		g_edges[i] = g_e[i].first.second;
		g_elabels[i] = g_e[i].second;
		if(i == 0||g_e[i].first.first != g_e[i-1].first.first) {
			while(idx <= g_e[i].first.first) g_starts[idx ++] = i;
		}
	}
	while(idx <= g_n) g_starts[idx ++] = g_e.size();

	preprocess();
}

void Application::preprocess() {
	q_g_swapped = false;
	if(q_n > g_n) {
#ifndef NDEBUG
		printf("q and g are swapped!\n");
#endif
		swap(q_n, g_n);
		swap(q_starts, g_starts);
		swap(q_edges, g_edges);
		swap(q_vlabels, g_vlabels);
		swap(q_elabels, g_elabels);
		q_g_swapped = true;
	}

	vlabels_n = relabel(q_n, q_vlabels, g_n, g_vlabels);
	elabels_n = relabel(q_starts[q_n], q_elabels, g_starts[g_n], g_elabels);

	if(vlabels_map != NULL) delete[] vlabels_map;
	vlabels_map = new int[vlabels_n];
	memset(vlabels_map, 0, sizeof(int)*vlabels_n);

	if(elabels_map != NULL) delete[] elabels_map;
	elabels_map = new int[elabels_n];
	memset(elabels_map, 0, sizeof(int)*elabels_n);

	if(visX != NULL) delete[] visX;
	visX = new char[g_n];
	memset(visX, 0, sizeof(char)*g_n);

	if(visY != NULL) delete[] visY;
	visY = new char[g_n];
	memset(visY, 0, sizeof(char)*g_n);

	// to handle large graphs, this should be replaced by a hash map
	if(q_matrix != NULL) delete[] q_matrix;
	q_matrix = new uchar[q_n*q_n];
	for(ui i = 0;i < q_n;i ++) {
		uchar *t_array = q_matrix+i*q_n;
		for(ui j = 0;j < q_n;j ++) t_array[j] = elabels_n;
		for(ui j = q_starts[i];j < q_starts[i+1];j ++) t_array[q_edges[j]] = q_elabels[j];
	}

	candidates = new ui[g_n];
	queue = new ui[g_n];
	prev = new ui[g_n];
	mx = new int[g_n];
	my = new int[g_n];
	BX = new ui[q_n];

	if(lb_method == BMa||lb_method == BMao) {
		cost = new ui[g_n*g_n];
		lx = new int[g_n];
		ly = new int[g_n];
		slack = new int[g_n];
		slackmy = new int[g_n];
	}
}

ui Application::relabel(ui len1, ui *array1, ui len2, ui *array2) {
	vector<ui> labels;
	labels.reserve(len1+len2);
	for(ui i = 0;i < len1;i ++) labels.pb(array1[i]);
	for(ui i = 0;i < len2;i ++) labels.pb(array2[i]);

	sort(labels.begin(), labels.end());
	ui labels_n = 1;
	for(ui i = 1;i < labels.size();i ++) if(labels[i] != labels[i-1]) {
		labels[labels_n ++] = labels[i];
	}

	for(ui i = 0;i < len1;i ++) array1[i] = search_index(array1[i], labels, labels_n);
	for(ui i = 0;i < len2;i ++) array2[i] = search_index(array2[i], labels, labels_n);

	return labels_n;
}

ui Application::search_index(ui val, std::vector<ui> &array, ui array_len) {
	assert(array[0] <= val);
	ui low = 0, high = array_len-1;
	while(low <= high) {
		ui mid = (low+high)/2;
		if(array[mid] == val) return mid;
		if(array[mid] < val) low = mid + 1;
		else high = mid - 1;
	}
	printf("%u is not in the array!\n", val);
	return 0;
}

ui Application::DFS(State *node) {
	if(MO == NULL) compute_mapping_order();

	ui start_level = 0;
	if(node != NULL) start_level = node->level;
	State **stack = NULL;
	if(search_n > start_level) stack = new State*[search_n - start_level];

	ui start = 0;
	if(node != NULL) {
		start = 1;
		stack[0] = node;
	}
	State *tt = NULL;
	if(search_n > start_level + start) tt = new State[search_n-start_level-start];
	for(ui i = start;i < search_n - start_level;i ++) {
		stack[i] = &tt[i-start];
#ifdef _EXPAND_ALL_
		stack[i]->siblings = NULL;
		if(g_n > i + start_level + 1) stack[i]->siblings = new ushort[(g_n - i - start_level - 1)*2];
#endif
	}

#ifndef _EXPAND_ALL_
	if(visited_siblings == NULL) {
		visited_siblings = new ushort[q_n*g_n];
		visited_siblings_n = new ushort[q_n];
	}
#endif

	if(node == NULL) generate_best_extension(NULL, stack[0]);

	State full;

#ifndef NDEBUG
	ui *cnt = new ui[q_n+g_n+q_starts[q_n]+g_starts[g_n]];
	memset(cnt, 0, sizeof(ui)*(q_n+g_n+q_starts[q_n]+g_starts[g_n]));
	ui total = 0;
#endif

	int idx = 0;
	while(idx >= 0&&(verify_upper_bound == INF||upper_bound > verify_upper_bound)) {
		if(stack[idx]->image == g_n||stack[idx]->lower_bound >= upper_bound) {
			-- idx;
			if(idx >= 0&&stack[idx]->lower_bound < upper_bound) {
#ifndef NDEBUG
				++ cnt[stack[idx]->lower_bound];
				++ total;
#endif
				construct_sibling(NULL, stack[idx]);
			}
			continue;
		}

		if(stack[idx]->level+1 == search_n) {
#ifndef NDEBUG
			++ cnt[stack[idx]->lower_bound];
			++ total;
#endif
			extend_to_full_mapping(stack[idx], &full);
			construct_sibling(NULL, stack[idx]);
		}
		else {
			generate_best_extension(stack[idx], stack[idx+1]);
			++ idx;
		}
	}

#ifndef NDEBUG
	ui smaller = 0, exact = cnt[upper_bound];
	for(ui i = 0;i < upper_bound;i ++) smaller += cnt[i];
	delete[] cnt; cnt = NULL;
	printf("Extended #states smaller: %d, equal: %d, larger: %d\n", smaller, exact, total-smaller-exact);
#endif

#ifdef _EXPAND_ALL_
	for(ui i = 0;i < search_n - start_level - start;i ++) if(tt[i].siblings != NULL) {
		if(tt[i].siblings != NULL) delete[] tt[i].siblings;
		tt[i].siblings = NULL;
	}
#endif
	if(tt != NULL) delete[] tt;
	tt = NULL;

	for(ui i = 0;i < search_n - start_level;i ++) stack[i] = NULL;
	if(stack != NULL) delete[] stack;
	stack = NULL;

	return upper_bound;
}

ui Application::AStar(vector<pair<ui,ui> > *mapping_ptr, int *lb_ptr) {
	if(MO == NULL) compute_mapping_order();
	State *root = get_a_new_state_node();
	root->cs_cnt = 0;
#ifdef _EXPAND_ALL_
	root->siblings = get_a_new_siblings_node();
#endif
	generate_best_extension(NULL, root);
	vector<State*> heap;
	ui heap_n = 1;
	heap.pb(root);

	if(mapping_ptr != NULL&&lb_ptr == NULL) printf("WA input for AStar\n");

	State full;
#ifndef NDEBUG
	ui *cnt = new ui[q_n+g_n+q_starts[q_n]+g_starts[g_n]];
	memset(cnt, 0, sizeof(ui)*(q_n+g_n+q_starts[q_n]+g_starts[g_n]));
	ui total = 0;
#endif
	while(heap_n > 0&&heap[0]->lower_bound < upper_bound&&(verify_upper_bound == INF||upper_bound > verify_upper_bound)) {
		State *now = heap[0];
		heap[0] = heap[-- heap_n];
		heap_top_down(0, heap_n, heap);
#ifndef NDEBUG
		++ cnt[now->lower_bound];
		++ total;
#endif

		if(mapping_ptr != NULL&&(*mapping_ptr).size() == now->level+1) {
			bool ok = true;
			State *tmp = now;
			for(ui i = 0;i <= now->level;i ++) if(MO[i] != (*mapping_ptr)[i].first) printf("WA matching order in AStar\n");

			for(ui i = 0;i <= now->level&&ok;i ++) {
				if(tmp == NULL) printf("WA in AStar\n");
				if((*mapping_ptr)[now->level-i].second != tmp->image) ok = false;
				tmp = tmp->parent;
			}

			if(ok) {
				*lb_ptr = now->lower_bound;
				break;
			}
		}

		State *sibling = get_a_new_state_node();
		sibling->cs_cnt = 0;
#ifdef _EXPAND_ALL_
		sibling->siblings = now->siblings;
		sibling->siblings_n = now->siblings_n;
		now->siblings = NULL;
		now->siblings_n = 0;
		sibling->parent = now->parent;
		sibling->level = now->level;
		construct_sibling(NULL, sibling);
#else
		construct_sibling(now, sibling);
		++ now->cs_cnt;
#endif
		if(sibling->image < g_n&&sibling->lower_bound < upper_bound) {
			add_to_heap(sibling, heap_n, heap);
			if(now->parent != NULL) ++ now->parent->cs_cnt;
		}
		else {
			put_a_state_to_pool(sibling);
#ifdef _EXPAND_ALL_
			put_a_sibling_to_pool(sibling->siblings);
			sibling->siblings = NULL;
#else
			-- now->cs_cnt;
#endif
		}

		if(now->level+1 == search_n) extend_to_full_mapping(now, &full);
		else {
			State *child = get_a_new_state_node();
			child->cs_cnt = 0;
#ifdef _EXPAND_ALL_
			child->siblings = get_a_new_siblings_node();
#endif
			generate_best_extension(now, child);
			now->cs_cnt ++;

			if(child->image < g_n&&child->lower_bound < upper_bound) add_to_heap(child, heap_n, heap);
			else {
#ifdef _EXPAND_ALL_
				put_a_sibling_to_pool(child->siblings);
				child->siblings = NULL;
#endif
				put_a_state_to_pool(child);
				-- now->cs_cnt;
			}
		}

		assert(now->cs_cnt >= 0);
		if(now->cs_cnt == 0) add_to_pool(now);

		while(heap_n > 0&&heap[heap_n-1]->lower_bound >= upper_bound) {
			-- heap_n;
			if(heap[heap_n]->cs_cnt == 0) add_to_pool(heap[heap_n]);
		}
	}
#ifndef NDEBUG
	ui smaller = 0, exact = cnt[upper_bound];
	for(ui i = 0;i < upper_bound;i ++) smaller += cnt[i];
	delete[] cnt; cnt = NULL;
	printf("Extended #states smaller: %d, equal: %d, larger: %d\n", smaller, exact, total-smaller-exact);
#endif

	if(mapping_ptr != NULL&&(*lb_ptr) < 0) {
		if(heap_n > 0&&heap[0]->lower_bound < upper_bound) printf("WA in AStar %u %u\n", heap[0]->lower_bound, upper_bound);
		*lb_ptr = upper_bound;
	}

	return upper_bound;
}

inline void Application::add_to_pool(State *st) {
	put_a_state_to_pool(st);
	assert(states_pool[states_pool_n-1] == st);
	for(ui i = states_pool_n-1;i < states_pool_n;i ++) {
		st = states_pool[i];
#ifdef _EXPAND_ALL_
		if(st->siblings != NULL) {
			put_a_sibling_to_pool(st->siblings);
			st->siblings = NULL;
		}
#endif
		if(st->pre_sibling != NULL) {
			assert(st->pre_sibling->cs_cnt > 0);
			if((-- st->pre_sibling->cs_cnt) == 0) {
				put_a_state_to_pool(st->pre_sibling);
				st->pre_sibling = NULL;
			}
		}
		if(st->parent != NULL) {
			//State *ss = st->parent;
			//printf("level %d, (->%d), mapp_cost: %d, lower_bound: %d, cs_cnt: %d\n", ss->level, ss->image, ss->mapped_cost, ss->lower_bound, ss->cs_cnt);
			assert(st->parent->cs_cnt > 0);
			if((-- st->parent->cs_cnt) == 0) {
				put_a_state_to_pool(st->parent);
				st->parent = NULL;
			}
		}
	}
}

inline void Application::add_to_heap(State *st, ui &heap_n, vector<State*> &heap) {
	if(heap.size() == heap_n) heap.pb(st);
	else heap[heap_n] = st;
	++ heap_n;
	heap_bottom_up(heap_n-1, heap);
}

inline State* Application::get_a_new_state_node() {
	if(states_pool_n == 0) {
		State *new_block = new State[block_size];
		for(ui i = 0;i < block_size;i ++) {
			if(states_pool.size() == states_pool_n) states_pool.pb(&new_block[i]);
			else states_pool[states_pool_n] = &new_block[i];
			++ states_pool_n;
		}
		states_memory.pb(new_block);
	}
	assert(states_pool_n > 0);
	-- states_pool_n;
	return states_pool[states_pool_n];
}

inline void Application::put_a_state_to_pool(State *st) {
	if(states_pool_n == states_pool.size()) states_pool.pb(st);
	else states_pool[states_pool_n] = st;
	++ states_pool_n;
}

inline ushort* Application::get_a_new_siblings_node() {
	if(siblings_pool_n == 0) {
		ui t_block_size = (block_size*8)/g_n+1;
		ushort *new_block = new ushort[t_block_size*2*g_n];
		for(ui i = 0;i < t_block_size;i ++) {
			if(siblings_pool.size() == siblings_pool_n) siblings_pool.pb(new_block+i*2*g_n);
			else siblings_pool[siblings_pool_n] = new_block+i*2*g_n;
			++ siblings_pool_n;
		}
		siblings_memory.pb(new_block);
	}
	assert(siblings_pool_n > 0);
	-- siblings_pool_n;
	return siblings_pool[siblings_pool_n];
}

inline void Application::put_a_sibling_to_pool(ushort *sibling) {
	if(siblings_pool_n == siblings_pool.size()) siblings_pool.pb(sibling);
	else siblings_pool[siblings_pool_n] = sibling;
	++ siblings_pool_n;
}

void Application::compute_mapping_order() {
	if(MO == NULL) MO = new ui[q_n];

	ui g_m = g_starts[g_n];
	ui *vlabels_cnt = new ui[vlabels_n];
	ui *elabels_cnt = new ui[elabels_n];
	memset(vlabels_cnt, 0, sizeof(ui)*vlabels_n);
	memset(elabels_cnt, 0, sizeof(ui)*elabels_n);

	for(ui i = 0;i < g_n;i ++) {
		++ vlabels_cnt[g_vlabels[i]];
		for(ui j = g_starts[i];j < g_starts[i+1];j ++) ++ elabels_cnt[g_elabels[j]];
	}

	ui root = 0;
	double root_weight = 0;
	for(ui i = 0;i < q_n;i ++) {
		double weight = 1 - vlabels_cnt[q_vlabels[i]]/double(g_n);
		for(ui j = q_starts[i];j < q_starts[i+1];j ++) {
			weight += 1 - elabels_cnt[q_elabels[j]]/double(g_m);
		}
		if(weight > root_weight) {
			root = i;
			root_weight = weight;
		}
	}

	vector<pair<double, int> > heap(q_n);
	for(ui i = 0;i < q_n;i ++) {
		if(i == root) heap[i].first = root_weight;
		else heap[i].first = 0;
		heap[i].second = i;
	}
	swap(heap[0], heap[root]);
	ui *pos = new ui[q_n];
	for(ui i = 0;i < q_n;i ++) pos[heap[i].second] = i;

	ui heap_n = q_n;
	for(ui i = 0;i < q_n;i ++) {
		ui u = heap[0].second;
		MO[i] = u;
		pos[u] = heap_n-1;
		heap[0] = heap[-- heap_n];
		pos[heap[0].second] = 0;
		heap_top_down(0, heap_n, heap, pos);
		for(ui j = q_starts[u];j < q_starts[u+1];j ++) if(pos[q_edges[j]] < heap_n) {
			ui idx = pos[q_edges[j]];
			if(heap[idx].first < EPS) heap[idx].first += 1 - vlabels_cnt[q_vlabels[q_edges[j]]]/double(g_n);
			heap[idx].first += 1 - elabels_cnt[q_elabels[j]]/double(g_m);
			heap_bottom_up(idx, heap, pos);
		}
	}

#ifndef NDEBUG
	//for(ui i = 0;i < q_n;i ++) MO[i] = i;
#endif

	for(ui i = 0;i < q_n;i ++) pos[MO[i]] = i;
	ui edges_cnt = 0;
	for(ui i = 0;i < q_n;i ++) {
		ui u = MO[i];
		for(ui j = q_starts[u];j < q_starts[u+1];j ++) if(pos[q_edges[j]] > i) edges_cnt += 2;
		//if(edges_cnt >= q_starts[q_n]) printf("%d %d\n", edges_cnt, q_starts[q_n]);
		if(edges_cnt == q_starts[q_n]) {
			search_n = i+1;
			break;
		}
	}

	search_n_for_IS = search_n;

	if(lb_method != BMao) search_n = q_n;

#ifndef NDEBUG
	printf("Matching order: (%d", MO[0]);
	for(ui i = 1;i < q_n;i ++) printf(", %d", MO[i]);
	printf(")\n");
	printf("Independent set size: %u\n", q_n - search_n);
#endif

	delete[] pos;
	delete[] vlabels_cnt;
	delete[] elabels_cnt;
}

void Application::heap_top_down(ui idx, ui heap_n, vector<pair<double, int> > &heap, ui *pos) {
	pair<double, int> tmp = heap[idx];
	while(2*idx+1 < heap_n) {
		ui i = 2*idx+1;
		if(i+1 < heap_n&&heap[i+1].first > heap[i].first) ++ i;
		if(heap[i].first > tmp.first) {
			heap[idx] = heap[i];
			pos[heap[idx].second] = idx;
			idx = i;
		}
		else break;
	}
	heap[idx] = tmp;
	pos[tmp.second] = idx;
}

void Application::heap_top_down(ui idx, ui heap_n, vector<State*> &heap) {
	State *tmp = heap[idx];
	while(2*idx+1 < heap_n) {
		ui i = 2*idx+1;
		if(i+1 < heap_n) {
			if(heap[i+1]->lower_bound < heap[i]->lower_bound) ++ i;
			else if(heap[i+1]->lower_bound == heap[i]->lower_bound&&heap[i+1]->level > heap[i]->level) ++ i;
		}
		if(heap[i]->lower_bound < tmp->lower_bound||(heap[i]->lower_bound == tmp->lower_bound&&heap[i]->level > tmp->level)) {
			heap[idx] = heap[i];
			idx = i;
		}
		else break;
	}
	heap[idx] = tmp;
}

void Application::heap_bottom_up(ui idx, vector<pair<double,int> > &heap, ui *pos) {
	pair<double, int> tmp = heap[idx];
	while(idx > 0) {
		ui i = (idx-1)/2;
		if(heap[i].first < tmp.first) {
			heap[idx] = heap[i];
			pos[heap[idx].second] = idx;
			idx = i;
		}
		else break;
	}
	heap[idx] = tmp;
	pos[tmp.second] = idx;
}

void Application::heap_bottom_up(ui idx, vector<State*> &heap) {
	State *tmp = heap[idx];
	while(idx > 0) {
		ui i = (idx-1)/2;
		if(heap[i]->lower_bound > tmp->lower_bound||(heap[i]->lower_bound == tmp->lower_bound&&heap[i]->level < tmp->level)) {
			heap[idx] = heap[i];
			idx = i;
		}
		else break;
	}
	heap[idx] = tmp;
}

void Application::generate_best_extension(State *parent, State *now) {
	if(parent != NULL) {
		now->level = parent->level + 1;
		now->mapped_cost = parent->mapped_cost;
	}
	else {
		now->level = 0;
		now->mapped_cost = 0;
	}
	now->parent = parent;
	now->pre_sibling = NULL;

	while(parent != NULL) {
		visY[parent->image] = 1;
		parent = parent->parent;
	}
	ui candidates_n = 0;
	for(ui i = 0;i < g_n;i ++) {
		if(!visY[i]) candidates[candidates_n ++] = i;
		else visY[i] = 0;
	}
	ui pre_siblings = 0;

	if(visited_siblings_n != NULL) visited_siblings_n[now->level] = 0;
	if(children == NULL) children = new pair<int,int>[g_n];

	if(lb_method == LSa) compute_best_extension_LSa(now, candidates_n, candidates, pre_siblings);
	else if(lb_method == BMao) compute_best_extension_BM(1, now, candidates_n, candidates, pre_siblings, 0);
	else if(lb_method == BMa) compute_best_extension_BMa(1, now, candidates_n, candidates, pre_siblings);
	else printf("lb_method is wrong in (generate_best_extension)!\n");
}

void Application::compute_best_extension_LSa(State *now, ui n, ui *candidates, ui pre_siblings) {
#ifndef NDEBUG
	for(ui i = 0;i < g_n;i ++) {
		assert(visX[i] == 0);
		assert(visY[i] == 0);
	}
	if(vlabels_map != NULL) for(ui i = 0;i < vlabels_n;i ++) assert(vlabels_map[i] == 0);
	for(ui i = 0;i < elabels_n;i ++) assert(elabels_map[i] == 0);
	if(elabels_matrix != NULL) for(ui i = 0;i < q_n*elabels_n;i ++) assert(elabels_matrix[i] == 0);
#endif
	if(vlabels_map == NULL) {
		vlabels_map = new int[vlabels_n];
		memset(vlabels_map, 0, sizeof(int)*vlabels_n);
	}
	if(elabels_matrix == NULL) {
		elabels_matrix = new short[q_n*elabels_n];
		memset(elabels_matrix, 0, sizeof(short)*q_n*elabels_n);
	}

	ui *sizeX = queue;
	ui *sizeY = prev;

	ui idx = now->level;
	ui u = MO[idx], u_cross_cnt = 0, u_inner_cnt = 0;
	for(ui i = 0;i <= idx;i ++) visX[MO[i]] = 1;
	for(ui i = 0;i <= idx;i ++) {
		ui v = MO[i];
		sizeX[v] = 0;
		for(ui j = q_starts[v];j < q_starts[v+1];j ++) if(!visX[q_edges[j]]) {
			++ sizeX[v];
			-- elabels_matrix[v*elabels_n+q_elabels[j]];
		}
	}
	for(ui i = q_starts[u];i < q_starts[u+1];i ++) if(visX[q_edges[i]]) ++ u_cross_cnt;
	for(ui i = idx+1;i < q_n;i ++) for(ui j = q_starts[MO[i]];j < q_starts[MO[i]+1];j ++) {
		ui uprime = q_edges[j];
		if(!visX[uprime]&&MO[i] < uprime) {
			-- elabels_map[q_elabels[j]];
			++ u_inner_cnt;
		}
	}
	for(ui i = idx+1;i < q_n;i ++) -- vlabels_map[q_vlabels[MO[i]]];
	for(ui i = 0;i <= idx;i ++) visX[MO[i]] = 0;

	for(State *parent = now->parent;parent != NULL;parent = parent->parent) {
		visY[parent->image] = 1;
		my[parent->image] = MO[parent->level];
	}

	ui ged_common = 0;
	for(State *st = now->parent;st != NULL;st = st->parent) {
		ui v = st->image;
		ui common = 0;
		sizeY[st->image] = 0;
		for(ui j = g_starts[v];j < g_starts[v+1];j ++) if(!visY[g_edges[j]]) {
			short &cnt = elabels_matrix[MO[st->level]*elabels_n + g_elabels[j]];
			if(cnt < 0) ++ common;
			++ cnt;
			++ sizeY[st->image];
		}
		ui t_ged = sizeX[MO[st->level]];
		if(sizeY[st->image] > t_ged) t_ged = sizeY[st->image];
		ged_common += t_ged - common;
	}

	ui vl_common = 0, el_common = 0, v_total_cnt = 0;
	for(ui i = 0;i < n;i ++) {
		ui v = candidates[i];
		if(vlabels_map[g_vlabels[v]] < 0) ++ vl_common;
		++ vlabels_map[g_vlabels[v]];
		for(ui j = g_starts[v];j < g_starts[v+1];j ++) {
			ui vprime = g_edges[j];
			if(!visY[vprime]&&v < vprime) {
				if(elabels_map[g_elabels[j]] < 0) ++ el_common;
				++ elabels_map[g_elabels[j]];
				++ v_total_cnt;
			}
		}
	}

	ui mapped_cost = 0;
	if(now->parent != NULL) mapped_cost = now->parent->mapped_cost;

	ui vl_ged = q_n-idx-1;
	if(g_n > q_n) vl_ged = g_n-idx-1;
	vl_ged -= vl_common;

	if(children == NULL) children = new pair<int,int>[g_n];

	for(ui i = pre_siblings;i < n;i ++) {
		ui v = candidates[i];
		ui lb = vl_ged + ged_common;
		if(vlabels_map[g_vlabels[v]] <= 0) ++ lb;
		//if(v == 3) printf("%d %d\n", ged_common, lb-ged_common);

		ui cross_ged = u_cross_cnt, inner_ged = v_total_cnt;
		if(g_vlabels[v] != q_vlabels[u]) ++ cross_ged;
		ui common = 0, t_size = 0;
		for(ui j = g_starts[v];j < g_starts[v+1];j ++) {
			ui vprime = g_edges[j];
			if(visY[vprime]) {
				++ cross_ged;
				if(q_matrix[u*q_n+my[vprime]] == g_elabels[j]) cross_ged -= 2;
				else if(q_matrix[u*q_n+my[vprime]] < elabels_n) -- cross_ged;

				if(elabels_matrix[my[vprime]*elabels_n + g_elabels[j]] < 0) ++ lb;
				if(sizeY[vprime] > sizeX[my[vprime]]) -- lb;
				//if(v == 3) printf("h %d %d %d\n", sizeY[vprime], sizeX[my[vprime]], lb);
				continue;
			}

			-- inner_ged;
			if(elabels_map[g_elabels[j]] <= 0) -- el_common;
			-- elabels_map[g_elabels[j]];

			++ t_size;
			//printf("t_size: %d (%d)\n", t_size, vprime);
			if(elabels_matrix[u*elabels_n + g_elabels[j]] < 0) ++ common;
			++ elabels_matrix[u*elabels_n + g_elabels[j]];
		}
		if(sizeX[u] > t_size) t_size = sizeX[u];
		//printf("sizeX[u]: %d, t_size: %d, common: %d\n", sizeX[u], t_size, common);
		lb += t_size - common;

		if(u_inner_cnt > inner_ged) inner_ged = u_inner_cnt;
		inner_ged -= el_common;
		//if(v == 3) printf("%d %d %d\n", cross_ged, inner_ged, lb);
		children[i-pre_siblings].first = - mapped_cost - cross_ged - inner_ged - lb;
		children[i-pre_siblings].second = v;

		//printf("(%d->%d) lower_bound: %d, mapped_cost: %d\n", u, v, -children[i-pre_siblings].first, mapped_cost);

		//if(v == 3) printf("mapped_cost: %d, cross_ged: %d, inner_ged: %d, v_ged: %d\n", mapped_cost, cross_ged, inner_ged, lb);

		for(ui j = g_starts[v];j < g_starts[v+1];j ++) if(!visY[g_edges[j]]) {
			if(elabels_map[g_elabels[j]] < 0) ++ el_common;
			++ elabels_map[g_elabels[j]];

			-- elabels_matrix[u*elabels_n + g_elabels[j]];
		}
	}
	sort(children, children + n-pre_siblings);
	now->image = children[n-pre_siblings-1].second;
	now->lower_bound = -children[n-pre_siblings-1].first;

	ui cross_ged = u_cross_cnt;
	for(ui j = g_starts[now->image];j < g_starts[now->image+1];j ++) if(visY[g_edges[j]]) {
		++ cross_ged;
		if(q_matrix[u*q_n+my[g_edges[j]]] == g_elabels[j]) cross_ged -= 2;
		else if(q_matrix[u*q_n+my[g_edges[j]]] < elabels_n) -- cross_ged;
	}
	if(q_vlabels[u] != g_vlabels[now->image]) ++ cross_ged;
	now->mapped_cost = mapped_cost + cross_ged;

#ifndef NDEBUG
	verify_induced_ged(now);
#endif

	for(ui i = 0;i <= idx;i ++) {
		ui v = MO[i];
		for(ui j = q_starts[v];j < q_starts[v+1];j ++) elabels_matrix[v*elabels_n + q_elabels[j]] = 0;
	}

	for(State *st = now->parent;st != NULL;st = st->parent) {
		ui v = st->image;
		for(ui j = g_starts[v];j < g_starts[v+1];j ++) elabels_matrix[my[v]*elabels_n + g_elabels[j]] = 0;
	}

	for(ui i = 0;i < n;i ++) {
		vlabels_map[g_vlabels[candidates[i]]] = 0;
		for(ui j = g_starts[candidates[i]];j < g_starts[candidates[i]+1];j ++) elabels_map[g_elabels[j]] = 0;
	}
	for(ui i = idx+1;i < q_n;i ++) {
		vlabels_map[q_vlabels[MO[i]]] = 0;
		for(ui j = q_starts[MO[i]];j < q_starts[MO[i]+1];j ++) elabels_map[q_elabels[j]] = 0;
	}
	for(State *parent = now->parent;parent != NULL;parent = parent->parent) visY[parent->image] = 0;

	++ search_space;

	if(now->lower_bound >= upper_bound||now->lower_bound > verify_upper_bound) {
		now->image = g_n;
		return ;
	}

#ifdef _EXPAND_ALL_
	ushort *t_array = now->siblings;
	now->siblings_n = 0;
	for(ui i = 0;i < n-pre_siblings-1;i ++) {
		t_array[now->siblings_n++] = children[i].second;
		t_array[now->siblings_n++] = -children[i].first;
	}
#endif
}

void Application::compute_best_extension_BM(char anchor_aware, State *now, ui n, ui *candidates, ui pre_siblings, char no_siblings, char IS) {
#ifndef NDEBUG
	for(ui i = 0;i < g_n;i ++) {
		assert(visX[i] == 0);
		assert(visY[i] == 0);
	}
#endif
	for(ui i = 0;i < now->level;i ++) visX[MO[i]] = 1;
	for(State *parent = now->parent;parent != NULL;parent = parent->parent) {
		visY[parent->image] = 1;
		my[parent->image] = MO[parent->level];
	}

	ui start_idx = now->level;
	for(ui i = 0;i < n;i ++) {
		ui *t_array = cost + i*n;
		if(start_idx + i >= q_n) {
			for(ui j = 0;j < n;j ++) {
				ui &cot = t_array[j];
				cot = 2;
				for(ui k = g_starts[candidates[j]];k < g_starts[candidates[j]+1];k ++) {
					if(visY[g_edges[k]]&&anchor_aware) cot += 2;
					else ++ cot;
				}
			}
			continue;
		}

		ui u = MO[start_idx+i];
		ui anchored_edges = 0;
		for(ui j = q_starts[u];j < q_starts[u+1];j ++) {
			if(anchor_aware&&visX[q_edges[j]]) ++ anchored_edges;
			else ++ elabels_map[q_elabels[j]];
		}
		for(ui j = 0;j < n;j ++) {
			if(i == 0&&j < pre_siblings) {
				t_array[j] = INF;
				continue;
			}
			ui v = candidates[j];
			ui &cot = t_array[j];
			if(anchor_aware) cot = anchored_edges;
			else cot = 0;
			ui total = 0, common = 0;
			ui queue_n = 0;
			//if(n == 4&&i == 3&&now->parent->image == 3) printf("%d cot: %d\n", j, cot);
			for(ui k = g_starts[v];k < g_starts[v+1];k ++) {
				if(anchor_aware&&visY[g_edges[k]]) {
					++ cot;
					if(q_matrix[u*q_n+my[g_edges[k]]] == g_elabels[k]) cot -= 2;
					else if(q_matrix[u*q_n+my[g_edges[k]]] < elabels_n) -- cot;
				}
				else {
					++ total;
					if(elabels_map[g_elabels[k]]) {
						++ common;
						-- elabels_map[g_elabels[k]];
						queue[queue_n ++] = g_elabels[k];
					}
				}
			}
			cot *= 2;
			//if(n == 4&&i == 3&&now->parent->image == 3) printf("%d: %d, %d, %d\n", candidates[j], cot, total, common);
			if(q_starts[u+1] - q_starts[u] - anchored_edges*anchor_aware > total) total = q_starts[u+1] - q_starts[u] - anchored_edges*anchor_aware;
			cot += total - common;
			for(ui k = 0;k < queue_n;k ++) ++ elabels_map[queue[k]];

			if(q_vlabels[u] != g_vlabels[v]) cot += 2;
		}
		for(ui j = q_starts[u];j < q_starts[u+1];j ++) elabels_map[q_elabels[j]] = 0;
	}
	for(State *parent = now->parent;parent != NULL;parent = parent->parent) visY[parent->image] = 0;
	for(ui i = 0;i < now->level;i ++) visX[MO[i]] = 0;

	if(!IS) ++ search_space;

#ifndef NDEBUG
	//if(n < q_n) printf("%d ", now->parent->image);
	//printf("candidates: %d:", n);
	//for(ui i = 0;i < n;i ++) printf(" %d", candidates[i]);
	//printf("\n");
#endif

	now->lower_bound = now->mapped_cost + (Hungarian(1, n, cost)+1)/2;
	if(now->lower_bound >= upper_bound||now->lower_bound > verify_upper_bound) {
		now->image = g_n;
		return ;
	}
	now->image = candidates[mx[0]];

#ifndef NDEBUG
	//printf("Lower bound (%d-%d): %d\n", MO[now->level], candidates[mx[0]], now->lower_bound);
#endif

	if(IS) {
		if(now->lower_bound < upper_bound) {
			upper_bound = now->lower_bound;
			for(State *st = now->parent;st != NULL;st = st->parent) BX[MO[st->level]] = st->image;
			for(ui i = 0;i+now->level < q_n;i ++) BX[MO[i+now->level]] = candidates[mx[i]];

#ifndef NDEBUG
			//printf("supposed GED in IS: %d\n", upper_bound);
			assert(upper_bound == compute_ged_of_BX());
#endif
		}
		return ;
	}

	compute_mapped_cost_and_upper_bound(now, n, candidates, mx);

	if(no_siblings) return ;

#ifdef _EXPAND_ALL_
	ui p_mapping_cost = 0;
	if(now->parent != NULL) p_mapping_cost = now->parent->mapped_cost;

	ushort *t_array = now->siblings;
	now->siblings_n = 0;
	for(ui i = 1;i < n;i ++) {
		//++ search_space;
		cost[mx[0]] = INF; my[mx[0]] = -1; mx[0] = -1;
		ui lb = p_mapping_cost + (Hungarian(0, n, cost)+1)/2;
#ifdef _EARLY_STOP_
		if(lb >= upper_bound||lb > verify_upper_bound) break;
#endif

		t_array[now->siblings_n++] = candidates[mx[0]];
		t_array[now->siblings_n++] = lb;
	}
	ui tn = now->siblings_n;
	for(ui i = 0;i + 2 < tn;i += 2) {
		swap(t_array[i], t_array[tn-2]);
		swap(t_array[i+1], t_array[tn-1]);
		tn -= 2;
	}
#endif
}

void Application::compute_best_extension_BMa(char anchor_aware, State *now, ui n, ui *candidates, ui pre_siblings) {
	State tmp;
	tmp.parent = now;
	tmp.level = now->level+1;
	tmp.pre_sibling = NULL;

	long long old_search_space = search_space;

	if(children == NULL) children = new pair<int,int>[g_n];
	for(ui i = pre_siblings;i < n;i ++) {
		now->image = candidates[i];
		swap(candidates[i], candidates[n-1]);

		compute_mapped_cost(now);
		tmp.mapped_cost = now->mapped_cost;
		compute_best_extension_BM(anchor_aware, &tmp, n-1, candidates, 0, 1);
		children[i-pre_siblings].first = -tmp.lower_bound;
		children[i-pre_siblings].second = now->image;

		swap(candidates[i], candidates[n-1]);
	}

	search_space = old_search_space + 1;

	sort(children, children + n-pre_siblings);
	now->image = children[n-pre_siblings-1].second;
	now->lower_bound = -children[n-pre_siblings-1].first;
	compute_mapped_cost(now);

	if(now->lower_bound >= upper_bound||now->lower_bound > verify_upper_bound) {
		now->image = g_n;
		return ;
	}

#ifdef _EXPAND_ALL_
	ushort *t_array = now->siblings;
	now->siblings_n = 0;
	for(ui i = 0;i < n-pre_siblings-1;i ++) {
		//++ search_space;
		t_array[now->siblings_n++] = children[i].second;
		t_array[now->siblings_n++] = -children[i].first;
	}
#endif
}

void Application::compute_mapped_cost_and_upper_bound(State *now, ui n, ui *candidates, int *mapping) {
	ui cot = now->mapped_cost;
	ui start_level = now->level;

#ifndef NDEBUG
	for(ui i = 0;i < g_n;i ++) {
		assert(visX[i] == 0);
		assert(visY[i] == 0);
	}
#endif

	assert((now->parent == NULL&&cot == 0)||(now->parent != NULL&&cot == now->parent->mapped_cost));
	for(State *parent = now->parent;parent != NULL;parent = parent->parent) {
		visY[parent->image] = 1;
		queue[parent->image] = MO[parent->level];
	}
	for(ui i = 0;i < start_level;i ++) visX[MO[i]] = 1;

	//printf("mapped_cost: %d, start_level: %d, n: %d\n", cot, start_level, n);
	//if(start_level == 1) printf("%d %d\n", q_vlabels[MO[0]], g_vlabels[now->parent->image]);

	for(ui i = 0;i < n;i ++) {
		if(i+start_level >= q_n) {
			ui v = candidates[mapping[i]];
			++ cot;
			for(ui j = g_starts[v];j < g_starts[v+1];j ++) if(visY[g_edges[j]]) ++ cot;
			visY[v] = 1;
			//printf("here\n");
			continue;
		}
		ui u = MO[i+start_level], v = candidates[mapping[i]];
		if(q_vlabels[u] != g_vlabels[v]) ++ cot;
		for(ui j = q_starts[u];j < q_starts[u+1];j ++) if(visX[q_edges[j]]) ++ cot;
		for(ui j = g_starts[v];j < g_starts[v+1];j ++) if(visY[g_edges[j]]) {
			++ cot;
			if(q_matrix[u*q_n+queue[g_edges[j]]] == g_elabels[j]) cot -= 2;
			else if(q_matrix[u*q_n+queue[g_edges[j]]] < elabels_n) -- cot;
		}
		if(i == 0) now->mapped_cost = cot;
		visX[u] = visY[v] = 1;
		queue[v] = u;
	}

#ifndef NDEBUG
	verify_induced_ged(now);
#endif

	memset(visX, 0, sizeof(char)*g_n);
	memset(visY, 0, sizeof(char)*g_n);

#ifndef NDEBUG
	//for(State *parent = now->parent;parent != NULL;parent = parent->parent) {
	//	queue[parent->level] = parent->image;
	//}
	//printf("Map (");
	//for(ui i = 0;i < start_level;i ++) printf("%d, ", queue[i]);
	//printf("%d;", now->image);
	//for(ui i = 1;i < n;i ++) printf(" %d", candidates[mapping[i]]);
	//printf("): cost %d, lower_bound %d\n", cot, now->lower_bound);
#endif

#ifdef _UPPER_BOUND_
	if(cot < upper_bound) {
		upper_bound = cot;
		for(State *st = now->parent;st != NULL;st = st->parent) BX[MO[st->level]] = st->image;
		for(ui i = 0;i+now->level < q_n;i ++) BX[MO[i+now->level]] = candidates[mapping[i]];

		//printf("supposed GED in upper_bound: %d (level: %d)\n", upper_bound, now->level);
		assert(upper_bound == compute_ged_of_BX());
	}
#endif
}

void Application::compute_mapped_cost(State *now) {
#ifndef NDEBUG
	for(ui i = 0;i < g_n;i ++) {
		assert(visX[i] == 0);
		assert(visY[i] == 0);
	}
#endif

	ui cot = 0;
	if(now->parent != NULL) cot = now->parent->mapped_cost;
	for(State *parent = now->parent;parent != NULL;parent = parent->parent) {
		visY[parent->image] = 1;
		my[parent->image] = MO[parent->level];
	}
	for(ui i = 0;i < now->level;i ++) visX[MO[i]] = 1;

	ui u = MO[now->level], v = now->image;
	if(q_vlabels[u] != g_vlabels[v]) ++ cot;
	for(ui j = q_starts[u];j < q_starts[u+1];j ++) if(visX[q_edges[j]]) ++ cot;
	for(ui j = g_starts[v];j < g_starts[v+1];j ++) if(visY[g_edges[j]]) {
		++ cot;
		if(q_matrix[u*q_n+my[g_edges[j]]] == g_elabels[j]) cot -= 2;
		else if(q_matrix[u*q_n+my[g_edges[j]]] < elabels_n) -- cot;
	}
	now->mapped_cost = cot;

	for(State *parent = now->parent;parent != NULL;parent = parent->parent) visY[parent->image] = 0;
	for(ui i = 0;i < now->level;i ++) visX[MO[i]] = 0;

#ifndef NDEBUG
	verify_induced_ged(now);
#endif
}

void Application::construct_sibling(State *pre_sibling, State *now) {
	now->pre_sibling = pre_sibling;
#ifdef _EXPAND_ALL_
	ushort &tn = now->siblings_n;
	if(tn == 0) now->image = g_n;
	else {
		now->image = now->siblings[tn-2];
		now->lower_bound = now->siblings[tn-1];
		compute_mapped_cost(now);
		tn -= 2;
#ifndef NDEBUG
		if(lb_method == LS) verify_LS_lower_bound(now);
#endif
	}
#else
	if(pre_sibling != NULL) {
		now->parent = pre_sibling->parent;
		now->level = pre_sibling->level;
	}

	if(now->parent != NULL) now->mapped_cost = now->parent->mapped_cost;
	else now->mapped_cost = 0;

	ui pre_siblings = 0;
	if(pre_sibling == NULL) {
		assert(visited_siblings_n != NULL);
		visited_siblings[now->level*g_n + (visited_siblings_n[now->level] ++)] = now->image;
		for(ui i = 0;i < visited_siblings_n[now->level];i ++) {
			candidates[pre_siblings ++] = visited_siblings[now->level*g_n+i];
		}
	}
	else {
		for(State *st = pre_sibling;st != NULL;st = st->pre_sibling) {
			candidates[pre_siblings ++] = st->image;
		}
	}

	for(ui i = 0;i < pre_siblings;i ++) visY[candidates[i]] = 1;
	for(State *st = now->parent;st != NULL;st = st->parent) visY[st->image] = 1;
	ui candidates_n = pre_siblings;
	for(ui i = 0;i < g_n;i ++) {
		if(!visY[i]) candidates[candidates_n ++] = i;
		else visY[i] = 0;
	}
	if(candidates_n <= pre_siblings) {
		now->image = g_n;
		return ;
	}

	if(lb_method == LS) compute_best_extension_LS(now, candidates_n, candidates, pre_siblings);
	else if(lb_method == BM) compute_best_extension_BM(0, now, candidates_n, candidates, pre_siblings, 0);
	else if(lb_method == SM) compute_best_extension_SM(0, now, candidates_n, candidates, pre_siblings);
	else if(lb_method == LSa) compute_best_extension_LSa(now, candidates_n, candidates, pre_siblings);
	else if(lb_method == BMao) compute_best_extension_BM(1, now, candidates_n, candidates, pre_siblings, 0);
	else if(lb_method == SMa) compute_best_extension_SM(1, now, candidates_n, candidates, pre_siblings);
	else if(lb_method == BMa) compute_best_extension_BMa(1, now, candidates_n, candidates, pre_siblings);
	else if(lb_method == Hybrid) {
		if(search_space < hyrbid_ratio*g_n*g_n) compute_best_extension_LSa(now, candidates_n, candidates, pre_siblings);
		else {
			search_n = search_n_for_IS;
			compute_best_extension_BM(1, now, candidates_n, candidates, pre_siblings, 0);
		}
	}
	else printf("lb_method is wrong in construct_sibling!\n");
#endif
}

void Application::extend_to_full_mapping(State *parent, State *now) {
	assert(parent != NULL);
	//printf("begin extend_to_full_mapping lb_method %d\n", lb_method);
	if(lb_method == BMao) {
		now->level = parent->level + 1;
		now->mapped_cost = parent->mapped_cost;
		now->parent = parent;
		now->pre_sibling = NULL;

		assert(now->level == search_n);

		while(parent != NULL) {
			visY[parent->image] = 1;
			parent = parent->parent;
		}
		ui candidates_n = 0;
		for(ui i = 0;i < g_n;i ++) {
			if(!visY[i]) candidates[candidates_n ++] = i;
			else visY[i] = 0;
		}
		ui pre_siblings = 0;

		compute_best_extension_BM(1, now, candidates_n, candidates, pre_siblings, 0, 1);
	}
	else {
		assert(parent != NULL&&parent->level+1 == q_n);
		ui cot = parent->mapped_cost;
		now = parent;
		if(q_n < g_n) {
			while(parent != NULL) {
				visY[parent->image] = 1;
				parent = parent->parent;
			}
			for(ui i = 0;i < g_n;i ++) if(!visY[i]) {
				for(ui j = g_starts[i];j < g_starts[i+1];j ++) if(visY[g_edges[j]]) ++ cot;
				visY[i] = 1;
			}
			cot += (g_n-q_n);
			for(ui i = 0;i < g_n;i ++) visY[i] = 0;
		}
		if(cot < upper_bound) {
			while(now != NULL) {
				BX[MO[now->level]] = now->image;
				now = now->parent;
			}
			upper_bound = cot;
#ifndef NDEBUG
			assert(upper_bound == compute_ged_of_BX());
#endif
		}
	}
	//printf("end extend_to_full_mapping\n");
}

ui Application::Hungarian(char initialization, ui n, ui *cost) {
#ifndef NDEBUG
	//printf("Cost Matrix:\n");
	//for(ui i = 0;i < n;i ++) {
	//	for(ui j = 0;j < n;j ++) printf(" %d", cost[i*n+j]);
	//	printf("\n");
	//}
#endif
	if(initialization) {//Initialization
		memset(mx, -1, sizeof(int)*n);
		memset(my, -1, sizeof(int)*n);
		memset(ly, 0, sizeof(int)*n);
		for(ui i = 0;i < n;i ++) {
			lx[i] = INF;
			ui *t_array = cost+i*n;
			for(ui j = 0;j < n;j ++) if(t_array[j] < lx[i]) lx[i] = t_array[j];
			for(ui j = 0;j < n;j ++) if(my[j] == -1&&t_array[j] == lx[i]) {
				mx[i] = j;
				my[j] = i;
				break;
			}
		}
	}

	for(ui u = 0;u < n;u ++) if(mx[u] == -1) { //Augmentation
		memset(visX, 0, sizeof(char)*n);
		memset(visY, 0, sizeof(char)*n);

		int q_n = 1;
		queue[0] = u;
		visX[u] = 1;

		ui *t_array = cost + u*n;
		for(ui i = 0;i < n;i ++) {
			slack[i] = t_array[i] - lx[u] - ly[i];
			slackmy[i] = u;
		}

		int target = n, X;
		while(true) {
			for(ui i = 0;i < q_n&&target == n;i ++) {
				ui v = queue[i];
				t_array = cost + v*n;
				for(ui j = 0;j < n;j ++) if(!visY[j]&&t_array[j] == lx[v] + ly[j]) {
					if(my[j] == -1) {
						X = v;
						target = j;
						break;
					}
					visY[j] = 1;
					X = my[j];
					visX[X] = 1;
					prev[X] = v;
					queue[q_n ++] = X;

					ui *tt_array = cost + X*n;
					for(ui k = 0;k < n;k ++) if(!visY[k]&&tt_array[k] - lx[X] - ly[k] < slack[k]) {
						slack[k] = tt_array[k] - lx[X] - ly[k];
						slackmy[k] = X;
					}
				}
			}
			if(target != n) break;

			q_n = 0;
			int delta = INF;
			for(ui i = 0;i < n;i ++) if(!visY[i]&&slack[i] < delta) delta = slack[i];
			for(ui i = 0;i < n;i ++) {
				if(visX[i]) lx[i] += delta;
				if(visY[i]) ly[i] -= delta;
				else slack[i] -= delta;
			}

			for(ui i = 0;i < n;i ++) if(!visY[i]&&slack[i] == 0) {
				if(my[i] == -1) {
					X = slackmy[i];
					target = i;
					break;
				}

				visY[i] = 1;
				if(!visX[my[i]]) {
					X = my[i];
					visX[X] = 1;
					prev[X] = slackmy[i];
					queue[q_n ++] = X;

					ui *tt_array = cost + X*n;
					for(ui k = 0;k < n;k ++) if(!visY[k]&&tt_array[k] - lx[X] - ly[k] < slack[k]) {
						slack[k] = tt_array[k] - lx[X] - ly[k];
						slackmy[k] = X;
					}
				}
			}
		}

		while(true) {
			int ty = mx[X];
			mx[X] = target;
			my[target] = X;
			if(X == u) break;

			X = prev[X];
			target = ty;
		}
	}

	memset(visX, 0, sizeof(char)*n);
	memset(visY, 0, sizeof(char)*n);

	int res = 0;
	for(ui i = 0;i < n;i ++) res += cost[i*n + mx[i]];
	return res;
}

ui Application::compute_ged_of_BX() {
	if(upper_bound > verify_upper_bound) return upper_bound;

	ui *py = new ui[g_n];
	for(ui i = 0;i < g_n;i ++) py[i] = q_n;
	for(ui i = 0;i < q_n;i ++) py[BX[i]] = i;

	ui ged = g_n - q_n;
	for(ui i = 0;i < q_n;i ++) if(q_vlabels[i] != g_vlabels[BX[i]]) ++ ged;

	ged += q_starts[q_n]/2;
	for(ui i = 0;i < g_n;i ++) for(ui j = g_starts[i];j < g_starts[i+1];j ++) {
		ui v = g_edges[j];
		if(v < i) continue;

		++ ged;
		if(py[i] >= q_n||py[v] >= q_n) continue;
		if(q_matrix[py[i]*q_n+py[v]] == g_elabels[j]) ged -= 2;
		else if(q_matrix[py[i]*q_n+py[v]] < elabels_n) -- ged;
	}
	delete[] py; py = NULL;
#ifndef NDEBUG
	printf("Verified mapping (");
	for(ui i = 0;i < q_n;i ++) {
		if(i != 0) printf(",");
		printf("%d", BX[i]);
	}
	printf(") has GED %d\n", ged);
#endif
	return ged;
}

void Application::get_mapping(vector<pair<ui,ui> > &mapping, ui n) {
	mapping.clear();
	for(ui i = 0;i < n;i ++) {
		ui u = MO[i];
		mapping.pb(mp(u, BX[i]));
	}
}

void Application::verify_induced_ged(State *now) {
	ui *px = new ui[q_n];
	ui *py = new ui[g_n];

	for(ui i = 0;i < q_n;i ++) px[i] = g_n;
	for(ui i = 0;i < g_n;i ++) py[i] = q_n;
	for(State *st = now;st != NULL;st = st->parent) {
		px[MO[st->level]] = st->image;
		py[st->image] = MO[st->level];
	}

	ui res = 0;
	for(ui i = 0;i <= now->level;i ++) {
		ui u = MO[i], v = px[u];
		if(q_vlabels[u] != g_vlabels[v]) ++ res;
		for(ui j = q_starts[u];j < q_starts[u+1];j ++) if(px[q_edges[j]] < g_n&&q_edges[j] > u) ++ res;
		for(ui j = g_starts[v];j < g_starts[v+1];j ++) if(py[g_edges[j]] < q_n&&g_edges[j] > v) {
			++ res;
			if(q_matrix[u*q_n+py[g_edges[j]]] == g_elabels[j]) res -= 2;
			else if(q_matrix[u*q_n+py[g_edges[j]]] < elabels_n) -- res;
		}
	}

	if(res != now->mapped_cost) printf("(%d->%d) res: %d, now->mapped_cost: %d\n", MO[now->level], now->image, res, now->mapped_cost);

	delete[] px;
	delete[] py;
}

void Application::verify_LS_lower_bound(State *now) {
	int *v_labels_cnt = new int[vlabels_n];
	int *e_labels_cnt = new int[elabels_n];
	memset(v_labels_cnt, 0, sizeof(int)*vlabels_n);
	memset(e_labels_cnt, 0, sizeof(int)*elabels_n);

	char *vis = new char[g_n];
	memset(vis, 0, sizeof(char)*g_n);
	for(State *st = now;st != NULL;st = st->parent) vis[st->image] = 1;

	int uvl_cnt = 0, vvl_cnt = 0, uel_cnt = 0, vel_cnt = 0, vl_common = 0, el_common = 0;
	for(ui i = now->level+1;i < q_n;i ++) {
		-- v_labels_cnt[q_vlabels[MO[i]]];
		++ uvl_cnt;
	}
	for(ui i = 0;i < g_n;i ++) if(!vis[i]) {
		if(v_labels_cnt[g_vlabels[i]] < 0) ++ vl_common;
		++ v_labels_cnt[g_vlabels[i]];
		++ vvl_cnt;
	}

	for(ui i = 0;i < g_n;i ++) if(!vis[i]) {
		for(ui j = g_starts[i];j < g_starts[i+1];j ++) if(vis[g_edges[j]]||(!vis[g_edges[j]]&&g_edges[j] > i)) {
			++ vel_cnt;
			-- e_labels_cnt[g_elabels[j]];
		}
	}

	memset(vis, 0, sizeof(char)*g_n);
	for(ui i = 0;i <= now->level;i ++) vis[MO[i]] = 1;

	for(ui i = now->level+1;i < q_n;i ++) {
		ui u = MO[i];
		for(ui j = q_starts[u];j < q_starts[u+1];j ++) if(vis[q_edges[j]]||(!vis[q_edges[j]]&&q_edges[j] > u)) {
			++ uel_cnt;
			if(e_labels_cnt[q_elabels[j]] < 0) ++ el_common;
			++ e_labels_cnt[q_elabels[j]];
		}
	}

	int res = now->mapped_cost;
	if(vvl_cnt > uvl_cnt) uvl_cnt = vvl_cnt;
	if(vel_cnt > uel_cnt) uel_cnt = vel_cnt;
	res += uvl_cnt - vl_common + uel_cnt - el_common;

	if(res != now->lower_bound) {
		printf("WA (%d->%d) res: %d, now->lower_bound: %d, now->mapped_cost: %d\n", MO[now->level], now->image, res, now->lower_bound, now->mapped_cost);
	}
	assert(res == now->lower_bound);

	delete[] vis;
	delete[] v_labels_cnt;
	delete[] e_labels_cnt;
}
