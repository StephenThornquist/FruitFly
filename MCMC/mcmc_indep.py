import numpy as np
import matplotlib.pyplot as plt
import pymc
import indep_samples

S = pymc.MCMC(indep_samples, db='pickle')
S.sample(iter=100000, burn=10000, thin=3)

print '\n'
print 'MEAN POSTERIOR ESTIMATES: \n' + 'p1: '+str(S.p1.stats()['mean']) + '\n' + \
		 'p2: ' + str(S.p2.stats()['mean']) + '\n' +\
		  'independence: ' + str(S.ind_assump.stats()['mean']) + '\n'

print '\n'

p1_values = S.p1.trace()
p2_values = S.p2.trace()
ind_values = S.ind_assump.trace()

print "68% confidence intervals: \n"
print 'p1: ' + str([np.percentile(p1_values,16),np.percentile(p1_values,84)])
print 'p2: ' + str([np.percentile(p2_values,16),np.percentile(p2_values,84)])
print 'independence: ' + str([np.percentile(ind_values,16),np.percentile(ind_values,84)])

print '\n'

print "95% confidence intervals: \n"
print 'p1: ' + str([np.percentile(p1_values,2.5),np.percentile(p1_values,97.5)])
print 'p2: ' + str([np.percentile(p2_values,2.5),np.percentile(p2_values,97.5)])
print 'independence: ' + str([np.percentile(ind_values,2.5),np.percentile(ind_values,97.5)])

observed = 0.851

print 'p value: '+str((ind_values > observed).sum()/float(ind_values.shape[0]))

#pymc.Matplot.plot(S)
