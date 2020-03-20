#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Individual;

struct Individual {
    struct Individual * neighbours[50];
    int num_neighbours;
    int state;
    int id;
};

struct Individual p[500];
int p_size = 200;
float r_rate = 0.3; // each time interval someone infected has this chance of recovering
float d_rate = 0.1; // and this chance of dying
float t_rate = 0.2; // this is the probability that someone infected will infect someone else this time step
int remission = 0; // if 1 can be infected multiple times
int min_neighbours = 1;
int max_neighbours = 10;
int rng(int min, int max) {
	return rand()%(max-min + 1) + min;
}

int pick_neighbour(int i) {
	int n = i;
	while (n == i)
		n = rng(0, p_size -1);
	return n; // could make this return in range around individual,
	// or return certain individuals more often
}

int rate_l(float r) {
	int k = r * 10000;
	return (rng(0, 10000)<k)?1:0;
} // Returns 1 if event occurs

void update(void) {
	int i;
        int st_c[4] = {0, 0, 0, 0};
	
	/* state = 0, alive. state = 1 infected. state = 2 recovered; state = 3 dead */
	for (i = 0; i < p_size; i++) {
		st_c[p[i].state]++;
	}
	printf("%d, %d, %d, %d\n", st_c[0], st_c[1], st_c[2], st_c[3]);
	return;
}

int iterate(void) {
	int i,n;
	for (i = 0; i < p_size; i++) {
		if (p[i].state != 1)
			continue;
		if (rate_l(r_rate) == 1)
			p[i].state = 2;
		if (rate_l(d_rate) == 1)
			p[i].state = 3;
		for (n = 0; n < p[i].num_neighbours-1; n++){
			if (rate_l(t_rate) == 1) {
				
				switch (p[i].neighbours[n]->state) {
					case 0: p[i].neighbours[n]->state = 1;break;
					case 2: if (remission == 1) p[i].neighbours[n]->state = 1;break;
					default: break;
				}
			}
		}
	}
	return 1;
}


void parse_opts(int argc, char *argv[]) {
	int i;
	if ((argc - 1)%2 != 0)
		printf("Odd number of options - may be incorrect.\n");
	for (i = 1; i < argc-1; i+=2) {
		switch (argv[i][0]) {
			case 'p': p_size = atoi(argv[i+1]); break;
			case 'r': r_rate = atof(argv[i+1]); break;
			case 'd': d_rate = atof(argv[i+1]); break;
			case 't': t_rate = atof(argv[i+1]); break;
			case 'e': remission = atoi(argv[i+1]); break;
			case 'n': min_neighbours = atoi(argv[i+1]); break;
			case 'm': max_neighbours = atoi(argv[i+1]); break;
			default: break;
		}
	}
	return;
}

int main(int argc, char * argv[]) {
	int i,r,l,n;
	srand(time(NULL));
	parse_opts(argc, argv);
	printf("#[%d] %f/%f/%f %d %d-%d\n", p_size, r_rate, d_rate, t_rate, remission, min_neighbours, max_neighbours);
	for (i = 0; i < p_size; i++) {
		p[i].state = 0;
		p[i].num_neighbours = 0;
		p[i].id = i;
	}
	for (i = 0; i < p_size; i++) {
		l = rng(min_neighbours, max_neighbours);
		for (r = p[i].num_neighbours; r < l; r++) {
			n = pick_neighbour(i); // vary this to vary how the graph spreads
			//printf("%d (%d) - %d (%d)\n", i, r, n, p[n].num_neighbours);
			p[i].neighbours[r] = &(p[n]);
			p[i].num_neighbours++;
			p[n].neighbours[p[n].num_neighbours] = &(p[i]);
			p[n].num_neighbours++;

		}
	}
	//patient zero
	p[rng(0, p_size)].state = 1;
	for (i = 0; i < 100; i++) {
		update();
		iterate();
	}
	return 1;

}
