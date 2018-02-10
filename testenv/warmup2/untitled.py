# TODO (pseudocode):
# 1. Timestamps
# 	- formatting: has 8 leading digits, a period, and three significant figures
# 	- (LAST) verify that the timestamps are the result of adding the inter-arrival times, service times, etc.
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

import re
import copy
import decimal

class Token(object):

	def __init__(self, *args, **kwargs):

		self.tokenID = kwargs.get("tokenID")
		self.dropped = False


class Packet(object):
	
	def __init__(self, *args, **kwargs):

		self.packetID = kwargs.get("packetID")
		self.ia = kwargs.get("ia") #inter-arrival time
		self.tokens = kwargs.get("tokens")
		self.q1_duration = kwargs.get("q1_duration")
		self.q2_duration = kwargs.get("q2_duration")
		self.service_time = kwargs.get("service_time")
		self.service_time_m = kwargs.get("service_time_m") #measured
		self.serverID = 0
		self.num_trace_lines = 0
		self.served = False
		self.dropped = False


class Stat(object):

	def __init__(self, *args, **kwargs):

		self.packet_ia 	= -1
		self.service_time = -1
		self.num_p_q1 = -1
		self.num_p_q2 = -1
		self.num_p_s1 = -1
		self.num_p_s2 = -1
		self.sys_time = -1
		self.stdv = -1
		self.t_drop = -1
		self.p_drop = -1

