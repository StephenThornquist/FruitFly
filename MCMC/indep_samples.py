#### compute credible intervals for two independent simultaneous trials with a Jeffreys prior SCT 06/10/2016

import pymc
import numpy as np
import matplotlib.pyplot as plt

### for testing purposes
example = True

example_data1 = np.hstack( (np.ones((19,)) , np.zeros((30,))) )
example_data2 = np.hstack( (np.ones((17,)) , np.zeros((24,))) )

p1 = np.mean(example_data1)
p2 = np.mean(example_data2)
print "P1 = " + str(p1)

print "P2 = " + str(p2)

print "Independence = " + str(p1+p2-p1*p2)

if example:
	data1 = example_data1
	data2 = example_data2

p1 = pymc.Beta('p1',alpha=0.5,beta=0.5)
p2 = pymc.Beta('p2',alpha=0.5,beta=0.5)

x1 = pymc.Binomial('x',n=len(data1),p=p1,value=np.sum(data1),observed=True)
x2 = pymc.Binomial('x',n=len(data2),p=p2,value=np.sum(data2),observed=True)

@pymc.deterministic
def ind_assump(p1=p1,p2=p2):
	return p1+p2-p1*p2