# this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

from typing import List, Dict

class Properties:
    def __init__(self):
        self._pairs = {}

    def set(self, key: str, value: int):
        self._pairs[key] = str(value)

    def set(self, key: str, value: int):
        self._pairs[key] = str(value)

    def set(self, key: str, value: int):
        self._pairs[key] = str(value)

    def set(self, key: str, value: bool):
        self._pairs[key] = "1" if value else "0"

    def set(self, key: str, value: float):
        self._pairs[key] = f"{value:.10f}"

    def set(self, key: str, value: str):
        self._pairs[key] = value

    def set(self, key: str, value: List[float]):
        self._pairs[key] = " ".join(map(str, value))

    def exist(self, key: str) -> bool:
        return key in self._pairs

    def get(self, key: str) -> str:
        return self._pairs.get(key, "")

    def get_int(self, key: str) -> int:
        return int(self._pairs.get(key, 0))

    def get_unsigned_int(self, key: str) -> int:
        return max(0, int(self._pairs.get(key, 0)))

    def get_long(self, key: str) -> int:
        return int(self._pairs.get(key, 0))

    def get_bool(self, key: str) -> bool:
        return self._pairs.get(key, "0") != "0"

    def get_double(self, key: str) -> float:
        return float(self._pairs.get(key, 0.0))

    def get_vector_double(self, key: str) -> List[float]:
        value = self.get(key)
        if not value:
            return []
        return list(map(float, value.split()))

    def save(self, file_name: str) -> bool:
        try:
            with open(file_name, 'w') as file:
                for key, value in self._pairs.items():
                    if value:
                        for line in value.splitlines():
                            file.write(f"{key}={line}\n")
                    else:
                        file.write(f"{key}=\n")
            return True
        except Exception:
            return False

    def load(self, file_name: str) -> bool:
        self._pairs.clear()
        try:
            with open(file_name, 'r') as file:
                for line in file:
                    line = line.strip()
                    if not line:
                        continue
                    if "=" not in line:
                        continue

                    key, value = line.split("=", 1)
                    if self.exist(key):
                        self.set(key, self.get(key) + '\n' + value)
                    else:
                        self.set(key, value)
            return True
        except Exception:
            return False

    def remove(self, key: str):
        self._pairs.pop(key, None)

    def all(self) -> Dict[str, str]:
        return self._pairs