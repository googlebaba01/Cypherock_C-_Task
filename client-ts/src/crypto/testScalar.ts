import { randomScalar, addModN, mulModN } from "./cryptoUtils";

function toHex(buf: Buffer): string {
  return buf.toString("hex");
}

console.log("=== Scalar Test (TS client) ===");

const a = randomScalar();
const b = randomScalar();

const sum = addModN(a, b);
const mul = mulModN(a, b);

console.log("a      =", toHex(a));
console.log("b      =", toHex(b));
console.log("a + b  =", toHex(sum));
console.log("a * b  =", toHex(mul));
