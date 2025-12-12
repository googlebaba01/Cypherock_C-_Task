import net from "net";
import { loadPlainMessageType } from "../proto/message";

interface PlainMessageType {
  type: string;
  payload: Uint8Array;
}

export class TcpClient {
  constructor(
    private readonly host: string,
    private readonly port: number
  ) {}

  async run(): Promise<void> {
    const PlainMessage = await loadPlainMessageType();

    const socket = net.createConnection(
      { host: this.host, port: this.port },
      () => {
        console.log("[client] Connected to server");

        // 1️⃣ Build message object
        const msgObj = {
          type: "hello",
          payload: Buffer.from("from ts client")
        };

        // 2️⃣ Validate
        const err = PlainMessage.verify(msgObj);
        if (err) throw new Error(err);

        // 3️⃣ Encode protobuf
        const encoded = PlainMessage.encode(msgObj).finish();

        // 4️⃣ Prepend 4-byte length header
        const lenBuf = Buffer.alloc(4);
        lenBuf.writeUInt32BE(encoded.length);

        const finalBuf = Buffer.concat([lenBuf, encoded]);

        // 5️⃣ Send to server
        socket.write(finalBuf);
        console.log("[client] Sent protobuf:", finalBuf);
      }
    );

    let recvBuffer = Buffer.alloc(0);

    socket.on("data", async (chunk: Buffer) => {
      recvBuffer = Buffer.concat([recvBuffer, chunk]);

      // Wait for 4 bytes (length prefix)
      if (recvBuffer.length < 4) return;

      const msgLen = recvBuffer.readUInt32BE(0);

      if (recvBuffer.length < 4 + msgLen) return;

      const msgBuf = recvBuffer.slice(4, 4 + msgLen);

      const PlainMessage = await loadPlainMessageType();

      // 6️⃣ Decode protobuf
      const decoded = PlainMessage.decode(msgBuf) as unknown as PlainMessageType;

      console.log("[client] Received reply:");
      console.log(" type:", decoded.type);
      console.log(" payload:", Buffer.from(decoded.payload).toString());

      socket.end();
    });

    socket.on("end", () => console.log("[client] Connection ended by server"));
    socket.on("error", (err: Error) => console.error("[client] Error:", err.message));
  }
}
