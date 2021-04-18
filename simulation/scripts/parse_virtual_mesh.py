import re
import subprocess

# time = 60
# kill_leader_at_time = 10
# n_nodes = 5
# n_logs_to_append = 20

# command = f"cd ../../library && cmake . && make virtual_esp && ./bin/virtual_esp -t {time} -n {n_nodes} -l {n_logs_to_append} -k {kill_leader_at_time}"
# output = subprocess.check_output(command, shell=True, text=True)

# f = open("output.txt", "a")
# f.write(output)
# f.close()

f = open("output.txt", "r")
output = f.read()

result = re.findall("(.*(?: @ ).*)", output)

for line in result:
    print(line)
