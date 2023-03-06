import re
from typing import List

FORMAT = r"Task: (\d+) | Delay: (\d+) | Counter: (\d+)"

def tl_key(line: str) -> int:
    if not re.match(FORMAT, line):
        raise ValueError("Invalid line format")
    
    # Get the three numbers out
    _, delay, counter = re.findall(FORMAT, line)
    return int(delay[1]) * int(counter[2])

def verify(lines: List[str]) -> bool:
    task_lines = [l for l in lines if re.match(FORMAT, l)]
    return task_lines == sorted(task_lines, key=tl_key)

if __name__ == "__main__":
    with open("k3_out.txt") as f:
        lines = f.readlines()

    print(verify(lines))
