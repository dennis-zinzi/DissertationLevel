# A* Pathfinding project
The Unreal Engine 4 level for my CSC3095 Dissertation project comparing the use of A* pathfinding for multiple single AI entities versus
the use of A* pathfinding on a single Flock of AI entities to move through a randomly generated environment to reach the goal.

## The Environment
The	 level	 is	 set	 up	 to	 include	N number of	world	 objects,	 to	 act	 as	 obstacles, placed	 at	 random	 positions	 on	 the	 floor	 of	 the	 game	world,	 and	 where	N objects	 will	 not	 have	 a	 high	 probability	 of	 preventing	 AI	 entities	
from	moving,	due	to	lack	of	physical	space	to	do so.	The	world	objects	are	set	at	random	
positions,	 as	 this	 approach	 will	 allow	 the	 test	 to	 be	 carried	 out	 on	 any	 game	 level
environment,	providing	a	degree	of	reliability	and	stability	to	the	tests	to	be	carried	out.	

### Environment as Nodes (PathNode objects)
A*	 search	 would	 not	 work	 without	 having	 the	 environment	 represented	 as	 a	 node	
graph,	and	to	do	that	we’ll	first	have	to	determine	the	characteristics	of	a	single	node.	

I	define	a	node	to	have	the	following	properties:
- Unique	identifier/ID	(int)
- 3-D	position	in	the	world (FVector)
- Cost	of	getting	to	the	node (int)
- List	of	neighboring	node	IDs (vector<int>)
- Parent	node (PathNode*)
- Is	the	node	reachable/blocked? (bool)


## A* Pathfinding
The A* search algorithm is a best-first node graph search algorithm which aims to find the list of nodes which form a path from node A to B by using a hueristic function to calculate the cost of moving to a given node. 

The	 formula	 for	 determining	 the	 cost	 to	 navigate	 to	 any	 node	 in	 the	 node	 graph, is	
defined	as:

                                            f(n) = g(n) + h(n)

Where:
- f(n) is	the	total	cost	to	reach	the	node	n
- g(n) is	the	minimum	cost	to	travel	to	node	n	from	the	initial	node
- h(n) is	the	heuristic	cost	from	node	n	to	the	goal	node

To	set	up	the	algorithm	you	need	to	initialize an	“open”	list, nodes	to	be	considered	for	
the	 algorithm; and	 a	 “closed”	 list, potential	 nodes	 that	 will	 generate	 the	 final	 path,	
initially	empty.	

Once	 the	 open	 and	 closed	 lists	 have	 been	 created,	 then	 the	 A*	 search	 algorithm	 can	
begin	executing	as	follows:

1) Add	start to	open	list and	calculate	ƒ(start). [Previous	formula]
2) Select	a	node	n with	the	lowest	ƒ(n) value	from	the	open	list.
3) If	n is	goal,	add	n to	closed list and	terminate	the	algorithm.
4) Otherwise,	add n to	closed	list,	and	find	n’s	successor nodes.	
5) Calculate	f(successor) for	each	child of	n,	and	add	each	successor	not already	
in	the	closed	list	to	the	open	list.	
6) If	any	successor	already	in	the	closed	list	has	an	f(successor) smaller	now	than	
it	was	when	was	added	to	the	closed list,	add	it	to	the	open	list.	
7) Go	to	Step	2.



#### *All A** *operations and functions are enclosed in the AStar class.*


## Reynolds Flocking
The AI swarm behavior I chose to adpot was the one defined by Reynolds: Flocking.

Flocking has three distinct rules:
1) Collision	Avoidance:	avoid	collisions	with	nearby	flockmates
2) Velocity	Matching:	attempt	to	match	velocity	with	nearby	flockmates
3) Flock	Centering:	attempt	to	stay	close	to	nearby	flockmates

I've additionally added a 4th rule, goal tendency, to	make the	swarm	converge	towards	a	certain	location,	as	we	need	to	make	
sure	the	boids	move	towards	the	goal	location	every	step	of	the	way.

#### *All flocking rules and swarm behaviors are enclosed in the BoidFlock class*