class Warmup2Tester(object):

	def __init__(self, *args, **kwargs):

		self.emul_file = kwargs.get("file")
		self.emul_lines = self.parse_file()
		self.raw_timestamps = self.get_timestamps()
		self.packets = {}
		self.stats = {}


	def parse_file(self):

		emul_lines = []
		with open(self.emul_file, "r") as f:
			# remove new line characters
			emul_lines = [line.replace('\n','') for line in f.readlines() if line]

		return emul_lines
		

	def get_timestamps(self):

		timestamp_p = "[0-9]+\.[0-9]+"
		timestamps = re.findall(timestamp_p, ' '.join(self.emul_lines))
		return timestamps



	def run_test(self):

		msg = "WARNING: No error handling is done. The script assumes the trace\n"
		msg += "file has no spelling errors.\n"
		# msg += " "
		print "\n----------Start of Testing------------"
		print msg

		# test timestamp
		self.valid_start_time()
		self.is_monotonic()

		# generate packet objects and validate their attributes
		self.parse_packets()
		self.validate_packets()

		# extract the stats
		self.parse_stats("parsed")

		# recompute the stats


		# test 


		

	def parse_packets(self):

		for line in self.emul_lines:

			packetID = self.get_packetID(line)
			if packetID != -1: 
				# if packet already exits, update its fields
				# otherwise, create a new packet object
				packet_obj = self.packets.get(packetID)
				if not packet_obj:
					packet_obj = Packet(packetID=packetID)
					self.packets[packetID] = packet_obj
				self.update_packet(packet_obj, line)

	





	def parse_stats(self, stat_type):

		stat_obj = Stat()
		for line in self.emul_lines:
			if "average packet inter-arrival time" in line:
				stat_obj.packet_ia = self.parse_float(line)
			elif "average packet service time" in line:
				stat_obj.service_time = self.parse_float(line)
			elif "average number of packets in Q1" in line:
				stat_obj.num_p_q1 = self.parse_float(line)
			elif "average number of packets in Q2" in line:
				stat_obj.num_p_q2 = self.parse_float(line)
			elif "average number of packets in S1" in line:
				stat_obj.num_p_s1 = self.parse_float(line)
			elif "average number of packets in S2" in line:
				stat_obj.num_p_s2 = self.parse_float(line)
			elif "average time a packet spent in system" in line:
				stat_obj.sys_time = self.parse_float(line)
			elif "standard deviation for time spent in system" in line:
				stat_obj.stdv = self.parse_float(line)
			elif "token drop probability" in line:
				stat_obj.t_drop = self.parse_float(line)
			elif "packet drop probability" in line:
				stat_obj.p_drop = self.parse_float(line)

		self.stats[stat_type] = stat_obj
		print "packet_ia: ", stat_obj.packet_ia
		print "service_time: ", stat_obj.service_time
		print "num_p_q1: ", stat_obj.num_p_q1
		print "num_p_q2: ", stat_obj.num_p_q2
		print "num_p_s1: ", stat_obj.num_p_s1
		print "num_p_s2: ", stat_obj.num_p_s2
		print "sys_time: ", stat_obj.sys_time
		print "stdv: ", stat_obj.stdv
		print "t_drop: ", stat_obj.t_drop
		print "p_drop: ", stat_obj.p_drop 

	def get_packetID(self, line):

		packet_p = "p[0-9]+\s"
		packetID = re.findall(packet_p, line)
		if packetID:
			packetID = packetID[0].replace(" ", "")
		else:
			packetID = -1

		return packetID


	def parse_float(self, line):

		pattern_d = "\s=\s[0-9]+\.[0-9]+"
		value = re.findall(pattern_d, line)[0]
		value = float(value.replace("=", "").replace(" ", ""))

		return value


	def parse_duration(self, packet_obj, line, pattern, facility):

		second_facility = False

		duration_pattern = "requesting\s[0-9]+"
		pattern_d = "\s=\s[0-9]+\.[0-9]+"
		if "begins" in facility:
			pattern_d = duration_pattern

		duration = re.findall(pattern_d, line)[0]
		duration = duration.replace("requesting", "").replace(" ","").replace("=","")
		duration = int(duration) if "begins" in facility else float(duration)

		match = re.findall(pattern, line)
		if not match:
			second_facility = True

		if second_facility and facility == "queue":
			packet_obj.q2_duration = duration
		elif facility == "queue":
			packet_obj.q1_duration = duration

		elif "server" in facility:
			if second_facility:
				packet_obj.serverID = 2
			else:
				packet_obj.serverID = 1
			if "begins" in facility:
				packet_obj.service_time = duration
			else:
				packet_obj.service_time_m = duration


	def update_packet(self, packet_obj, line):

		"""
		Parse packet attributes and update the object. Note that 
		num_trace_lines is only incremented inside each conditional 
		block, not upon any functional call to update_packet to 
		ensure that each trace line strictly follows the assignment specs.
		"""
		can_print = False
		if "arrives" in line:
			token_count = self.get_p_tokens(line)
			packet_obj.tokens =  token_count

			inter_arrival = self.get_p_inter_arrival(line)
			packet_obj.ia = inter_arrival
			packet_obj.num_trace_lines += 1

		elif "enters" in line:
			packet_obj.num_trace_lines += 1

		elif "leaves" in line:

			q1_pattern = "\leaves\sQ1"
			self.parse_duration(packet_obj, line, q1_pattern, "queue")
			packet_obj.num_trace_lines += 1

		elif "begins" in line:
			s1_pattern = "\sS1"
			self.parse_duration(packet_obj, line, s1_pattern, "server-begins")
			packet_obj.num_trace_lines += 1

		elif "departs" in line: 
			can_print = True
			s1_pattern = "\sS1"
			self.parse_duration(packet_obj, line, s1_pattern, "server-departs")
			packet_obj.served = True
			packet_obj.num_trace_lines += 1

		elif "dropped" in line:
			# do not increment num_trace_lines
			packet_obj.dropped = True
			 
	
		# if can_print:
		# 	print "packetID: {}\ttoken count: {}\tia: {}\tq1: {}\tq2: {}\tserverID: {}\tst: {}\tst_m: {}\tlines: {}".format(
		# 		packet_obj.packetID, packet_obj.tokens,packet_obj.ia, packet_obj.q1_duration, 
		# 		packet_obj.q2_duration, packet_obj.serverID, packet_obj.service_time,
		# 		packet_obj.service_time_m, packet_obj.num_trace_lines)

	
	def get_p_tokens(self, line):
		# parse out the token count
		token_p = "(\s[0-9]+\stoken)"
		match = re.findall(token_p, line)
		if match:
			match = match[0].replace("tokens", "").replace("token","")
			match.replace(" ", "")
		return int(match)

	def get_p_inter_arrival(self, line):

		ia_pattern = "\s=\s[0-9]+\.[0-9]+"
		match = re.findall(ia_pattern, line)
		if match:
			match = match[0].replace("=","").replace(" ","")

		return float(match)






	
	def valid_start_time(self):

		star_str = "00000000.000"
		star_str_sorted = sorted(copy.deepcopy(self.raw_timestamps))[0]

		result = star_str == str(star_str_sorted)
		msg = "{} \tTimestamp: 00000000.000 start time"
		msg = msg.format(self.pass_or_fail(result))
		print msg




	def is_monotonic(self):

		sorted_timestamps = copy.deepcopy(self.raw_timestamps)
		sorted(sorted_timestamps) # in-place sorting, hence the copying

		result = self.raw_timestamps == sorted_timestamps
		timestamp_msg = "{} \tTimestamp: Monotonically non-decreasing"
		timestamp_msg = timestamp_msg.format(self.pass_or_fail(result))

		print timestamp_msg


	def validate_packets(self):

		NUM_LINES = 7
		fail = False
		for packetID, packet_obj in self.packets.items():
			if packet_obj.served:
				if packet_obj.num_trace_lines != NUM_LINES:
					fail = True
					break

		msg = "{}\tPackets: Each served packet as exactly 7 lines"
		msg = msg.format(self.pass_or_fail( not fail))
		print msg

	def pass_or_fail(self, value):

		return  "PASS" if value == True else "FAIL"



		


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


