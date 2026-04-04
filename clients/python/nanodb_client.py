import socket


class NanoDBClient:
    def __init__(self, host='127.0.0.1', port=8080) -> None:
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((host, port))

    def _send_command(self, cmd_str: str):
        cmd_str += "\n"
        self.sock.sendall(cmd_str.encode('utf-8'))
        response = self.sock.recv(4096).decode('utf-8')
        return response

    def insert(self, ts: int, val: float):
        response = self._send_command(f"insert {ts} {val}")

        if response == "OK\n":
            return True
        return False

    def query(self, start: int, end: int):
        response = self._send_command(f"query {start} {end}")

        if response == "(no results)":
            return []

        res_list: list[tuple[int, float]] = []

        for line in response.split("\n")[:-2]:
            first, second = line.split(": ")
            res_list.append((int(first), float(second)))

        return res_list

    def close(self):
        self.sock.close()
