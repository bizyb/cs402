# TODO (pseudocode):
# 1. Timestamps
# 	- formatting: has 8 leading digits, a period, and three significant figures
# 	- emul start: verify it's at time point 0 and has the form 00000000.000ms
# 	- monotonicity: 
# 		- parse out all the timestamps, cast them to float, and add them to a list (control)
# 		-sort the control list and create a new sorted list
# 		-compare the sorted and unsorted list; if any difference, fail timestamp on monotonicity
# 		- (LAST) verify that the timestamps are the result of adding the inter-arrival times, service times, etc.
# 2. Packet trace output
# 	- each packet fully served must have 7 lines (temporally contiguous but not necessarily spacially)
# 	- 
# 3. Packet values
# 	- create a Packet object with all the relevant attributes
# 	-save each packet object in hash table, i.e. a dictionary where key=packetID, value=packet object
# 	-parse out each attribute and assign it to the appropriate packet 
# 3. Stats 
# 	- create a stat object and save the relevant values to its attributes 
# 4. Recompute the stats
# 	- use the packet objects to recompute the stats
# 	- output the recomputed values and the parsed values side by side 
# 	- let the user decide whether the values are close enough
# 	- make note that comparing floats with many significant figures for equivalence is not a good idea
# 5.  
