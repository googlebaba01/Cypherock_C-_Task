import crypto from "crypto";

export type Scalar32 = Buffer;

// Order n of secp256k1 as BigInt
const N = BigInt("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141");

export function randomScalar(): Scalar32 {
  while (true) {
    const buf = crypto.randomBytes(32);
    const x = BigInt("0x" + buf.toString("hex"));
    if (x >= 1n && x < N) return buf;
  }
}

function bufToBigInt(b: Buffer): bigint {
  return BigInt("0x" + b.toString("hex"));
}

function bigIntToBuf(x: bigint): Buffer {
  let hex = x.toString(16);
  if (hex.length % 2 === 1) hex = "0" + hex;
  const buf = Buffer.from(hex, "hex");
  if (buf.length === 32) return buf;
  if (buf.length > 32) return buf.subarray(buf.length - 32);
  const out = Buffer.alloc(32, 0);
  buf.copy(out, 32 - buf.length);
  return out;
}

export function addModN(a: Scalar32, b: Scalar32): Scalar32 {
  const res = (bufToBigInt(a) + bufToBigInt(b)) % N;
  return bigIntToBuf(res);
}

export function mulModN(a: Scalar32, b: Scalar32): Scalar32 {
  const res = (bufToBigInt(a) * bufToBigInt(b)) % N;
  return bigIntToBuf(res);
}
