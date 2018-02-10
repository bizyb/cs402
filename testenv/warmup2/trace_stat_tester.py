# TODO (pseudocode):
# 1. Timestamps
# 	- (LAST) verify that the timestamps are the result of adding the inter-arrival times, service times, etc.
 

import re
import copy
import decimal
import statistics

class Token(object):

	def __init__(self, *args, **kwargs):

		self.num_tokens = 0
		self.num_dropped = 0


class Packet(object):
	
	def __init__(self, *args, **kwargs):

		self.packetID = kwargs.get("packetID")
		self.ia = -1 #inter-arrival time
		self.tokens = -1
		self.q1_duration = -1
		self.q2_duration = -1
		self.service_time = -1
		self.service_time_m = -1 #measured
		self.arrival_t = -1
		self.departure_t = -1
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
		self.token_obj = Token()
		self.packets = {}
		self.stats = {}
		self.THOUSAND = 1000


	def parse_file(self):

		emul_lines = []
		with open(self.emul_file, "r") as f:
			# remove new line characters
			emul_lines = [line.replace('\n','') for line in f.readlines() if line]

		return emul_lines
		

	def get_timestamps(self):

		timestamp_p = "[0-9]{8}\.[0-9]{3}"
		timestamps = re.findall(timestamp_p, ' '.join(self.emul_lines))
		timestamps = [float(t) for t in timestamps]
		return timestamps



	def run_test(self):

		msg = "WARNING: No error handling is done. The script assumes the trace\n"
		msg += "file has no spelling errors. Not all failures are valid due to \n"
		msg += "precision and rounding errors.\n"
		# msg += " "
		print "\n----------Start of Testing-------------------------------------"
		print msg

		# test timestamp
		self.valid_start_time()
		self.is_monotonic()

		# generate packet objects and validate their attributes
		self.parse_packets()
		self.validate_packets()

		self.parse_tokens()

		# extract the stats
		self.parse_stats("parsed")

		# recompute the stats and validate them
		self.recompute_stats()
		self.validate_stats("packet_ia")
		self.validate_stats("service_time")
		self.validate_stats("num_p_q1")
		self.validate_stats("num_p_q2")
		self.validate_stats("num_p_s1")
		self.validate_stats("num_p_s2")
		self.validate_stats("sys_time")
		self.validate_stats("stdv")
		self.validate_stats("t_drop")
		self.validate_stats("p_drop")
 
		print "\n"

		print "----------End of Testing-------------------------------------\n"

	def parse_tokens(self):

		for line in self.emul_lines:
			token_p = "token\st[0-9]+\sarrives"
			if re.findall(token_p, line):
				self.token_obj.num_tokens += 1
				if "dropped" in line:
					self.token_obj.num_dropped += 1

	
	def recompute_stats(self):

		stat_obj = Stat()
		stat_obj.packet_ia = self.stat_packet_ia()
		stat_obj.service_time = self.stat_service_time()
		stat_obj.num_p_q1 = self.stat_avg_num("q1_duration")
		stat_obj.num_p_q2 = self.stat_avg_num("q2_duration")
		stat_obj.num_p_s1 = self.stat_avg_num("service_time_m", id=1)
		stat_obj.num_p_s2 = self.stat_avg_num("service_time_m", id=2)
		stat_obj.sys_time = self.stat_service_time(sys_time=True)
		stat_obj.stdv = self.stat_stdv()
		stat_obj.t_drop = self.stat_drop("token")
		stat_obj.p_drop = self.stat_drop("packet")

		self.stats["recomputed"] = stat_obj


	def stat_packet_ia(self):

		num_packets = len(self.packets.keys())
		total_time = 0
		for packetID, packet_obj in self.packets.items():
			total_time += packet_obj.ia
		ia = total_time/num_packets/self.THOUSAND
		return ia

	def stat_service_time(self, sys_time=False):

		num_packets = 0
		total_time = 0
		for packetID, packet_obj in self.packets.items():
			if packet_obj.served:
				if sys_time:
					total_time += packet_obj.departure_t - packet_obj.arrival_t
				else:
					total_time += packet_obj.service_time_m
				num_packets += 1
		st = total_time/num_packets/self.THOUSAND
		return st

	def stat_avg_num(self, facility, id=-1):

		total_time = 0
		for packetID, packet_obj in self.packets.items():
			if packet_obj.served:
				if id > 0 and packet_obj.serverID == id or id == -1:
					total_time += getattr(packet_obj, facility)
				
		avg_num = total_time/self.raw_timestamps[-1] # total emulation time == last timestamp
		return avg_num

	def stat_stdv(self):

		sys_times = []
		for packetID, packet_obj in self.packets.items():
			if packet_obj.served:
				sys_times.append(packet_obj.departure_t - packet_obj.arrival_t)
		stdv =  statistics.pstdev(sys_times)/self.THOUSAND # population stddev
		return stdv

	def stat_drop(self, drop_type):

		num_total = 0
		num_dropped = 0
		if drop_type == "packet":
			num_total = len(self.packets.keys())
			for packetID, packet_obj in self.packets.items():
				if packet_obj.dropped:
					num_dropped += 1

		elif drop_type == "token":
			num_total = self.token_obj.num_tokens
			num_dropped = self.token_obj.num_dropped


		return num_dropped/float(num_total)

	def parse_packets(self):

		TIMESTAMP_OFFSET = 7
		for index, line in enumerate(self.emul_lines):
			packetID = self.get_packetID(line)
			if packetID != -1: 
				# if packet already exits, update its fields
				# otherwise, create a new packet object
				packet_obj = self.packets.get(packetID)
				if not packet_obj:
					packet_obj = Packet(packetID=packetID)
					self.packets[packetID] = packet_obj
				self.update_packet(packet_obj, line, index - TIMESTAMP_OFFSET)

	
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


	def update_packet(self, packet_obj, line, line_num=-1):

		"""
		Parse packet attributes and update the object. Note that 
		num_trace_lines is only incremented inside each conditional 
		block, not upon any functional call to update_packet to 
		ensure that each trace line strictly follows the assignment specs.
		"""
		if "arrives" in line:
			token_count = self.get_p_tokens(line)
			packet_obj.tokens =  token_count
			packet_obj.arrival_t = self.raw_timestamps[line_num]


			inter_arrival = self.get_p_inter_arrival(line)
			packet_obj.ia = inter_arrival

			if "dropped" in line:
				packet_obj.dropped = True

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

			s1_pattern = "\sS1"
			self.parse_duration(packet_obj, line, s1_pattern, "server-departs")
			packet_obj.served = True
			packet_obj.departure_t = self.raw_timestamps[line_num]
			packet_obj.num_trace_lines += 1
			

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

		start_f = float("00000000.000")
		start_f_sorted = float(sorted(copy.deepcopy(self.raw_timestamps))[0])

		result = start_f == start_f_sorted
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

		msg = "{}\tPackets: Each served packet has exactly 7 lines"
		msg = msg.format(self.pass_or_fail( not fail))
		print msg

	def validate_stats(self, attr):

		msg = "{}\tStats: "
		if attr == "packet_ia":
			msg += "average packet inter-arrival time "
		elif attr == "service_time":
			msg += "average packet service time "
		elif attr == "num_p_q1":
			msg += "average number of packets in Q1 "
		elif attr == "num_p_q2":
			msg += "average number of packets in Q2 "
		elif attr == "num_p_s1":
			msg += "average number of packets in S1 "
		elif attr == "num_p_s2":
			msg += "average number of packets in S2 "
		elif attr == "sys_time":
			msg += "average time a packet spent in system "
		elif attr == "stdv":
			msg += "standard deviation for time spent in system "
		elif attr == "t_drop":
			msg += "token drop probability "
		elif attr == "p_drop":
			msg += "packet drop probability "

		msg += "[parsed={} recomputed={}]"

		# parameterize attribute access with getattr
		parsed = getattr(self.stats.get("parsed"), attr)
		recomputed = getattr(self.stats.get("recomputed"), attr)
		recomputed_str = "{:.6f}".format(recomputed)

		# compare the rounded float value to the parsed value to avoid precision 
		# errors
		result = parsed == float(recomputed_str)
		msg = msg.format(self.pass_or_fail(result), parsed, recomputed)
		print msg



	def pass_or_fail(self, value):

		return  "PASS" if value == True else "FAIL"

#-------------------------------------------------------------------------------------#
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


