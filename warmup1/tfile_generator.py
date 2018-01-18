
import random
import copy
'''
output file format (matching regex: (\-|\+)(\t\d{1,10}\t\d{1,7}\.\d{2}\t\w+))

-	1230728833	45.33	Phone bill
-	1263142433	654.32	Beemer monthly payment
+	1247542433	10388.07	Dear parents
+	1219356033	1723.00	Initial deposit


'''
NUM_TRANSACTIONS = 2
MAX_LINE_LENGTH = 200
dates = {}

def getFile():

	

	raw_text = []
	clean_text = []
	with open("lorem_ipsum.txt", "r") as f:

		raw_text = f.readlines()

	for sentence in raw_text:

		sentence.replace('\t', ' ').replace('\n', ' ')
		sentence[0].replace(" ","")
		clean_text.extend(sentence.split("."))

	return clean_text


def getFlag():

	return random.choice(['-', '+'])

def getDate():
	
	# avoid duplicate timestamp
	while True:
		d = random.randint(0, 9999999999)
		if not dates.get(d):
			dates[d] = d
			return d
		

def getAmount():
	
	return "{:.2f}".format(random.uniform(0, 9999999))


def getDesc(text):

	while(True): 

		t = random.choice(text)
		if len(t) > 1:
			# Ignore leading space
			t = " ".join([c for c in t.split() if c])
			# return a description at least one character or longer
			return t


def generateFile():

	rows = []
	file_name = "tfile_{}.txt".format(NUM_TRANSACTIONS)
	text = getFile()

	for i in range(NUM_TRANSACTIONS):

		row = "{}\t{}\t{}\t{}\n".format(getFlag(), getDate(), getAmount(),
											getDesc(text))
		rows.append(row)


	random.shuffle(rows)
	with open(file_name, "w") as f:

		for row in rows:

			f.write(row.encode("utf-8"))

generateFile()







