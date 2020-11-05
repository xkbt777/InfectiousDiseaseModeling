## Summary

We are going to implement an MPI parallel program that models infectious disease spreading in a big crowd. The basic version would apply a random wandering individual movement model and Gaussian Distribution on the distance between individuals to calculate the infectious rate, combined with a concurrent R-tree to optimize searching individuals within a specific area. A Social Force based movement model and danger area assumption are two feasible extensions on this topic.


## Proposal

You could find our project proposal use the link [here](https://docs.google.com/document/d/1tsleYAoje3bNcmz6uF-RaMOxEU0FN9mPoS9msJkUcrA/edit?usp=sharing).

## Reference

1. Individual space–time activity-based modelling of infectious disease transmission within a city. [https://www.ncbi.nlm.nih.gov/pmc/articles/PMC2607451/](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC2607451/)

2. Eubank et al., Modelling disease outbreaks in realistic urban social networks, Nature 429, 180-184. [http://www.nature.com/nature/journal/v429/n6988/full/nature02541.html](http://www.nature.com/nature/journal/v429/n6988/full/nature02541.html)

3. Simulating dynamical features of escape panic. [https://www.nature.com/articles/35035023](https://www.nature.com/articles/35035023)

4. A study of concurrent operations on R-trees. [https://www.sciencedirect.com/science/article/pii/S0020025596002198](https://www.sciencedirect.com/science/article/pii/S0020025596002198)

5. High-Concurrency Locking in R-Trees. [https://dsf.berkeley.edu/papers/vldb95-rtree.pdf](https://dsf.berkeley.edu/papers/vldb95-rtree.pdf)
