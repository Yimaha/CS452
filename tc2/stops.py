
from typing import List

STOPS = """
a11, a12, b7, b8, b11, b12, b9, b10, c3, c4, e11, e12,
e5, e6, d1, d2, e1, e2, c1, c2, b13, b14, e13, e14, c9, c10, c5, c6,
c11, c12, e7, e8, c13, c14, a1, a2, a13, a14, a15, a16
"""

def sensor_to_index(sensor: str) -> int:
    """
    Convert a sensor name to a 0-based index.
    """
    return 16 * (ord(sensor[0]) - ord("a")) + int(sensor[1:]) - 1

def stops_to_indices(stops: str) -> List[int]:
    """
    Convert a string of comma-separated sensor names
    to a list of 0-based indices.
    """
    return [sensor_to_index(s.strip()) for s in stops.split(",")]

if __name__ == "__main__":
    l = sorted(stops_to_indices(STOPS))
    s = ", ".join([str(i) for i in l])
    print(f"const int STOPS[] = {{{s}}};")

    x = ["true" if i in l else "false" for i in range(144)]
    s = ", ".join(x)
    print(f"const bool STOP_CHECK[] = {{{s}}};")