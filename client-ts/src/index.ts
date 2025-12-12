import { TcpClient } from "./network/TcpClient";

function main() {
  const client = new TcpClient("127.0.0.1", 9000);
  client.run();
}

main();
