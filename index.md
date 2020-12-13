## Summary

We are going to implement an MPI parallel program that models infectious disease spreading in a big crowd. The basic version would apply a random wandering individual movement model and Gaussian Distribution on the distance between individuals to calculate the infectious rate, combined with a concurrent R-tree to optimize searching individuals within a specific area. A Social Force based movement model and danger area assumption are two feasible extensions on this topic.


## Documents
### Proposal

You could find our project proposal use the link [here](https://docs.google.com/document/d/1tsleYAoje3bNcmz6uF-RaMOxEU0FN9mPoS9msJkUcrA/edit?usp=sharing).

### Checkpoint Report

You could find checkpoint report for this porject [here](https://docs.google.com/document/d/1BM-jaM9GXgmGWbn2sI7aPPbhSdBLyLSXRcmYJ2oPEDE/edit?usp=sharing).

### Final Report

Final report could be found in this [link](https://www.overleaf.com/read/wcshrqfybwbs). Also, we have a simple video pre on [here](https://youtu.be/exwPxSS-S1k).

## Work Completed
After exploring the researches in infectious disease field, we dicided to provide a paralleled version of SIR model simulation based on distance between individuals in a specific area. Individuals are divided into suspicious, infected and recovered group and their contact are described by contact between individuals whose distance inbetween is less than a specific threshold. At each iteration, the individuals are moving based on random incentive or affected by individuals nearby and contact happened after each movement. The simulation could indicate the SIR number in a specific area after a number of iteration based on parameters, such as infectious rate or infectious distance.

We firstly started with a linear scan version, which is straight forward and easy to implement. The individuals are moving randomly inside the area and at each iteration, we linear scan every individuals to calculate distance for every infected individuals. In this approach, it is easily to tell that for both movement stage and contact stage, the time complexity is O(N^2), where N denotes the number of individuals.

One big drawback of this problem is that linear scan is very inefficient and unnecesary. To optimize the contact stage, we implement R-tree for this problem that could optimize the time complexity from sqaure time to logarithmic time based on the entry size of each node in R-tree. Although R-tree could accelerate the contact stage, we still need to re-construct the r-tree at every iteration after each movement and our computation are still serialized.

To parallel our calculation, we come up with a data-partition approach based on MPI to further optimize our simulation. We started with a static assignment that divided the target areas into P rectangles, each rectangle has the same width of target area, but only with partial height. These rectangles are aligned vertically so each rectangle only next to the rectangle above or below it. With this static assignment, each worker could build the R-tree locally. At contact stage, each worker need to communicate with the worker above and below to get the "ghost rectangle" which contains the individuals that may contact with the local individual. At movement stage, each worker need to communicate with the worker above and below to send and receive the individuals who leaves or moves into local area.

One problem is that in the real world, people are usually gathering together inside a area, instead of uniformly distributed. It is a common scenario when people's attention are focusing on one lecturer or a performer. So, we also provide another initial distribution that objects are centralized according to a normal distribution. We believe such initial distribution are much closer to the scenario in the real life. With this initial distribution, static assignment's performance is broken since pre-defined area would lead to significant work imbalance. So we implemented the cost zone approach mentioned in the lecture to dynamically generate an initial distribution according to the input object distribution.

Firstly, we let each worker count one part of all objects to find out which block it belongs, and maintain the counter for each block locally. After this, our master process gathers all counter from all workers and do a cost zone assignment. Using the counter information, the master process knows the potential work at each block, so it balances it according to its counter by giving each worker different number of blocks, and send this assignment to all workers.

### Schedule

#### Nov.5 - Nov.11
* Read papers to learn the details of R-tree implementation.
* Design linear infectious disease model based on infectious research.

#### Nov.12 - Nov.15
* Implement linear R-tree searching.
* Implement linear R-tree inserting.
* Implement linear R-tree deleting.

#### Nov.16 - Nov.18
* Test R-tree implementation using randomly generated test cases.
* Implement the linear model using a linear scan searching.

#### Nov.19 - Nov.21
* Implement the linear model using R-tree.
* Implement visualization tools for R-tree and model outcome.

#### Nov.22 - Nov.25
* Design MPI data partition version as a parallel baseline for infectious modeling.
* Implement MPI baseline infectious model (static task assignment and contact calculation).
* Implement MPI baseline infectious model (data partition check and task reallocate).
* Validate the outcome of the MPI version infectious model.

#### Nov.26 - Nov.28
* Thanksgiving break (thank all 15-618 course-staff).

#### Nov.29 - Dec.1
* Finish mid-checkpoint report.
* Design a dynamic task assignment for the MPI infectious model.

#### Dec.2 - Dec.4
* Implement MPI parallel version using cost zones (object and matrix generators).
* Implement MPI parallel version using cost zones (task counting and dynamic task assigning).

#### Dec.5 - Dec.7
* Implement MPI parallel version using cost zones (ghost objects collection and contact calculation).
* Implement MPI parallel version using cost zones (objects movement and task reassignment).

#### Dec.8 - Dec.12
* Analyze the performance and scalability of MPI version infectious model.
* Refine the MPI version infectious model if find some severe bottlenecks

#### Dec.13 - Dec.14
* Prepare for the poster session.

## Goals and Deliverables
### Plan to Achieve
* Implement r-tree from scratch, which should surpass linear scan searching performance.
* Implement a serial program simulating the infectious disease spreading as the baseline.
* Combine R-tree with our baseline to provide reasonable serial simulation.
* Design and implement a simple static assignment MPI version of our infectious disease spreading simulation.
* Improve performance on imbalanced initial distribution by converting static assignment to dynamic assignment.
* Analyze the performance, weak scalability and strong scalability of our implementation based on different sizes of cluster.

### Nice to Have
* Move from the random wandering movement model to the Social Force movement model without degrading the previous speedup.
* Add danger area concept into the calculation of infectious rate without degrading previous speedup.



## Reference

1. Individual space–time activity-based modelling of infectious disease transmission within a city. [https://www.ncbi.nlm.nih.gov/pmc/articles/PMC2607451/](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC2607451/)

2. Eubank et al., Modelling disease outbreaks in realistic urban social networks, Nature 429, 180-184. [http://www.nature.com/nature/journal/v429/n6988/full/nature02541.html](http://www.nature.com/nature/journal/v429/n6988/full/nature02541.html)

3. Simulating dynamical features of escape panic. [https://www.nature.com/articles/35035023](https://www.nature.com/articles/35035023)

4. A study of concurrent operations on R-trees. [https://www.sciencedirect.com/science/article/pii/S0020025596002198](https://www.sciencedirect.com/science/article/pii/S0020025596002198)

5. High-Concurrency Locking in R-Trees. [https://dsf.berkeley.edu/papers/vldb95-rtree.pdf](https://dsf.berkeley.edu/papers/vldb95-rtree.pdf)
