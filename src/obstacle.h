#ifndef OBSTACLE_H
#define OBSTACLE_H

#define Q_EMPTY \
	0,0,0,0, \
	0,0,0,0, \
	0,0,0,0, \
	0,0,0,0
#define Q_FULL \
	1,1,1,1, \
	1,1,1,1, \
	1,1,1,1, \
	1,1,1,1
#define Q_TRIANGLE_RIGHT \
	1,1,1,1, \
	0,1,1,1, \
	0,0,0,1, \
	0,0,0,0
#define Q_TRIANGLE_LEFT \
	1,1,1,1, \
	1,1,1,0, \
	1,0,0,0, \
	0,0,0,0
#define Q_TOP_HALF \
	1,1,1,1, \
	1,1,1,1, \
	0,0,0,0, \
	0,0,0,0
#define Q_TOP_QUARTER \
	1,1,1,1, \
	0,0,0,0, \
	0,0,0,0, \
	0,0,0,0
#define Q_TOP_3_QUARTERS \
	1,1,1,1, \
	1,1,1,1, \
	1,1,1,1, \
	0,0,0,0
#define Q_PILLAR \
	0,1,1,0, \
	0,1,1,0, \
	0,1,1,0, \
	0,1,1,0

extern short table_decor_quart[];

#endif