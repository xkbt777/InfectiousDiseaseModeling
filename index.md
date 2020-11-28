## Summary

We are going to implement an MPI parallel program that models infectious disease spreading in a big crowd. The basic version would apply a random wandering individual movement model and Gaussian Distribution on the distance between individuals to calculate the infectious rate, combined with a concurrent R-tree to optimize searching individuals within a specific area. A Social Force based movement model and danger area assumption are two feasible extensions on this topic.


## Proposal

You could find our project proposal use the link [here](https://docs.google.com/document/d/1tsleYAoje3bNcmz6uF-RaMOxEU0FN9mPoS9msJkUcrA/edit?usp=sharing).

## Work Completed
After exploring the researches in infectious disease field, we dicided to provide a paralleled version of SIR model simulation based on distance between individuals in a specific area. Individuals are divided into suspicious, infected and recovered group and their contact are described by contact between individuals whose distance inbetween is less than a specific threshold. At each iteration, the individuals are moving based on random incentive or affected by individuals nearby and contact happened after each movement. The simulation could indicate the SIR number in a specific area after a number of iteration based on parameters, such as infectious rate or infectious distance.

We firstly started with a linear scan version, which is straight forward and easy to implement. The individuals are moving randomly inside the area and at each iteration, we linear scan every individuals to calculate distance for every infected individuals. In this approach, it is easily to tell that for both movement stage and contact stage, the time complexity is O(N^2), where N denotes the number of individuals.

One big drawback of this problem is that linear scan is very inefficient and unnecesary. To optimize the contact stage, we implement R-tree for this problem that could optimize the time complexity from sqaure time to logarithmic time based on the entry size of each node in R-tree. Although R-tree could accelerate the contact stage, we still need to re-construct the r-tree at every iteration after each movement and our computation are still serialized.

To parallel our calculation, we come up with a data-partition approach based on MPI to further optimize our simulation. We started with a static assignment that divided the target areas into P rectangles, each rectangle has the same width of target area, but only with partial height. These rectangles are aligned vertically so each rectangle only next to the rectangle above or below it. With this static assignment, each worker could build the R-tree locally. At contact stage, each worker need to communicate with the worker above and below to get the "ghost rectangle" which contains the individuals that may contact with the local individual. At movement stage, each worker need to communicate with the worker above and below to send and receive the individuals who leaves or moves into local area.

## Schedule
### Schedule

#### Nov.5 - Nov.11
* Do some research on R-tree implementation.
* Design our simplified infectious disease simulation based on infectious research.

#### Nov.12 - Nov.19
* Finish basic R-tree implementation using C.
* Finish linear-scan search infectious disease simulation.
* Combine R-tree with infectious disease simulation to provide linear R-tree baseline.

#### Nov.20 - Nov.30 (Mid checkpoint)
* Design and implement MPI data partition version of baseline.
* Provide vistualization tools for simulation outcome.
* Validate the simulation outcome of MPI version.



## Reference

1. Individual space–time activity-based modelling of infectious disease transmission within a city. [https://www.ncbi.nlm.nih.gov/pmc/articles/PMC2607451/](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC2607451/)

2. Eubank et al., Modelling disease outbreaks in realistic urban social networks, Nature 429, 180-184. [http://www.nature.com/nature/journal/v429/n6988/full/nature02541.html](http://www.nature.com/nature/journal/v429/n6988/full/nature02541.html)

3. Simulating dynamical features of escape panic. [https://www.nature.com/articles/35035023](https://www.nature.com/articles/35035023)

4. A study of concurrent operations on R-trees. [https://www.sciencedirect.com/science/article/pii/S0020025596002198](https://www.sciencedirect.com/science/article/pii/S0020025596002198)

5. High-Concurrency Locking in R-Trees. [https://dsf.berkeley.edu/papers/vldb95-rtree.pdf](https://dsf.berkeley.edu/papers/vldb95-rtree.pdf)
