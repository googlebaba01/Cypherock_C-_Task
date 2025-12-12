import path from "path";
import protobuf from "protobufjs";

export async function loadPlainMessageType() {
  const protoPath = path.join(
    __dirname,
    "..",
    "..",
    "proto",
    "message_client.proto"
  );

  const root = await protobuf.load(protoPath);
  return root.lookupType("cot.PlainMessage");
}
