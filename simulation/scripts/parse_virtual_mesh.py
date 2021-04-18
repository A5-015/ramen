import re
import subprocess

time = 1
kill_leader_at_time = 0
n_nodes = 3
n_logs_to_append = 1

command = f"cd ../../library && cmake . && make virtual_esp && ./bin/virtual_esp -t {time} -n {n_nodes} -l {n_logs_to_append} -k {kill_leader_at_time}"
output = subprocess.check_output(command, shell=True, text=True)

result = re.findall("(.*(?: @ ).*)", output)

print(result)
