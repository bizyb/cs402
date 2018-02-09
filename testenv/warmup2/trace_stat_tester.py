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

class Packet(object):
	
	def __init__(self, *args, **kwargs):

		self.packetID = kwargs.get("packetID")
		self.serverID = kwargs.get("serverID")
		self.ia = kwargs.get("ia") #inter-arrival time
		self.q1_duration = kwargs.get("q1_duration")
		self.q2_duration = kwargs.get("q2_duration")
		self.service_time = kwargs.get("service_time")
		self.service_time_m = kwargs.get("service_time_m") #measured


class Stat(object):

	def __init__(self, *args, **kwargs):

		self.packet_ia 	= kwargs.get("packet_ia")
		self.service_time = kwargs.get("service_time")
		self.num_p_q1 = kwargs.get("num_p_q1")
		self.num_p_q2 = kwargs.get("num_p_q2")
		self.num_p_s1 = kwargs.get("num_p_s1")
		self.num_p_s2 = kwargs.get("num_p_s2")
		self.sys_time = kwargs.get("sys_time")
		self.stdv = kwargs.get("stdv")
		self.t_drop = kwargs.get("t_drop")
		self.p_drop = kwargs.get("p_drop")

class Warmup2Tester(object):

	def __init__(self, *args, **kwargs):

		self.emul_file = kwargs.get("file")
		self.parse_file()
		# print "\n\nEmulation file: {}\n\n".format(self.emul_file)

	def parse_file(self):

		with open(self.emul_file, "r") as f:
			print f.readlines()

	def run_test(self):
		pass


def add_arguments(parser):
	"""
	Parse command-line arguments. Output usage info as appropriate.
	"""

	help = 'Usage: python trace_stat_tester.py emulation_file'
	arguments = [
			{
				'argument': 'emul_file',
				'settings': {
					'nargs': '+',
					'type': str,
					'help': help,
				}
			}
	]
	for arg_dict in arguments:
		arg = arg_dict.get('argument')
		settings = arg_dict.get('settings')
		parser.add_argument(arg, **settings)

	return parser

def parse_arguments(parser):

	args = parser.parse_args()
	kwargs = {'file': args.emul_file[0]}
	
	return kwargs


import argparse
parser = argparse.ArgumentParser(description='CS402 Spring 2018: Warmup2 Emulation Trace Tester')
parser = add_arguments(parser)
kwargs = parse_arguments(parser)

w2t = Warmup2Tester(**kwargs)
w2t.run_test()


